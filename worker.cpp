#include "worker.h"

#include <QThread>
#include <QDebug>

Worker::Worker(QObject *parent)
	: ThreadWorker{parent}
{
	qInfo() << "Created" << this << QThread::currentThread();
}

Worker::~Worker()
{
	qInfo() << "Destroyed" << this << QThread::currentThread();

}

void Worker::work()
{
	for(int i = 0; i < 5; i++)
	{
		qInfo() << "Work" << this << QThread::currentThread();
		QThread::currentThread()->msleep(1000);
	}
}

void Worker::run()
{
	// Starting in the thread
	qInfo() << "Starting" << this << QThread::currentThread();
	emit started();

	this->work();

	qInfo() << "Finishing" << this << QThread::currentThread();
	emit finished();

	// todo: remove auto deletion and this sleep
	QThread::currentThread()->msleep(100);
	// Finishing in the thread
}
