#ifndef MESHSTATSWORKER_H
#define MESHSTATSWORKER_H

#include <QObject>
#include <QRunnable>

#include "GeometryObject.h"

class MeshStatsWorker : public QObject, public QRunnable
{
    Q_OBJECT
public:
	explicit MeshStatsWorker(const CGeometryObject& geometryObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent = nullptr);
	~MeshStatsWorker();

	void SetMutex(QMutex* mutex);
	void SetOutput(SMeshStats* output);
	void SetCallback(TGetStatsCallback funcCallback);

signals:
	void Finished(TGetStatsCallback funcCallback);

	// QRunnable interface
public:
	void run() Q_DECL_OVERRIDE;

private:
	void Work();

private:
	QMutex*		Mutex;
	SMeshStats* Output;
	std::function<void(const SMeshStats&)> Callback;

	const CGeometryObject&	GeometryObject;
	qsizetype				BeginIndex	= 0;
	qsizetype				EndIndex	= 0;


};

#endif // MESHSTATSWORKER_H
