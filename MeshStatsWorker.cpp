#include "MeshStatsWorker.h"

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
