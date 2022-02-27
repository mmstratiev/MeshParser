#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>
#include <QMutex>
#include <QSet>

#include "AccelerationStructs/BVH.h"
#include "DCEL/DCEL.h"
#include "OpenGL/Vertex.h"
#include "Datatypes.h"
#include "AccelerationStructs/BoundingBox.h"

class QByteArray;

struct SVertex
{
	QVector3D Location;
	QVector3D Normal;
};

using TOnIdleCallback = std::function<void()>;

class CGeometryObject : public QObject
{
	Q_OBJECT
	friend class CMeshReader;
	friend class CMeshAnalyzer;
	friend class CMeshSubdivider;

public:
	enum class EState
	{
		Initializing,
		Analyzing,
		Idle,
	};

	CGeometryObject();

	// Async methods
	void		Init(const QByteArray& jsonByteArr);
	void		Init(CDCEL& edgeList);

	bool		IsInitialized() const;
	void		WaitForIdle(TOnIdleCallback callback) const;

	qsizetype	GetVerticesCount() const;
	bool		GetVertex(qsizetype vertexID, SVertex& outVertex) const;

	qsizetype	GetTrianglesCount() const;
	bool		GetTriangle(qsizetype triangleID, STriangle& outTriangle) const;
	qsizetype	GetTriangleVertID(qsizetype triangleID, qsizetype vertIndex) const;

	const CBoundingBox& GetBoundingBox() const;

	double		GetMinTriangleArea() const;
	double		GetMaxTriangleArea() const;
	double		GetTotalArea() const;

	bool		IsClosed() const;

	void		Subdivide(ESubdivisionAlgorithm algo);

	// OpenGL methods
	void			BuildOpenGLVertexes();
	qsizetype		GetOpenGLVerticesCount() const;
	CVertex*		GetOpenGLVerticesBegin();

signals:
	void		StateChanged(CGeometryObject::EState newState);
	void		Idled();

private:
	void		SetState(EState newState);

	void		Initialize(const QByteArray &rawData);
	void		Analyze();

	void		ClearInitializedData();
	void		ClearAnalyzedData();

private slots:
	void		MeshInitializerFinished();
	void		MeshAnalyzerFinished();

private:
	EState		State = EState::Idle;
	CDCEL		EdgeList;
	CBVH		BoundingBoxHierarchy;

	std::vector<CVertex> OpenGLVertices;

	CBoundingBox	BoundingBox;
	double			MinTriangleArea	= std::numeric_limits<double>().max();
	double			MaxTriangleArea	= std::numeric_limits<double>().min();
	double			TotalArea		= 0.0f;
	bool			bIsClosed		= true;

	QMutex					Mutex;
	QSet<class QObject*>	Workers;
};

#endif // GEOMETRYOBJECT_H
