#include "GeometryObject.h"
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
	connect(this, &CGeometryObject::OnRecalculated, callback);

	Object		= QJsonDocument::fromJson(jsonByteArr).object();
	MeshStats	= SMeshStats();

	this->Recalculate();
}

void CGeometryObject::Wait(TPromise callback)
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
	QJsonArray vertices = this->GetVerticesRaw();
	return vertices.count() / 3;
}

QVector3D CGeometryObject::GetVertex(qsizetype vertexIndex) const
{
	QVector3D result;

	QJsonArray vertices = this->GetVerticesRaw();
	result.setX(vertices.at(vertexIndex * 3).toDouble());
	result.setY(vertices.at(vertexIndex * 3 + 1).toDouble());
	result.setZ(vertices.at(vertexIndex * 3 + 2).toDouble());

	return result;
}

qsizetype CGeometryObject::GetTrianglesCount() const
{
	QJsonArray triangles = this->GetTrianglesRaw();
	return triangles.count() / 3;
}

STriangle CGeometryObject::GetTriangle(qsizetype triangleIndex) const
{
	STriangle result;

	std::vector<qsizetype> indices = this->GetTriangleVerticesIDs(triangleIndex);
	result.Vertices[0] = this->GetVertex(indices[0]);
	result.Vertices[1] = this->GetVertex(indices[1]);
	result.Vertices[2] = this->GetVertex(indices[2]);

	return result;
}

std::vector<qsizetype> CGeometryObject::GetTriangleVerticesIDs(qsizetype triangleIndex) const
{
	std::vector<qsizetype> result;

	QJsonArray triangles = this->GetTrianglesRaw();
	result.push_back(triangles.at(triangleIndex * 3).toInt());
	result.push_back(triangles.at(triangleIndex * 3 + 1).toInt());
	result.push_back(triangles.at(triangleIndex * 3 + 2).toInt());

	return result;
}

SMeshStats CGeometryObject::GetStats() const
{
	return this->MeshStats;
}

void CGeometryObject::Recalculate()
{
	bRecalculating = true;

	MeshStats.TrianglesCount = this->GetTrianglesCount();

	// We want to use ALL available threads
	int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	qsizetype trianglesCount		= this->GetTrianglesCount();
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

		CMeshAnalyzer *worker = new CMeshAnalyzer(*this, beginIndex, endIndex);
		worker->setObjectName("StatsWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		AnalyzeWorkers.insert(worker);
		connect(worker, &CMeshAnalyzer::Finished, this, &CGeometryObject::MeshAnalyzerFinished, Qt::QueuedConnection);

		QThreadPool::globalInstance()->start(worker);
	}
}

QJsonObject CGeometryObject::GetInnerObject() const
{
    QJsonObject result;

    auto objectIt = Object.find("geometry_object");
    if(objectIt != Object.end())
    {
        result = objectIt.value().toObject();
    }

	return result;
}

QJsonArray CGeometryObject::GetVerticesRaw() const
{
	QJsonArray result;
	QJsonObject innerObject = this->GetInnerObject();

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
	QJsonObject innerObject = this->GetInnerObject();

	auto verticesIt = innerObject.find("triangles");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

void CGeometryObject::MeshAnalyzerFinished()
{
	AnalyzeWorkers.remove(sender());

	// Call callback only when all workers are done
	if(AnalyzeWorkers.isEmpty())
	{
		bRecalculating = false;
		emit OnRecalculated();
	}
}
