#include "ThreadedObj.h"
#include "worker.h"

#include <QThreadPool>
#include <QThread>
#include <QDebug>

CThreadedObj::CThreadedObj(QObject *parent)
    : QObject{parent}
{
	qInfo() << "Created" << this << QThread::currentThread();
}

void CThreadedObj::start()
{
	// We want to use all available threads for calculations
	int maxThreadCount = QThreadPool::globalInstance()->maxThreadCount();
	for(int i = 0; i < maxThreadCount; i++)
	{
		ThreadWorker *worker = new Worker();
		worker->setObjectName("CalcWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		connect(worker, &Worker::started, this, &CThreadedObj::started, Qt::QueuedConnection);
		connect(worker, &Worker::finished, this, &CThreadedObj::finished, Qt::QueuedConnection);

		QThreadPool::globalInstance()->start(worker);
	}
}

void CThreadedObj::started()
{
	Worker* worker = qobject_cast<Worker*>(sender());
	if(!worker) return;

	qInfo() << "Started" << worker << QThread::currentThread();
}

void CThreadedObj::finished()
{
	Worker* worker = qobject_cast<Worker*>(sender());
	if(!worker) return;

	qInfo() << "Finished" << worker << QThread::currentThread();
}
