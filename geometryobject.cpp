#include "GeometryObject.h"
#include "MeshInitializer.h"
#include "MeshAnalyzer.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QThreadPool>
#include <QThread>
#include <QMutexLocker>

CGeometryObject::CGeometryObject()
{}

void CGeometryObject::Init(const QByteArray &jsonByteArr, TPromise callback)
{
	EdgeList.Clear();
	RawData		= QJsonDocument::fromJson(jsonByteArr).object();

	MinTriangleArea	= std::numeric_limits<double>().max();
	MaxTriangleArea	= std::numeric_limits<double>().min();
	TotalArea		= 0.0f;
	bIsClosed		= true;

	connect(this, &CGeometryObject::OnRecalculated, callback);

	this->Recalculate();
}

void CGeometryObject::Wait(TPromise callback) const
{
	if(bRecalculating)
	{
		connect(this, &CGeometryObject::OnRecalculated, callback);
	}
	else
	{
		callback();
	}
}

qsizetype CGeometryObject::GetVerticesCount() const
{
	return EdgeList.GetVerticesCount();
}

SVertex CGeometryObject::GetVertex(qsizetype vertexIndex) const
{
	SVertex result;
	TDCEL_VertPtr vertex = EdgeList.GetVertex(vertexIndex);
	result.Location = vertex->Get();
	result.Normal	= vertex->GetNormal();
	return result;
}

qsizetype CGeometryObject::GetTrianglesCount() const
{
	return EdgeList.GetFacesCount();
}

STriangle CGeometryObject::GetTriangle(qsizetype triangleIndex) const
{
	STriangle result;
	TDCEL_FacePtr face = EdgeList.GetFace(triangleIndex);
	if(face)
	{
		result = face->Get();
	}
	return result;
}

qsizetype CGeometryObject::GetTriangleVertexIndex(qsizetype triangleIndex, qsizetype vertexNum) const
{
	qsizetype result;
	vertexNum = std::min<qsizetype>(vertexNum, 2);

	QJsonArray triangles = this->GetTrianglesRaw();
	result = triangles.at(triangleIndex * 3 + vertexNum).toInt();

	return result;
}

double CGeometryObject::GetMinTriangleArea() const
{
	return MinTriangleArea;
}

double CGeometryObject::GetMaxTriangleArea() const
{
	return MaxTriangleArea;
}

double CGeometryObject::GetTotalArea() const
{
	return TotalArea;
}

bool CGeometryObject::IsClosed() const
{
	return bIsClosed;
}

QJsonObject CGeometryObject::GetRawData() const
{
	QJsonObject result;

	auto objectIt = RawData.find("geometry_object");
	if(objectIt != RawData.end())
	{
		result = objectIt.value().toObject();
	}

	return result;
}

