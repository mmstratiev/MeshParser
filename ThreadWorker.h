#ifndef THREADWORKER_H
#define THREADWORKER_H

#include <QObject>
#include <QRunnable>

class ThreadWorker : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit ThreadWorker(QObject *parent = nullptr);

signals:
	void started();
	void finished();

signals:

};

#endif // THREADWORKER_H
