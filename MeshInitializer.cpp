#include "MeshInitializer.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>
#include <QQuaternion>

CMeshInitializer::CMeshInitializer(CGeometryObject& inOutObject, const QJsonObject& jsonDataObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, DataObject(jsonDataObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{}

void CMeshInitializer::run()
{
	this->Work();
	emit Finished();
	QThread::currentThread()->msleep(25);
}

void CMeshInitializer::Work()
{
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		// Cull degenerate triangles. They add no value to the object and impact performance.
		CTriangle triangle = CMeshInitializer::GetTriangleRaw(DataObject, triangleIndex);
		if(triangle.IsDegenerate())
		{
			GeometryObject.TempMaxProgress--;
		}
		else
		{
			GeometryObject.BoundingBox.ExtendTo(triangle.Vertices(0));
			GeometryObject.BoundingBox.ExtendTo(triangle.Vertices(1));
			GeometryObject.BoundingBox.ExtendTo(triangle.Vertices(2));

			TDCEL_VertID vert1ID(CMeshInitializer::GetTriangleVertexIndexRaw(DataObject, triangleIndex, 0));
			TDCEL_VertID vert2ID(CMeshInitializer::GetTriangleVertexIndexRaw(DataObject, triangleIndex, 1));
			TDCEL_VertID vert3ID(CMeshInitializer::GetTriangleVertexIndexRaw(DataObject, triangleIndex, 2));

			QMutexLocker locker(&GeometryObject.Mutex);
			GeometryObject.EdgeList.AddVertex(vert1ID, CMeshInitializer::GetVertexRaw(DataObject, vert1ID));
			GeometryObject.EdgeList.AddVertex(vert2ID, CMeshInitializer::GetVertexRaw(DataObject, vert2ID));
			GeometryObject.EdgeList.AddVertex(vert3ID, CMeshInitializer::GetVertexRaw(DataObject, vert3ID));

			GeometryObject.EdgeList.Connect(vert1ID, vert2ID, vert3ID);
			GeometryObject.TempProgress++;
		}
		emit MadeProgress();
	}
}

QJsonObject CMeshInitializer::GetRawData(const QJsonObject& jsonDataObject)
{
	QJsonObject result;

	auto objectIt = jsonDataObject.find("geometry_object");
	if(objectIt != jsonDataObject.end())
	{
		result = objectIt.value().toObject();
	}

	return result;
}

QJsonArray CMeshInitializer::GetVerticesRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray result;
	QJsonObject innerObject = CMeshInitializer::GetRawData(jsonDataObject);

	auto verticesIt = innerObject.find("vertices");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

QJsonArray CMeshInitializer::GetTrianglesRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray result;
	QJsonObject innerObject = CMeshInitializer::GetRawData(jsonDataObject);

	auto verticesIt = innerObject.find("triangles");
	if(verticesIt != innerObject.end())
	{
		result = verticesIt.value().toArray();
	}

	return result;
}

qsizetype CMeshInitializer::GetVerticesCountRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray vertices = CMeshInitializer::GetVerticesRaw(jsonDataObject);
	return vertices.count() / 3;
}

QVector3D CMeshInitializer::GetVertexRaw(const QJsonObject& jsonDataObject, qsizetype vertexIndex)
{
	QVector3D result;

	QJsonArray vertices = CMeshInitializer::GetVerticesRaw(jsonDataObject);
	result.setX(vertices.at(vertexIndex * 3).toDouble());
	result.setY(vertices.at(vertexIndex * 3 + 1).toDouble());
	result.setZ(vertices.at(vertexIndex * 3 + 2).toDouble());

	return result;
}

qsizetype CMeshInitializer::GetTrianglesCountRaw(const QJsonObject& jsonDataObject)
{
	QJsonArray triangles = CMeshInitializer::GetTrianglesRaw(jsonDataObject);
	return triangles.count() / 3;
}

CTriangle CMeshInitializer::GetTriangleRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex)
{
	CTriangle result;

	QVector3D vert1 = CMeshInitializer::GetVertexRaw(jsonDataObject, CMeshInitializer::GetTriangleVertexIndexRaw(jsonDataObject, triangleIndex, 0));
	QVector3D vert2 = CMeshInitializer::GetVertexRaw(jsonDataObject, CMeshInitializer::GetTriangleVertexIndexRaw(jsonDataObject, triangleIndex, 1));
	QVector3D vert3 = CMeshInitializer::GetVertexRaw(jsonDataObject, CMeshInitializer::GetTriangleVertexIndexRaw(jsonDataObject, triangleIndex, 2));

	return CTriangle(vert1, vert2, vert3);
}

qsizetype CMeshInitializer::GetTriangleVertexIndexRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex, qsizetype vertexNum)
{
	qsizetype result;
	vertexNum = std::min<qsizetype>(vertexNum, 2);

	QJsonArray triangles = CMeshInitializer::GetTrianglesRaw(jsonDataObject);
	result = triangles.at(triangleIndex * 3 + vertexNum).toInt();

	return result;
}