QJsonArray CGeometryObject::GetVerticesRaw() const
{
	QJsonArray result;
	QJsonObject innerObject = this->GetRawData();

	auto verticesIt = innerObject.find("vertices");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

QJsonArray CGeometryObject::GetTrianglesRaw() const
{
	QJsonArray result;
	QJsonObject innerObject = this->GetRawData();

	auto verticesIt = innerObject.find("triangles");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

qsizetype CGeometryObject::GetVerticesCountRaw() const
{
	QJsonArray vertices = this->GetVerticesRaw();
	return vertices.count() / 3;
}

QVector3D CGeometryObject::GetVertexRaw(qsizetype vertexIndex) const
{
	QVector3D result;

	QJsonArray vertices = this->GetVerticesRaw();
	result.setX(vertices.at(vertexIndex * 3).toDouble());
	result.setY(vertices.at(vertexIndex * 3 + 1).toDouble());
	result.setZ(vertices.at(vertexIndex * 3 + 2).toDouble());

	return result;
}

qsizetype CGeometryObject::GetTrianglesCountRaw() const
{
	QJsonArray triangles = this->GetTrianglesRaw();
	return triangles.count() / 3;
}

STriangle CGeometryObject::GetTriangleRaw(qsizetype triangleIndex) const
{
	STriangle result;

	result.Vertices[0] = this->GetVertexRaw(this->GetTriangleVertexIndexRaw(triangleIndex, 0));
	result.Vertices[1] = this->GetVertexRaw(this->GetTriangleVertexIndexRaw(triangleIndex, 1));
	result.Vertices[2] = this->GetVertexRaw(this->GetTriangleVertexIndexRaw(triangleIndex, 2));

	return result;
}

qsizetype CGeometryObject::GetTriangleVertexIndexRaw(qsizetype triangleIndex, qsizetype vertexNum) const
{
	qsizetype result;
	vertexNum = std::min<qsizetype>(vertexNum, 2);

	QJsonArray triangles = this->GetTrianglesRaw();
	result = triangles.at(triangleIndex * 3 + vertexNum).toInt();

	return result;
}

void CGeometryObject::Recalculate()
{
	bRecalculating = true;
	this->StartInitializers();
}

void CGeometryObject::StartInitializers()
{
	// We want to use ALL available threads
	int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	qsizetype trianglesCount		= this->GetTrianglesCountRaw();
	qsizetype trianglesPerThread	= trianglesCount / maxThreadCount;
	qsizetype remainder				= trianglesCount % maxThreadCount;

	qsizetype lastEndIndex			= 0;
	for(int i = 0; i < maxThreadCount; i++)
	{
		// remainder can never be more than the divisor(num of threads), so this is a safe and efficent way to distribute triangles
		qsizetype trianglesForThread = trianglesPerThread;
		if(remainder > 0)
		{
			trianglesForThread++;
			remainder--;
		}

		qsizetype beginIndex	= lastEndIndex;
		qsizetype endIndex		= beginIndex + trianglesForThread;
		lastEndIndex			= endIndex;

		CMeshInitializer *worker = new CMeshInitializer(*this, beginIndex, endIndex);
		worker->setObjectName("InitializerWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		Workers.insert(worker);
		connect(worker, &CMeshInitializer::Finished, this, &CGeometryObject::MeshInitializerFinished, Qt::QueuedConnection);

		QThreadPool::globalInstance()->start(worker);
	}
}

void CGeometryObject::StartAnalyzers()
{
	// We want to use ALL available threads
	int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	qsizetype trianglesCount		= this->GetTrianglesCount();
	qsizetype trianglesPerThread	= trianglesCount / maxThreadCount;
	qsizetype remainder				= trianglesCount % maxThreadCount;

	qsizetype lastEndIndex			= 0;
	for(int i = 0; i < maxThreadCount; i++)
	{
		// remainder can never be more than the divisor(num of threads), so this is a safe and efficent way to distribute work
		qsizetype edgesForThread = trianglesPerThread;
		if(remainder > 0)
		{
			edgesForThread++;
			remainder--;
		}

		qsizetype beginIndex	= lastEndIndex;
		qsizetype endIndex		= beginIndex + edgesForThread;
		lastEndIndex			= endIndex;

		CMeshAnalyzer *worker = new CMeshAnalyzer(*this, beginIndex, endIndex);
		worker->setObjectName("AnalyzerWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		Workers.insert(worker);
		connect(worker, &CMeshAnalyzer::Finished, this, &CGeometryObject::MeshAnalyzerFinished, Qt::QueuedConnection);

		QThreadPool::globalInstance()->start(worker);
	}
}

void CGeometryObject::MeshInitializerFinished()
{
	Workers.remove(sender());

	// Init is complete, now analyze the mesh
	if(Workers.isEmpty())
	{
		this->StartAnalyzers();
	}
}

void CGeometryObject::MeshAnalyzerFinished()
{
	Workers.remove(sender());

	// Call callback only when all workers are done
	if(Workers.isEmpty())
	{
		bRecalculating = false;
		emit OnRecalculated();
	}
}
