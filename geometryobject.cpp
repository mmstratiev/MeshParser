#include "GeometryObject.h"
#include "MeshStatsWorker.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QThreadPool>
#include <QThread>

CGeometryObject::CGeometryObject()
{}

CGeometryObject::CGeometryObject(const QByteArray& jsonByteArr)
{
	this->Init(jsonByteArr);
}

void CGeometryObject::Init(const QByteArray &jsonByteArr)
{
	Object = QJsonDocument::fromJson(jsonByteArr).object();

	MeshStats = SMeshStats();
	MeshStats.TrianglesCount = this->GetTrianglesCount();
}

QJsonArray CGeometryObject::GetVertices() const
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

QJsonArray CGeometryObject::GetTriangles() const
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

qsizetype CGeometryObject::GetVerticesCount() const
{
	QJsonArray vertices = this->GetVertices();
	return vertices.count() / 3;
}

QVector3D CGeometryObject::GetVertex(qsizetype vertexIndex) const
{
	QVector3D result;

	QJsonArray vertices = this->GetVertices();
	result.setX(vertices.at(vertexIndex * 3).toDouble());
	result.setY(vertices.at(vertexIndex * 3 + 1).toDouble());
	result.setZ(vertices.at(vertexIndex * 3 + 2).toDouble());

	return result;
}

qsizetype CGeometryObject::GetTrianglesCount() const
{
	QJsonArray triangles = this->GetTriangles();
	return triangles.count() / 3;
}

STriangle CGeometryObject::GetTriangle(qsizetype triangleIndex) const
{
	STriangle result;

	QJsonArray triangles = this->GetTriangles();
	result.Vertices[0] = this->GetVertex(triangles.at(triangleIndex * 3).toInt());
	result.Vertices[1] = this->GetVertex(triangles.at(triangleIndex * 3 + 1).toInt());
	result.Vertices[2] = this->GetVertex(triangles.at(triangleIndex * 3 + 2).toInt());

	return result;
}

void CGeometryObject::GetStats(TGetStatsCallback funcCallback)
{
	// We want to use ALL available threads
	int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	qsizetype trianglesCount		= this->GetTrianglesCount();
	qsizetype trianglesPerThread	= trianglesCount / maxThreadCount;
	qsizetype remainder				= trianglesCount % maxThreadCount;

	qsizetype lastEndIndex			= 0;

	for(int i = 0; i < maxThreadCount; i++)
	{
		// remainder can never be more than the divisor(num of threads), so this is a safe and very efficent way to distribute triangles
		qsizetype trianglesForThread = trianglesPerThread;
		if(remainder > 0)
		{
			trianglesForThread++;
			remainder--;
		}

		qsizetype beginIndex	= lastEndIndex;
		qsizetype endIndex		= beginIndex + trianglesForThread;
		lastEndIndex			= endIndex;

		MeshStatsWorker *worker = new MeshStatsWorker(*this, beginIndex, endIndex);
		worker->SetMutex(&Mutex);
		worker->SetOutput(&MeshStats);
		worker->SetCallback(funcCallback);

		worker->setObjectName("StatsWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		Workers.insert(worker);

		connect(worker, &MeshStatsWorker::Finished, this, &CGeometryObject::CalcStatsFinished, Qt::QueuedConnection);

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

void CGeometryObject::CalcStatsFinished(TGetStatsCallback funcCallback)
{
	Workers.remove(sender());

	// Call callback only when all workers are done
	if(Workers.isEmpty())
	{
		funcCallback(MeshStats);
	}
}
