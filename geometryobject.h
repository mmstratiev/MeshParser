#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>
#include <QMutex>
#include <QSet>

#include "DCEL.h"

class QByteArray;

struct SMeshStats
{
	size_t	TrianglesCount	= 0;

	double	MinTriangleArea	= std::numeric_limits<double>().max();
	double	MaxTriangleArea	= std::numeric_limits<double>().min();
	double	Area			= 0.0f;

	bool	bIsClosed		= false;

	double GetAvgTriangleArea() const
	{
		return Area / TrianglesCount;
	}
};

struct SVertex
{
	QVector3D Location;
	QVector3D Normal;
};

using TPromise = std::function<void()>;
class CGeometryObject : public QObject
{
	Q_OBJECT
	friend class CMeshAnalyzer;

public:
	CGeometryObject();

	// Async methods
	void Init(const QByteArray& jsonByteArr, TPromise callback);
	void Wait(TPromise callback);

	qsizetype	GetVerticesCount() const;
	SVertex		GetVertex(qsizetype vertexIndex) const;

	qsizetype	GetTrianglesCount() const;
	STriangle	GetTriangle(qsizetype triangleIndex) const;
	qsizetype	GetTriangleVertexIndex(qsizetype triangleIndex, qsizetype vertexNum) const;

	SMeshStats	GetStats() const;

signals:
	void		OnRecalculated();

private:
	QJsonObject GetRawData() const;

	QJsonArray	GetVerticesRaw() const;
	QJsonArray	GetTrianglesRaw() const;

	qsizetype	GetVerticesCountRaw() const;
	QVector3D	GetVertexRaw(qsizetype vertexIndex) const;

	qsizetype	GetTrianglesCountRaw() const;
	STriangle	GetTriangleRaw(qsizetype triangleIndex) const;
	qsizetype	GetTriangleVertexIndexRaw(qsizetype triangleIndex, qsizetype vertexNum) const;

	void		Recalculate();

public slots:
	void		MeshAnalyzerFinished();

private:
	bool		bRecalculating = false;
	QJsonObject	RawData;
	SMeshStats	MeshStats;
	CDCEL		EdgeList;

	QMutex					Mutex;
	QSet<class QObject*>	AnalyzeWorkers;
};

#endif // GEOMETRYOBJECT_H
