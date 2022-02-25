#include "MeshInitializer.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>

CMeshInitializer::CMeshInitializer(CGeometryObject& inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{
	//qInfo() << "Created" << this << QThread::currentThread();
}

CMeshInitializer::~CMeshInitializer()
{
	//qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshInitializer::Work()
{
	//qInfo() << BeginIndex << EndIndex  << this;
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{		
		this->FillDCEL(triangleIndex);
		this->FillMeshStats(triangleIndex);
	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

void CMeshInitializer::FillDCEL(qsizetype triangleIndex)
{
	// Skip degenerate triangles. They add no value to the object.
	if(GeometryObject.GetTriangleRaw(triangleIndex).IsDegenerate()) return;

	TDCEL_VertID vert1ID(GeometryObject.GetTriangleVertexIndexRaw(triangleIndex, 0));
	TDCEL_VertID vert2ID(GeometryObject.GetTriangleVertexIndexRaw(triangleIndex, 1));
	TDCEL_VertID vert3ID(GeometryObject.GetTriangleVertexIndexRaw(triangleIndex, 2));

	QMutexLocker locker(&GeometryObject.Mutex);
	GeometryObject.EdgeList.AddVertex(vert1ID, GeometryObject.GetVertexRaw(vert1ID));
	GeometryObject.EdgeList.AddVertex(vert2ID, GeometryObject.GetVertexRaw(vert2ID));
	GeometryObject.EdgeList.AddVertex(vert3ID, GeometryObject.GetVertexRaw(vert3ID));

	GeometryObject.EdgeList.Connect(vert1ID, vert2ID, vert3ID, triangleIndex);
}

void CMeshInitializer::FillMeshStats(qsizetype triangleIndex)
{
	QMutexLocker locker(&GeometryObject.Mutex);
	STriangle triangle = GeometryObject.GetTriangle(triangleIndex);

	if(!triangle.IsDegenerate())
	{
		double triangleArea = triangle.GetArea();

		GeometryObject.MaxTriangleArea	= std::max<double>(GeometryObject.MaxTriangleArea, triangleArea);
		GeometryObject.MinTriangleArea	= std::min<double>(GeometryObject.MinTriangleArea, triangleArea);
		GeometryObject.TotalArea		= GeometryObject.TotalArea + triangleArea;
	}
}

void CMeshInitializer::run()
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
