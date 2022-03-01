#include <QJsonDocument>
#include <QJsonArray>
#include <QThreadPool>
#include <QThread>
#include <QMutexLocker>
#include <QQuaternion>

#include "GeometryObject.h"
#include "ThreadWorkers/MeshInitializer.h"
#include "ThreadWorkers/MeshAnalyzer.h"
#include "ThreadWorkers/MeshSubdivider.h"

CGeometryObject::CGeometryObject()
{
}

void CGeometryObject::Init(const QByteArray &jsonByteArr)
{
	this->Initialize(jsonByteArr);
}

bool CGeometryObject::IsInitialized() const
{
	return EdgeList.GetVerticesCount() > 0 && State == EState::Idle;
}

void CGeometryObject::WhenIdle(TOnIdleCallback callback) const
{
	if(State != EState::Idle)
	{
		connect(this, &CGeometryObject::Idled, callback);
	}
	else
	{
		callback();
	}
}

CGeometryObject::EState CGeometryObject::GetState() const
{
	return State;
}

qsizetype CGeometryObject::GetVerticesCount() const
{
	return EdgeList.GetVerticesCount();
}

bool CGeometryObject::GetVertex(qsizetype vertexID, SVertex& outVertex) const
{
	bool result = false;
	TDCEL_VertPtr vertex = EdgeList.GetVertex(vertexID);
	if(vertex)
	{
		outVertex.Location	= vertex->Get();
		outVertex.Normal	= vertex->GetNormal();
		result = true;
	}

	return result;
}

qsizetype CGeometryObject::GetTrianglesCount() const
{
	return EdgeList.GetFacesCount();
}

bool CGeometryObject::GetTriangle(qsizetype triangleID, CTriangle& outTriangle) const
{
	bool result = false;
	TDCEL_FacePtr face = EdgeList.GetFace(triangleID);
	if(face)
	{
		outTriangle = face->Get();
		result = true;
	}
	return result;
}

qsizetype CGeometryObject::GetTriangleVertID(qsizetype triangleID, qsizetype vertIndex) const
{
	qsizetype result = 0;
	TDCEL_FacePtr face = EdgeList.GetFace(triangleID);
	if(face)
	{
		CFaceVerticesIterator vertIt = face->GetFaceVerticesIterator();
		vertIt += vertIndex;
		result = vertIt.Value()->GetID();
	}
	return result;
}

const CBoundingBox &CGeometryObject::GetBoundingBox() const
{
	return BoundingBox;
}

double CGeometryObject::GetMinTriangleArea() const
{
	return MinTriangleArea;
}

double CGeometryObject::GetMaxTriangleArea() const
{
	return MaxTriangleArea;
}

double CGeometryObject::GetTotalArea() const
{
	return TotalArea;
}

bool CGeometryObject::IsClosed() const
{
	return bIsClosed;
}

void CGeometryObject::Subdivide(ESubdivisionAlgorithm algo)
{
	this->SetState(EState::Subdividing);
	TempLastOddVertID	= this->GetVerticesCount();

	this->StartThreads(this->GetTrianglesCount(), [&] (qsizetype workerIndex) -> CThreadWorker*
	{
		CMeshSubdivider* worker = new CMeshSubdivider(*this, algo);
		worker->setObjectName("SubdividerWorker_" + QString::number(workerIndex));

		connect(worker, &CThreadWorker::Finished, this, &CGeometryObject::MeshSubdividerFinished, Qt::QueuedConnection);
		connect(worker, &CThreadWorker::MadeProgress, this, &CGeometryObject::ThreadMadeProgress, Qt::QueuedConnection);

		return worker;
	});
}

bool CGeometryObject::RayTrace(const QVector3D &origin, const QVector3D &dir, std::vector<CTriangle>& outHitTris)
{
	return BoundingVolHierarchy.RayTrace(origin, dir, outHitTris);
}

qsizetype CGeometryObject::GetOpenGLVerticesCount() const
{
	return this->OpenGLVertices.size();
}

COpenGLVertex *CGeometryObject::GetOpenGLVerticesBegin()
{
	return &this->OpenGLVertices[0];
}

void CGeometryObject::SetState(EState newState)
{
	State = newState;
	emit StateChanged(newState);
	if(State == EState::Idle)
		emit Idled();
}

