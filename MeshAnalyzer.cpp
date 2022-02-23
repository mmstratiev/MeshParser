#include "MeshAnalyzer.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>

CMeshAnalyzer::CMeshAnalyzer(CGeometryObject& inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{
	qInfo() << "Created" << this << QThread::currentThread();
}

CMeshAnalyzer::~CMeshAnalyzer()
{
	qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshAnalyzer::Work()
{
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		QMutexLocker locker(&GeometryObject.Mutex);
		this->FillDCEL(triangleIndex);

		STriangle triangle = GeometryObject.GetTriangle(triangleIndex);
		double triangleArea = triangle.GetArea();

		if(triangle.IsDegenerate())
		{
			// TODO: Warn the user of degeneracy and ignore in calculations
			qInfo() << "Triangle " << triangleIndex << ": " << triangle.Vertices[0] << ", " << triangle.Vertices[1] << ", " << triangle.Vertices[2];
			qInfo() << "Area " + QString::number(triangleIndex) + ": " + QString::number(triangleArea);
		}

		GeometryObject.MeshStats.MaxTriangleArea = std::max<double>(GeometryObject.MeshStats.MaxTriangleArea, triangleArea);
		GeometryObject.MeshStats.MinTriangleArea = std::min<double>(GeometryObject.MeshStats.MinTriangleArea, triangleArea);
		GeometryObject.MeshStats.Area			+= triangleArea;

	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

void CMeshAnalyzer::FillDCEL(qsizetype triangleIndex)
{
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

	edge1Ptr->Next = edge2Ptr;
	edge1Ptr->Prev = edge3Ptr;
	edge1Ptr->Origin = vert1Ptr;
	edge1Ptr->Face = facePtr;

	edge2Ptr->Next = edge3Ptr;
	edge2Ptr->Prev = edge1Ptr;
	edge2Ptr->Origin = vert2Ptr;
	edge2Ptr->Face = facePtr;

	edge3Ptr->Next = edge1Ptr;
	edge3Ptr->Prev = edge2Ptr;
	edge3Ptr->Origin = vert3Ptr;
	edge3Ptr->Face = facePtr;
}

void CMeshAnalyzer::run()
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
