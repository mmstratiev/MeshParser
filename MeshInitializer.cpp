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
	qInfo() << "Created" << this << QThread::currentThread();
}

CMeshInitializer::~CMeshInitializer()
{
	qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshInitializer::Work()
{
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		QMutexLocker locker(&GeometryObject.Mutex);
		this->FillDCEL(triangleIndex);
		this->FillMeshStats(triangleIndex);
	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

void CMeshInitializer::FillDCEL(qsizetype triangleIndex)
{
	// Clean up degenerate triangles. They add no value to the object.
	if(GeometryObject.GetTriangleRaw(triangleIndex).IsDegenerate()) return;

	TDCEL_VertID vert1ID(GeometryObject.GetTriangleVertexIndexRaw(triangleIndex, 0));
	TDCEL_VertID vert2ID(GeometryObject.GetTriangleVertexIndexRaw(triangleIndex, 1));
	TDCEL_VertID vert3ID(GeometryObject.GetTriangleVertexIndexRaw(triangleIndex, 2));

	TDCEL_EdgeID edge1ID(vert1ID, vert2ID);
	TDCEL_EdgeID edge2ID(vert2ID, vert3ID);
	TDCEL_EdgeID edge3ID(vert3ID, vert1ID);

	TDCEL_FaceID faceID = triangleIndex;

	TDCEL_EdgePtr edge1Ptr = GeometryObject.EdgeList.AddEdge(edge1ID);
	TDCEL_EdgePtr edge2Ptr = GeometryObject.EdgeList.AddEdge(edge2ID);
	TDCEL_EdgePtr edge3Ptr = GeometryObject.EdgeList.AddEdge(edge3ID);

	TDCEL_VertPtr vert1Ptr = GeometryObject.EdgeList.AddVertex(vert1ID, GeometryObject.GetVertexRaw(vert1ID), edge1Ptr);
	TDCEL_VertPtr vert2Ptr = GeometryObject.EdgeList.AddVertex(vert2ID, GeometryObject.GetVertexRaw(vert2ID), edge2Ptr);
	TDCEL_VertPtr vert3Ptr = GeometryObject.EdgeList.AddVertex(vert3ID, GeometryObject.GetVertexRaw(vert3ID), edge3Ptr);

	TDCEL_FacePtr facePtr = GeometryObject.EdgeList.AddFace(faceID, edge1Ptr);

	edge1Ptr->SetNext(edge2Ptr);
	edge1Ptr->SetPrev(edge3Ptr);
	edge1Ptr->SetOrigin(vert1Ptr);
	edge1Ptr->SetFace(facePtr);

	edge2Ptr->SetNext(edge3Ptr);
	edge2Ptr->SetPrev(edge1Ptr);
	edge2Ptr->SetOrigin(vert2Ptr);
	edge2Ptr->SetFace(facePtr);

	edge3Ptr->SetNext(edge1Ptr);
	edge3Ptr->SetPrev(edge2Ptr);
	edge3Ptr->SetOrigin(vert3Ptr);
	edge3Ptr->SetFace(facePtr);
}

void CMeshInitializer::FillMeshStats(qsizetype triangleIndex)
{
	STriangle triangle = GeometryObject.GetTriangle(triangleIndex);
	if(!triangle.IsDegenerate())
	{
		double triangleArea = triangle.GetArea();

		GeometryObject.MeshStats.MaxTriangleArea = std::max<double>(GeometryObject.MeshStats.MaxTriangleArea, triangleArea);
		GeometryObject.MeshStats.MinTriangleArea = std::min<double>(GeometryObject.MeshStats.MinTriangleArea, triangleArea);
		GeometryObject.MeshStats.TotalArea			+= triangleArea;
	}
}

void CMeshInitializer::run()
{
	// Starting the thread
	qInfo() << "Starting" << this << QThread::currentThread();

	this->Work();

	qInfo() << "Finishing" << this << QThread::currentThread();
	emit Finished();

	// todo: remove auto deletion and this sleep
	QThread::currentThread()->msleep(100);
	// Finishing the thread
}
