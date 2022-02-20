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
	size_t TrianglesCount	= 0;

	double MinTriangleArea	= std::numeric_limits<double>().max();
	double MaxTriangleArea	= std::numeric_limits<double>().min();
	double SumTriangleArea	= 0.0f;

	double GetAvgTriangleArea() const
	{
		return SumTriangleArea / TrianglesCount;
	}
};

struct STriangle
{
	QVector3D Vertices[3];

	double GetSideA() const
	{
		return (Vertices[1] - Vertices[2]).length();
	}

	double GetSideB() const
	{
		return (Vertices[0] - Vertices[2]).length();
	}

	double GetSideC() const
	{
		return (Vertices[0] - Vertices[1]).length();
	}

	double GetPerimeter() const
	{
		return this->GetSideA() + this->GetSideB() + this->GetSideC();
	}

	double GetArea() const
	{
		double semiP = this->GetPerimeter() / 2.0f;
		return sqrt(semiP*(semiP-this->GetSideA())*(semiP-this->GetSideB())*(semiP-this->GetSideC()));
	}
};

using TGetStatsCallback = std::function<void(const SMeshStats &)>;

class CGeometryObject : public QObject
{
public:
	CGeometryObject();
	CGeometryObject(const QByteArray& jsonByteArr);

	void Init(const QByteArray& jsonByteArr);

	QJsonArray	GetVertices() const;
	QJsonArray	GetTriangles() const;

	qsizetype	GetVerticesCount() const;
	QVector3D	GetVertex(qsizetype vertexIndex) const	;

	qsizetype	GetTrianglesCount() const;
	STriangle	GetTriangle(qsizetype triangleIndex) const;

	// Multithreaded method
	void		GetStats(TGetStatsCallback funcCallback);

private:
    QJsonObject GetInnerObject() const;
	void		StartCalculations();

public slots:
	void		CalcStatsStart();
	void		CalcStatsStarted();
	void		CalcStatsFinished(TGetStatsCallback funcCallback);

private:
    QJsonObject Object;
	SMeshStats	MeshStats;

	QMutex		Mutex;

	QSet<class QObject*> Workers;

};

#endif // GEOMETRYOBJECT_H
