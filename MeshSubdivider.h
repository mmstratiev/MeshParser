#ifndef CMESHSUBDIVIDER_H
#define CMESHSUBDIVIDER_H

#include <QObject>
#include <QRunnable>

#include "GeometryObject.h"

class CMeshSubdivider : public QObject, public QRunnable
{
	Q_OBJECT
public:
	explicit CMeshSubdivider(CGeometryObject& inOutObject, ESubdivisionAlgorithm algo, qsizetype beginIndex, qsizetype endIndex, QObject *parent = nullptr);

signals:
	void Finished();

	// QRunnable interface
public:
	void run() Q_DECL_OVERRIDE;

	CDCEL GetResult();

private:
	void Work();

	QVector3D GetEvenVertPosition(TDCEL_VertPtr originalVert) const;
	QVector3D GetOddVertPosition(TDCEL_EdgePtr originalEdge) const;

private:
	CGeometryObject&		GeometryObject;
	ESubdivisionAlgorithm	Algo;
	qsizetype				BeginIndex	= 0;
	qsizetype				EndIndex	= 0;
	CDCEL					Destination;

};

#endif // CMESHSUBDIVIDER_H
