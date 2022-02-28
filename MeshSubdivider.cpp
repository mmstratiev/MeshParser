#include "MeshSubdivider.h"
#include <QThread>

CMeshSubdivider::CMeshSubdivider(CGeometryObject &inOutObject, ESubdivisionAlgorithm algo, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, Algo(algo)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{}

void CMeshSubdivider::run()
{
	// Starting the thread
	//qInfo() << "Starting" << this << QThread::currentThread();

	this->Work();

	//qInfo() << "Finishing" << this << QThread::currentThread();
	emit Finished();

	// todo: remove auto deletion and this sleep
	QThread::currentThread()->msleep(100);
	// Finishing the thread
}

void CMeshSubdivider::Work()
{
	std::unordered_map<TDCEL_EdgeID, TDCEL_VertID, TDCEL_EdgeID> EdgeToVerts;

	CDCEL& Source			= GeometryObject.EdgeList;
	size_t lastOddVertID	= Source.GetVerticesCount();

	size_t facesCount = Source.GetFacesCount();
	for(size_t faceIndex = 0; faceIndex < facesCount; faceIndex++)
	{
		TDCEL_FacePtr currentFace = Source.GetFace(faceIndex);
		if(!currentFace)
		{
			GeometryObject.MaxProgress--;
			emit MadeProgress();
			continue;
		};

		CFaceEdgesIterator edgeIt = currentFace->GetFaceEdgesIterator();
		while(!edgeIt.End())
		{
			// 1. Create even verts
			TDCEL_VertPtr origVert = (*edgeIt)->Origin();
			std::vector<TDCEL_VertPtr> adjacentVerts = origVert->GetAdjacentVertices();

			Destination.AddVertex(origVert->GetID(), this->GetEvenVertPosition(origVert));

			// 2. Create odd(new) verts
			TDCEL_EdgeID origEdgeID = (*edgeIt)->GetID();

			auto mapIt = EdgeToVerts.find(origEdgeID.GetTwin());
			if(mapIt == EdgeToVerts.end())
			{
				EdgeToVerts[origEdgeID] = Destination.AddVertex(lastOddVertID++, this->GetOddVertPosition(*edgeIt))->GetID();
			}
			else
			{
				// Original twin edge was already subdivided, assign that vertex
				EdgeToVerts[origEdgeID] = Destination.GetVertex(mapIt->second)->GetID();
			}

			++edgeIt;
		}

		// 3. Connect even and odd verts to form outer triangles
		edgeIt.Reset();

		while(!edgeIt.End())
		{
			TDCEL_EdgeID origEdgeID = (*edgeIt)->GetID();
			auto mapItCurr= EdgeToVerts.find(origEdgeID);
			auto mapItPrev = EdgeToVerts.find((*edgeIt)->Prev()->GetID());

			TDCEL_VertID vertID1 = Destination.GetVertex(origEdgeID.ID1)->GetID();
			TDCEL_VertID vertID2 = mapItCurr->second;
			TDCEL_VertID vertID3 = mapItPrev->second;

			Destination.Connect(vertID1, vertID2, vertID3);
			++edgeIt;
		}

		// 4. Connect odd verts to form inner triangle
		edgeIt.Reset();
		TDCEL_VertID innerVertID1 = EdgeToVerts.find((*edgeIt)->GetID())->second;
		TDCEL_VertID innerVertID2 = EdgeToVerts.find((*edgeIt)->Next()->GetID())->second;
		TDCEL_VertID innerVertID3 = EdgeToVerts.find((*edgeIt)->Prev()->GetID())->second;

		Destination.Connect(innerVertID1, innerVertID2, innerVertID3);

		GeometryObject.Progress++;
		emit MadeProgress();
	}

	Source = Destination;
}

QVector3D CMeshSubdivider::GetEvenVertPosition(TDCEL_VertPtr originalVert) const
{
	if(Algo == ESubdivisionAlgorithm::Simple) return originalVert->Get();

	QVector3D result;;
	std::vector<TDCEL_VertPtr> adjacentVerts = originalVert->GetAdjacentVertices();

	if(originalVert->IsBoundary())
	{
		QVector3D adjacentSum = adjacentVerts[0]->Get() + adjacentVerts[adjacentVerts.size()-1]->Get();
		result = originalVert->Get() * 0.75f + adjacentSum * 0.125f;
	}
	else
	{
		qsizetype adjacentVertsCount = adjacentVerts.size();
		QVector3D adjacentVertsSum;
		for(TDCEL_VertPtr adjacentVert : adjacentVerts)
		{
			adjacentVertsSum += adjacentVert->Get();
		}

		float alpha = 0.1875f;
		if(adjacentVertsCount > 3)
		{
			alpha = (0.625f - pow(0.375f + (0.25f * std::cos(2 * M_PI / adjacentVertsCount)), 2)) / adjacentVertsCount;
		}

		result = originalVert->Get() * (1 - adjacentVertsCount * alpha) + adjacentVertsSum * alpha;
	}

	return result;
}

QVector3D CMeshSubdivider::GetOddVertPosition(TDCEL_EdgePtr originalEdge) const
{
	QVector3D result;

	TDCEL_VertPtr origEdgeBegin = originalEdge->Origin();
	TDCEL_VertPtr origEdgeEnd	= originalEdge->Next()->Origin();

	if(originalEdge->IsBoundary() || Algo == ESubdivisionAlgorithm::Simple)
	{
		result = (origEdgeBegin->Get() + origEdgeEnd->Get()) / 2.0f;
	}
	else
	{
		result = (origEdgeBegin->Get() + origEdgeEnd->Get()) * 0.375f
				 + (originalEdge->Prev()->Origin()->Get() + originalEdge->Twin()->Prev()->Origin()->Get()) * 0.125f;
	}
	return result;
}
