#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>
#include <QMutex>
#include <QSet>

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

struct SEdge
{
	SEdge(QVector3D Vertex1, QVector3D Vertex2)
	{
		Vertices[0] = Vertex1;
		Vertices[1] = Vertex2;
	}

	QVector3D Vertices[2];
	double GetLength() const
	{
		return (Vertices[0] - Vertices[1]).length();
	}
};

struct STriangle
{
	QVector3D Vertices[3];

	SEdge GetSideA() const
	{
		return SEdge(Vertices[2], Vertices[1]);
	}

	SEdge GetSideB() const
	{
		return SEdge(Vertices[0], Vertices[2]);
	}

	SEdge GetSideC() const
	{
		return SEdge(Vertices[1], Vertices[0]);
	}

	double GetPerimeter() const
	{
		return this->GetSideA().GetLength() + this->GetSideB().GetLength() + this->GetSideC().GetLength();
	}

	// Heron's formula - https://en.wikipedia.org/wiki/Heron%27s_formula
	double GetArea() const
	{
		double semiP = this->GetPerimeter() / 2.0f;
		return	sqrt(semiP
					*(semiP-this->GetSideA().GetLength())
					*(semiP-this->GetSideB().GetLength())
					*(semiP-this->GetSideC().GetLength()));
	}

	bool IsDegenerate() const
	{
		return qFuzzyIsNull(this->GetArea());
	}
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
	QVector3D	GetVertex(qsizetype vertexIndex) const;

	qsizetype	GetTrianglesCount() const;
	STriangle	GetTriangle(qsizetype triangleIndex) const;

	SMeshStats	GetStats() const;

signals:
	void		OnRecalculated();

private:
	void		Recalculate();

	QJsonObject GetInnerObject() const;

	QJsonArray	GetVerticesRaw() const;
	QJsonArray	GetTrianglesRaw() const;

public slots:
	void		MeshAnalyzerFinished();

private:
	bool		bRecalculating = false;
    QJsonObject Object;
	SMeshStats	MeshStats;

	QMutex								Mutex;
	QHash<qsizetype, QSet<qsizetype>>	AdjacencyMap;
	QSet<class QObject*>				AnalyzeWorkers;
};

#endif // GEOMETRYOBJECT_H
