#ifndef THREADEDOBJ_H
#define THREADEDOBJ_H

#include <QObject>

class CThreadedObj : public QObject
{
    Q_OBJECT
public:
	explicit CThreadedObj(QObject *parent = nullptr);

public slots:
	void start();
	void started();
	void finished();
};

#endif // THREADEDOBJ_H
