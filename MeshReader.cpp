#include "MeshReader.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>

CMeshReader::CMeshReader(CGeometryObject& inOutObject, const QJsonObject& jsonDataObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, DataObject(jsonDataObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{
	//qInfo() << "Created" << this << QThread::currentThread();
}

CMeshReader::~CMeshReader()
{
	//qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshReader::run()
{
	// Starting the thread
	//qInfo() << "Starting" << this << QThread::currentThread();

	this->Work();

	//qInfo() << "Finishing" << this << QThread::currentThread();
	emit Finished();

	// todo: remove auto deletion and this sleep
	QThread::currentThread()->msleep(100);
	// Finishing the thread
}

void CMeshReader::Work()
{
	//qInfo() << BeginIndex << EndIndex  << this;
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{		
		// Skip degenerate triangles. They add no value to the object.
		if(CMeshReader::GetTriangleRaw(DataObject, triangleIndex).IsDegenerate()) return;

		TDCEL_VertID vert1ID(CMeshReader::GetTriangleVertexIndexRaw(DataObject, triangleIndex, 0));
		TDCEL_VertID vert2ID(CMeshReader::GetTriangleVertexIndexRaw(DataObject, triangleIndex, 1));
		TDCEL_VertID vert3ID(CMeshReader::GetTriangleVertexIndexRaw(DataObject, triangleIndex, 2));

		QMutexLocker locker(&GeometryObject.Mutex);
		GeometryObject.EdgeList.AddVertex(vert1ID, CMeshReader::GetVertexRaw(DataObject, vert1ID));
		GeometryObject.EdgeList.AddVertex(vert2ID, CMeshReader::GetVertexRaw(DataObject, vert2ID));
		GeometryObject.EdgeList.AddVertex(vert3ID, CMeshReader::GetVertexRaw(DataObject, vert3ID));

		GeometryObject.EdgeList.Connect(vert1ID, vert2ID, vert3ID, triangleIndex);
	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

QJsonObject CMeshReader::GetRawData(const QJsonObject& jsonDataObject)
{
	QJsonObject result;

	auto objectIt = jsonDataObject.find("geometry_object");
	if(objectIt != jsonDataObject.end())
	{
		result = objectIt.value().toObject();
	}

	return result;
}

QJsonArray CMeshReader::GetVerticesRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray result;
	QJsonObject innerObject = CMeshReader::GetRawData(jsonDataObject);

	auto verticesIt = innerObject.find("vertices");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

QJsonArray CMeshReader::GetTrianglesRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray result;
	QJsonObject innerObject = CMeshReader::GetRawData(jsonDataObject);

	auto verticesIt = innerObject.find("triangles");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

qsizetype CMeshReader::GetVerticesCountRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray vertices = CMeshReader::GetVerticesRaw(jsonDataObject);
	return vertices.count() / 3;
}

QVector3D CMeshReader::GetVertexRaw(const QJsonObject& jsonDataObject, qsizetype vertexIndex)
{
	QVector3D result;

	QJsonArray vertices = CMeshReader::GetVerticesRaw(jsonDataObject);
	result.setX(vertices.at(vertexIndex * 3).toDouble());
	result.setY(vertices.at(vertexIndex * 3 + 1).toDouble());
	result.setZ(vertices.at(vertexIndex * 3 + 2).toDouble());

	return result;
}

qsizetype CMeshReader::GetTrianglesCountRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray triangles = CMeshReader::GetTrianglesRaw(jsonDataObject);
	return triangles.count() / 3;
}

STriangle CMeshReader::GetTriangleRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex)
{
	STriangle result;

	result.Vertices[0] = CMeshReader::GetVertexRaw(jsonDataObject, CMeshReader::GetTriangleVertexIndexRaw(jsonDataObject, triangleIndex, 0));
	result.Vertices[1] = CMeshReader::GetVertexRaw(jsonDataObject, CMeshReader::GetTriangleVertexIndexRaw(jsonDataObject, triangleIndex, 1));
	result.Vertices[2] = CMeshReader::GetVertexRaw(jsonDataObject, CMeshReader::GetTriangleVertexIndexRaw(jsonDataObject, triangleIndex, 2));

	return result;
}

qsizetype CMeshReader::GetTriangleVertexIndexRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex, qsizetype vertexNum)
{
	qsizetype result;
	vertexNum = std::min<qsizetype>(vertexNum, 2);

	QJsonArray triangles = CMeshReader::GetTrianglesRaw(jsonDataObject);
	result = triangles.at(triangleIndex * 3 + vertexNum).toInt();

	return result;
}
