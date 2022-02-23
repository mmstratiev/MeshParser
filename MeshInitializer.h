#ifndef MESHINITIALIZER_H
#define MESHINITIALIZER_H

#include <QObject>
#include <QRunnable>

#include "GeometryObject.h"

class CMeshInitializer : public QObject, public QRunnable
{
    Q_OBJECT
public:
	explicit CMeshInitializer(CGeometryObject& inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent = nullptr);
	~CMeshInitializer();

signals:
	void Finished();

	// QRunnable interface
public:
	void run() Q_DECL_OVERRIDE;

private:
	void Work();
	void FillDCEL(qsizetype triangleIndex);
	void FillMeshStats(qsizetype triangleIndex);

private:
	CGeometryObject&		GeometryObject;
	qsizetype				BeginIndex	= 0;
	qsizetype				EndIndex	= 0;

};

#endif // MESHINITIALIZER_H
