#include "ThreadWorker.h"

CThreadWorker::CThreadWorker(CGeometryObject &inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{}

CThreadWorker::CThreadWorker(CGeometryObject &inOutObject, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
{}

void CThreadWorker::SetBeginIndex(qsizetype newIndex)
{
	BeginIndex = newIndex;
}

void CThreadWorker::SetEndIndex(qsizetype newIndex)
{
	EndIndex = newIndex;
}

void CThreadWorker::run()
{
	this->Work();
	emit Finished();
}
