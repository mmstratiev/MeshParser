#ifndef CTHREADWORKER_H
#define CTHREADWORKER_H

#include <QObject>
#include <QRunnable>

#include "GeometryObject/GeometryObject.h"

class CThreadWorker : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit CThreadWorker(CGeometryObject& inOutObject, QObject *parent = nullptr);
	explicit CThreadWorker(CGeometryObject& inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent = nullptr);

	void SetBeginIndex(qsizetype newIndex);
	void SetEndIndex(qsizetype newIndex);

signals:
	void Finished();
	void MadeProgress();

	// QRunnable interface
public:
	void run() override;

protected:
	virtual void Work() = 0;

protected:
	CGeometryObject&		GeometryObject;
	qsizetype				BeginIndex	= 0;
	qsizetype				EndIndex	= 0;
};

#endif // CTHREADWORKER_H