void CGeometryObject::Initialize(const QByteArray& rawData)
{
	this->SetState(EState::Initializing);
	this->ClearInitializedData();

	QJsonObject jsonDataObject	= QJsonDocument::fromJson(rawData).object();

	this->StartThreads(CMeshInitializer::GetTrianglesCountRaw(jsonDataObject), [&] (qsizetype workerIndex) -> CThreadWorker*
	{
		CMeshInitializer* worker = new CMeshInitializer(*this, jsonDataObject);
		worker->setObjectName("InitializerWorker_" + QString::number(workerIndex));

		connect(worker, &CThreadWorker::Finished, this, &CGeometryObject::MeshInitializerFinished, Qt::QueuedConnection);
		connect(worker, &CThreadWorker::MadeProgress, this, &CGeometryObject::ThreadMadeProgress, Qt::QueuedConnection);

		return worker;
	});
}

void CGeometryObject::Analyze()
{
	this->SetState(EState::Analyzing);
	this->ClearAnalyzedData();

	// higher depth will incerase analyze time but decrease ray trace time
	BoundingVolHierarchy.Init(BoundingBox, 4);

	this->StartThreads(this->GetTrianglesCount(), [&] (qsizetype workerIndex) -> CThreadWorker*
	{
		CMeshAnalyzer* worker = new CMeshAnalyzer(*this);
		worker->setObjectName("AnalyzerWorker_" + QString::number(workerIndex));

		connect(worker, &CThreadWorker::Finished, this, &CGeometryObject::MeshAnalyzerFinished, Qt::QueuedConnection);
		connect(worker, &CThreadWorker::MadeProgress, this, &CGeometryObject::ThreadMadeProgress, Qt::QueuedConnection);

		return worker;
	});
}

void CGeometryObject::StartThreads(qsizetype trianglesCount, std::function<CThreadWorker*(qsizetype workerIndex)> getThreadWorker)
{
	// We want to use ALL available threads
	WorkersCount					= QThreadPool::globalInstance()->maxThreadCount();

	qsizetype trianglesPerThread	= trianglesCount / WorkersCount;
	qsizetype remainder				= trianglesCount % WorkersCount;

	TempMaxProgress						= trianglesCount;
	TempProgress						= 0;

	qsizetype lastEndIndex			= 0;
	for(int workerIndex = 0; workerIndex < WorkersCount; workerIndex++)
	{
		// remainder can never be more than the divisor(num of threads), so this is a safe and efficent way to distribute work
		qsizetype trianglesForThread = trianglesPerThread;
		if(remainder > 0)
		{
			trianglesForThread++;
			remainder--;
		}

		qsizetype beginIndex	= lastEndIndex;
		qsizetype endIndex		= beginIndex + trianglesForThread;
		lastEndIndex			= endIndex;

		CThreadWorker *worker = getThreadWorker(workerIndex);
		worker->SetBeginIndex(beginIndex);
		worker->SetEndIndex(endIndex);
		worker->setAutoDelete(true);

		QThreadPool::globalInstance()->start(worker);
	}
}

void CGeometryObject::ClearInitializedData()
{
	EdgeList.Clear();
	BoundingBox	= CBoundingBox();
}

void CGeometryObject::ClearAnalyzedData()
{
	OpenGLVertices.clear();

	MinTriangleArea	= std::numeric_limits<double>().max();
	MaxTriangleArea	= std::numeric_limits<double>().min();
	TotalArea		= 0.0f;
	bIsClosed		= true;
}

void CGeometryObject::MeshInitializerFinished()
{
	WorkersCount--;
	if(WorkersCount == 0)
	{
		this->Analyze();
	}
}

void CGeometryObject::MeshAnalyzerFinished()
{
	WorkersCount--;
	if (WorkersCount == 0)
	{
		this->SetState(EState::Idle);
	}
}

void CGeometryObject::MeshSubdividerFinished()
{
	WorkersCount--;
	if (WorkersCount == 0)
	{
		EdgeList = std::move(TempEdgeList);

		TempEdgeList.Clear();
		TempEdgeToNewVert.clear();

		this->Analyze();
	}
}

void CGeometryObject::ThreadMadeProgress()
{
	emit MadeProgress(TempProgress, TempMaxProgress);
}
