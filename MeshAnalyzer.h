#ifndef MESHANALYZER_H
#define MESHANALYZER_H

#include <QObject>
#include <QRunnable>

#include "GeometryObject.h"

class CMeshAnalyzer : public QObject, public QRunnable
{
    Q_OBJECT
public:
	explicit CMeshAnalyzer(CGeometryObject& inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent = nullptr);
	~CMeshAnalyzer();

signals:
	void Finished();

	// QRunnable interface
public:
	void run() Q_DECL_OVERRIDE;

private:
	void Work();

private:
	CGeometryObject&		GeometryObject;
	qsizetype				BeginIndex	= 0;
	qsizetype				EndIndex	= 0;

};

#endif // MESHANALYZER_H
