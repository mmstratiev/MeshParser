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
	//qInfo() << "Created" << this << QThread::currentThread();
}

CMeshAnalyzer::~CMeshAnalyzer()
{
	//qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshAnalyzer::Work()
{
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		if(!GeometryObject.IsClosed())
		{
			break;
		}

		// Find out if mesh is closed or open
		TDCEL_FacePtr faceToCheck = GeometryObject.EdgeList.GetFace(triangleIndex);
		if(faceToCheck)
		{
			QMutexLocker locker(&GeometryObject.Mutex);

			TDCEL_EdgePtr currentEdge = faceToCheck->Edge();
			do
			{
				GeometryObject.bIsClosed = currentEdge->Twin();
				currentEdge = currentEdge->Next();
			} while(currentEdge != faceToCheck->Edge() && GeometryObject.bIsClosed);
		}
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
