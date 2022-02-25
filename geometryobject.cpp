#include "GeometryObject.h"
#include "MeshReader.h"
#include "MeshAnalyzer.h"
#include "MeshSubdividerr.h"

#include <QJsonDocument>
#include <QJsonArray>
#include <QThreadPool>
#include <QThread>
#include <QMutexLocker>

CGeometryObject::CGeometryObject()
{
}

void CGeometryObject::Init(const QByteArray &jsonByteArr)
{
	EdgeList.Clear();
	this->Initialize(jsonByteArr);
}

void CGeometryObject::Init(CDCEL& edgeList)
{
	EdgeList = edgeList;
	this->Analyze();
}

bool CGeometryObject::IsInitialized() const
{
	return EdgeList.GetVerticesCount() > 0 && State == EState::Idle;
}

void CGeometryObject::WaitForIdle(TOnIdleCallback callback) const
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

bool CGeometryObject::GetTriangle(qsizetype triangleID, STriangle& outTriangle) const
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

void CGeometryObject::Subdivide()
{
	this->SetState(EState::Initializing);

	CMeshSubdivider *worker = new CMeshSubdivider(*this, 0, 0);
	worker->setObjectName("SubdividerWorker_" + QString::number(0));
	worker->setAutoDelete(true);

	Workers.insert(worker);
	connect(worker, &CMeshSubdivider::Finished, this, &CGeometryObject::MeshInitializerFinished, Qt::QueuedConnection);

	QThreadPool::globalInstance()->start(worker);

	//// We want to use ALL available threads
	//int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	//qsizetype trianglesCount		= this->GetTrianglesCount();
	//qsizetype trianglesPerThread	= trianglesCount / maxThreadCount;
	//qsizetype remainder				= trianglesCount % maxThreadCount;

	//qsizetype lastEndIndex			= 0;
	//for(int i = 0; i < maxThreadCount; i++)
	//{
	//	// remainder can never be more than the divisor(num of threads), so this is a safe and efficent way to distribute triangles
	//	qsizetype trianglesForThread = trianglesPerThread;
	//	if(remainder > 0)
	//	{
	//		trianglesForThread++;
	//		remainder--;
	//	}

	//	qsizetype beginIndex	= lastEndIndex;
	//	qsizetype endIndex		= beginIndex + trianglesForThread;
	//	lastEndIndex			= endIndex;

	//	CMeshSubdivider *worker = new CMeshSubdivider(*this, beginIndex, endIndex);
	//	worker->setObjectName("SubdividerWorker_" + QString::number(i));
	//	worker->setAutoDelete(true);

	//	Workers.insert(worker);
	//	connect(worker, &CMeshSubdivider::Finished, this, &CGeometryObject::MeshInitializerFinished, Qt::QueuedConnection);

	//	QThreadPool::globalInstance()->start(worker);
	//}
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

	// We want to use ALL available threads
	int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	QJsonObject jsonDataObject		= QJsonDocument::fromJson(rawData).object();
	qsizetype trianglesCount		= CMeshReader::GetTrianglesCountRaw(jsonDataObject);
	qsizetype trianglesPerThread	= trianglesCount / maxThreadCount;
	qsizetype remainder				= trianglesCount % maxThreadCount;

	qsizetype lastEndIndex			= 0;
	for(int i = 0; i < maxThreadCount; i++)
	{
		// remainder can never be more than the divisor(num of threads), so this is a safe and efficent way to distribute triangles
		qsizetype trianglesForThread = trianglesPerThread;
		if(remainder > 0)
		{
			trianglesForThread++;
			remainder--;
		}

		qsizetype beginIndex	= lastEndIndex;
		qsizetype endIndex		= beginIndex + trianglesForThread;
		lastEndIndex			= endIndex;

		CMeshReader *worker = new CMeshReader(*this, jsonDataObject, beginIndex, endIndex);
		worker->setObjectName("InitializerWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		Workers.insert(worker);
		connect(worker, &CMeshReader::Finished, this, &CGeometryObject::MeshInitializerFinished, Qt::QueuedConnection);

		QThreadPool::globalInstance()->start(worker);
	}
}

void CGeometryObject::Analyze()
{
	this->SetState(EState::Analyzing);
	this->ClearMeshDetails();

	// We want to use ALL available threads
	int maxThreadCount	= QThreadPool::globalInstance()->maxThreadCount();

	qsizetype trianglesCount		= this->GetTrianglesCount();
	qsizetype trianglesPerThread	= trianglesCount / maxThreadCount;
	qsizetype remainder				= trianglesCount % maxThreadCount;

	qsizetype lastEndIndex			= 0;
	for(int i = 0; i < maxThreadCount; i++)
	{
		// remainder can never be more than the divisor(num of threads), so this is a safe and efficent way to distribute work
		qsizetype edgesForThread = trianglesPerThread;
		if(remainder > 0)
		{
			edgesForThread++;
			remainder--;
		}

		qsizetype beginIndex	= lastEndIndex;
		qsizetype endIndex		= beginIndex + edgesForThread;
		lastEndIndex			= endIndex;

		CMeshAnalyzer *worker = new CMeshAnalyzer(*this, beginIndex, endIndex);
		worker->setObjectName("AnalyzerWorker_" + QString::number(i));
		worker->setAutoDelete(true);

		Workers.insert(worker);
		connect(worker, &CMeshAnalyzer::Finished, this, &CGeometryObject::MeshAnalyzerFinished, Qt::QueuedConnection);

		QThreadPool::globalInstance()->start(worker);
	}
}

void CGeometryObject::ClearMeshDetails()
{
	MinTriangleArea	= std::numeric_limits<double>().max();
	MaxTriangleArea	= std::numeric_limits<double>().min();
	TotalArea		= 0.0f;
	bIsClosed		= true;
}

void CGeometryObject::MeshInitializerFinished()
{
	Workers.remove(sender());
	if(Workers.isEmpty())
	{
		this->Analyze();
	}
}

void CGeometryObject::MeshAnalyzerFinished()
{
	Workers.remove(sender());
	if(Workers.isEmpty())
	{
		this->SetState(EState::Idle);
	}
}
