#include "MeshStatsWorker.h"

#include <QThread>
#include <QMutexLocker>
#include <QDebug>

CMeshAnalyzer::CMeshAnalyzer(const CGeometryObject& geometryObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(geometryObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{
	qInfo() << "Created" << this << QThread::currentThread();
}

CMeshAnalyzer::~CMeshAnalyzer()
{
	qInfo() << "Destroyed" << this << QThread::currentThread();

}

void CMeshAnalyzer::SetMutex(QMutex *mutex)
{
	Mutex = mutex;
}

void CMeshAnalyzer::SetOutput(SMeshStats *output)
{
	Output = output;
}

void CMeshAnalyzer::SetCallback(TGetStatsCallback funcCallback)
{
	Callback = funcCallback;
}

void CMeshAnalyzer::Work()
{
	if(!Output) return;

	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		QMutexLocker locker(Mutex);

		STriangle triangle = GeometryObject.GetTriangle(triangleIndex);
		double triangleArea = triangle.GetArea();

		if(triangle.IsDegenerate())
		{
			// TODO: Warn the user of degeneracy and ignore in calculations
			qInfo() << "Triangle " << triangleIndex << ": " << triangle.Vertices[0] << ", " << triangle.Vertices[1] << ", " << triangle.Vertices[2];
			qInfo() << "Area " + QString::number(triangleIndex) + ": " + QString::number(triangleArea);
		}

		Output->MaxTriangleArea = std::max<double>(Output->MaxTriangleArea, triangleArea);
		Output->MinTriangleArea = std::min<double>(Output->MinTriangleArea, triangleArea);
		Output->Area += triangleArea;

	//	qInfo() << "Work" << this << QThread::currentThread();
	}
}

void CMeshAnalyzer::run()
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
