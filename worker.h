#ifndef WORKER_H
#define WORKER_H

#include "ThreadWorker.h"

class Worker : public ThreadWorker
{
    Q_OBJECT
public:
    explicit Worker(QObject *parent = nullptr);
	~Worker();

	// QRunnable interface
public:
	void run() Q_DECL_OVERRIDE;

private:
	void work();
};

#endif // WORKER_H
