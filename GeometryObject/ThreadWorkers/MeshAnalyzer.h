#ifndef MESHANALYZER_H
#define MESHANALYZER_H

#include "ThreadWorker.h"

class CMeshAnalyzer : public CThreadWorker
{
    Q_OBJECT
public:
	explicit CMeshAnalyzer(CGeometryObject& inOutObject, QObject *parent = nullptr);

protected:
	void Work() override;

};

#endif // MESHANALYZER_H
