#include "MeshStatsWorker.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>

MeshStatsWorker::MeshStatsWorker(const CGeometryObject& geometryObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(geometryObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{
	qInfo() << "Created" << this << QThread::currentThread();
}

MeshStatsWorker::~MeshStatsWorker()
{
	qInfo() << "Destroyed" << this << QThread::currentThread();

}

void MeshStatsWorker::SetMutex(QMutex *mutex)
{
	Mutex = mutex;
}

void MeshStatsWorker::SetOutput(SMeshStats *output)
{
	Output = output;
}

void MeshStatsWorker::SetCallback(TGetStatsCallback funcCallback)
{
	Callback = funcCallback;
}

void MeshStatsWorker::Work()
{
	if(!Output) return;

	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		QMutexLocker locker(Mutex);

		STriangle triangle = GeometryObject.GetTriangle(triangleIndex);
		double triangleArea = triangle.GetArea();

		if(qFuzzyCompare(triangleArea, 0))
		{
			// TODO: Warn the user of degeneracy and ignore in calculations
			qInfo() << "Triangle " << triangleIndex << ": " << triangle.Vertices[0] << ", " << triangle.Vertices[1] << ", " << triangle.Vertices[2];
			qInfo() << "Area " + QString::number(triangleIndex) + ": " + QString::number(triangleArea);
		}
		Output->MaxTriangleArea = std::max<double>(Output->MaxTriangleArea, triangleArea);
		Output->MinTriangleArea = std::min<double>(Output->MinTriangleArea, triangleArea);
		Output->SumTriangleArea += triangleArea;

	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

void MeshStatsWorker::run()
{
	// Starting the thread
	qInfo() << "Starting" << this << QThread::currentThread();

	this->Work();

	qInfo() << "Finishing" << this << QThread::currentThread();
	emit Finished(Callback);

	// todo: remove auto deletion and this sleep
	QThread::currentThread()->msleep(100);
	// Finishing the thread
}
