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
	this->Work();
	emit Finished();
	QThread::currentThread()->msleep(25);
}

void CMeshSubdivider::Work()
{
	CDCEL& Source			= GeometryObject.EdgeList;
	CDCEL& Destination		= GeometryObject.TempEdgeList;

	for(qsizetype faceIndex = BeginIndex; faceIndex < EndIndex; faceIndex++)
	{
		QMutexLocker locker(&GeometryObject.Mutex);
		TDCEL_FacePtr currentFace = Source.GetFace(faceIndex);
		if(!currentFace)
		{
			GeometryObject.TempMaxProgress--;
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

			auto mapIt = GeometryObject.TempEdgeToNewVert.find(origEdgeID.GetTwin());
			if(mapIt == GeometryObject.TempEdgeToNewVert.end())
			{
				qsizetype newVertID = GeometryObject.TempLastOddVertID++;
				GeometryObject.TempEdgeToNewVert[origEdgeID] = Destination.AddVertex(newVertID, this->GetOddVertPosition(*edgeIt))->GetID();
			}
			else
			{
				// Original twin edge was already subdivided, assign that vertex
				GeometryObject.TempEdgeToNewVert[origEdgeID] = Destination.GetVertex(mapIt->second)->GetID();
			}

			++edgeIt;
		}

		// 3. Connect even and odd verts to form outer triangles
		edgeIt.Reset();

		while(!edgeIt.End())
		{
			TDCEL_EdgeID origEdgeID = (*edgeIt)->GetID();
			auto mapItCurr= GeometryObject.TempEdgeToNewVert.find(origEdgeID);
			auto mapItPrev = GeometryObject.TempEdgeToNewVert.find((*edgeIt)->Prev()->GetID());

			TDCEL_VertID vertID1 = Destination.GetVertex(origEdgeID.ID1)->GetID();
			TDCEL_VertID vertID2 = mapItCurr->second;
			TDCEL_VertID vertID3 = mapItPrev->second;

			Destination.Connect(vertID1, vertID2, vertID3);
			++edgeIt;
		}

		// 4. Connect odd verts to form inner triangle
		edgeIt.Reset();
		TDCEL_VertID innerVertID1 = GeometryObject.TempEdgeToNewVert.find((*edgeIt)->GetID())->second;
		TDCEL_VertID innerVertID2 = GeometryObject.TempEdgeToNewVert.find((*edgeIt)->Next()->GetID())->second;
		TDCEL_VertID innerVertID3 = GeometryObject.TempEdgeToNewVert.find((*edgeIt)->Prev()->GetID())->second;


		Destination.Connect(innerVertID1, innerVertID2, innerVertID3);
		GeometryObject.TempProgress++;
		emit MadeProgress();
	}
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
