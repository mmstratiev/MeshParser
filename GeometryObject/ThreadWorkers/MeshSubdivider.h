#ifndef CMESHSUBDIVIDER_H
#define CMESHSUBDIVIDER_H

#include "ThreadWorker.h"

class CMeshSubdivider : public CThreadWorker
{
	Q_OBJECT
public:
	explicit CMeshSubdivider(CGeometryObject& inOutObject, ESubdivisionAlgorithm algo, QObject *parent = nullptr);

protected:
	void Work() override;

private:
	QVector3D GetEvenVertPosition(TDCEL_VertPtr originalVert) const;
	QVector3D GetOddVertPosition(TDCEL_EdgePtr originalEdge) const;

private:
	ESubdivisionAlgorithm	Algo;
};

#endif // CMESHSUBDIVIDER_H
