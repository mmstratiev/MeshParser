#include "MeshAnalyzer.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>

#include "DCEL/DCEL_Edge.h"
#include "DCEL/DCEL_Face.h"

CMeshAnalyzer::CMeshAnalyzer(CGeometryObject& inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{
	// qInfo() << "Created" << this << QThread::currentThread();
}

CMeshAnalyzer::~CMeshAnalyzer()
{
	//qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshAnalyzer::Work()
{
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		QMutexLocker locker(&GeometryObject.Mutex);
		TDCEL_FacePtr faceToCheck = GeometryObject.EdgeList.GetFace(triangleIndex);

		if(!faceToCheck)
		{
			GeometryObject.MaxProgress--;
		}
		else
		{
			CTriangle triangle = faceToCheck->Get();
			double triangleArea = triangle.GetArea();

			GeometryObject.BoundingVolHierarchy.AddLeaf(faceToCheck);

			GeometryObject.MaxTriangleArea	= std::max<double>(GeometryObject.MaxTriangleArea, triangleArea);
			GeometryObject.MinTriangleArea	= std::min<double>(GeometryObject.MinTriangleArea, triangleArea);
			GeometryObject.TotalArea		= GeometryObject.TotalArea + triangleArea;

			// Find out if mesh is closed or open
			if(GeometryObject.IsClosed())
			{
				TDCEL_EdgePtr currentEdge = faceToCheck->Edge();
				do
				{
					GeometryObject.bIsClosed = currentEdge->Twin();
					currentEdge = currentEdge->Next();
				} while(currentEdge != faceToCheck->Edge() && GeometryObject.bIsClosed);
			}
			GeometryObject.Progress++;
		}
		emit MadeProgress();
	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

void CMeshAnalyzer::run()
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
