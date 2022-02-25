#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>
#include <QMutex>
#include <QSet>

#include "DCEL/DCEL.h"

class QByteArray;

struct SVertex
{
	QVector3D Location;
	QVector3D Normal;
};

using TPromise = std::function<void()>;
class CGeometryObject : public QObject
{
	Q_OBJECT
	friend class CMeshInitializer;
	friend class CMeshAnalyzer;
	friend class CMeshSubdivider;

public:
	CGeometryObject();

	// Async methods
	void Init(const QByteArray& jsonByteArr, TPromise callback);
	void Wait(TPromise callback) const;

	qsizetype	GetVerticesCount() const;
	SVertex		GetVertex(qsizetype vertexIndex) const;

	qsizetype	GetTrianglesCount() const;
	STriangle	GetTriangle(qsizetype triangleIndex) const;
	qsizetype	GetTriangleVertexIndex(qsizetype triangleIndex, qsizetype vertexNum) const;

	double		GetMinTriangleArea() const;
	double		GetMaxTriangleArea() const;
	double		GetTotalArea() const;

	bool		IsClosed() const;

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
	void		StartInitializers();
	void		StartAnalyzers();

public slots:
	void		MeshInitializerFinished();
	void		MeshAnalyzerFinished();

private:
	bool		bRecalculating = false;
	QJsonObject	RawData;
	CDCEL		EdgeList;

	double		MinTriangleArea	= std::numeric_limits<double>().max();
	double		MaxTriangleArea	= std::numeric_limits<double>().min();
	double		TotalArea		= 0.0f;
	bool		bIsClosed		= true;

	QMutex					Mutex;
	QSet<class QObject*>	Workers;
};

#endif // GEOMETRYOBJECT_H
