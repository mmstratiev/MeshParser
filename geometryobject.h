#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>
#include <QJsonArray>
#include <QVector3D>
#include <QMutex>
#include <QSet>

#include "AccelerationStructs/BVH.h"
#include "DCEL/DCEL.h"
#include "OpenGL/OpenGLVertex.h"
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
	friend class CMeshInitializer;
	friend class CMeshAnalyzer;
	friend class CMeshSubdivider;

public:
	enum class EState
	{
		Initializing,
		Analyzing,
		Subdividing,
		Idle,
	};

	CGeometryObject();

	// Async methods
	void		Init(const QByteArray& jsonByteArr);

	bool		IsInitialized() const;
	void		WhenIdle(TOnIdleCallback callback) const;

	EState		GetState() const;

	qsizetype	GetVerticesCount() const;
	bool		GetVertex(qsizetype vertexID, SVertex& outVertex) const;

	qsizetype	GetTrianglesCount() const;
	bool		GetTriangle(qsizetype triangleID, CTriangle& outTriangle) const;
	qsizetype	GetTriangleVertID(qsizetype triangleID, qsizetype vertIndex) const;

	const CBoundingBox& GetBoundingBox() const;

	double		GetMinTriangleArea() const;
	double		GetMaxTriangleArea() const;
	double		GetTotalArea() const;

	bool		IsClosed() const;

	void		Subdivide(ESubdivisionAlgorithm algo);
	bool		RayTrace(const QVector3D& origin, const QVector3D& dir, std::vector<CTriangle>& outHitTris);

	// OpenGL methods
	qsizetype		GetOpenGLVerticesCount() const;
	COpenGLVertex*	GetOpenGLVerticesBegin();

signals:
	void		StateChanged(CGeometryObject::EState newState);
	void		Idled();

	// UI
	void		MadeProgress(int currentProgress, int maxProgress);

private:
	void		SetState(EState newState);

	void		Initialize(const QByteArray &rawData);
	void		Analyze();

	void		ClearInitializedData();
	void		ClearAnalyzedData();

private slots:
	void		MeshInitializerFinished();
	void		MeshAnalyzerFinished();
	void		MeshSubdividerFinished();

	// UI
	void		ThreadMadeProgress();

private:
	EState		State = EState::Idle;
	CDCEL		EdgeList;
	CBVH		BoundingVolHierarchy;

	std::vector<COpenGLVertex> OpenGLVertices;

	CBoundingBox	BoundingBox;
	double			MinTriangleArea	= std::numeric_limits<double>().max();
	double			MaxTriangleArea	= std::numeric_limits<double>().min();
	double			TotalArea		= 0.0f;
	bool			bIsClosed		= true;

	QMutex					Mutex;
	QSet<class QObject*>	Workers;

	// Temp values which are used to share information between threads
	std::unordered_map<TDCEL_EdgeID, TDCEL_VertID, TDCEL_EdgeID> TempEdgeToNewVert;
	CDCEL				TempEdgeList;
	std::atomic<size_t>	TempLastOddVertID;

	std::atomic<int> TempMaxProgress	= 0;
	std::atomic<int> TempProgress		= 0;
};

#endif // GEOMETRYOBJECT_H
