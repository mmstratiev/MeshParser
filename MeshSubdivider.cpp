#include "MeshSubdividerr.h"
#include <QThread>

CMeshSubdivider::CMeshSubdivider(CGeometryObject &inOutObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent)
	: QObject{parent}
	, GeometryObject(inOutObject)
	, BeginIndex(beginIndex)
	, EndIndex(endIndex)
{

}

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

CDCEL CMeshSubdivider::GetResult()
{
	return Destination;
}

void CMeshSubdivider::Work()
{
	CDCEL& Source = GeometryObject.EdgeList;

	size_t oddVertIndex = Source.GetVerticesCount();
	std::unordered_map<TDCEL_EdgeID, TDCEL_VertID, TDCEL_EdgeID> EdgeToVerts;
	size_t facesCount = Source.GetFacesCount();
	for(size_t faceIndex = 0; faceIndex < facesCount; faceIndex++)
	{
		CFaceEdgesIterator edgeIt = Source.GetFace(faceIndex)->GetFaceEdgesIterator();
		while(!edgeIt.End())
		{
			// 1. Create even(old) verts
			TDCEL_VertPtr origVert = (*edgeIt)->Origin();

			// 1.1. calculate even vert position

			Destination.AddVertex(origVert->GetID(), origVert->Get());

			// 2. Create odd(new) verts
			TDCEL_EdgeID origEdgeID = (*edgeIt)->GetID();

			auto mapIt = EdgeToVerts.find(origEdgeID);
			if(mapIt == EdgeToVerts.end())
			{
				TDCEL_VertPtr origEdgeBegin = (*edgeIt)->Origin();
				TDCEL_VertPtr origEdgeEnd	= (*edgeIt)->Next()->Origin();

				QVector3D newVertLoc = (origEdgeBegin->Get() + origEdgeEnd->Get()) / 2;

				// 2.1. calculate odd vert position

				EdgeToVerts[origEdgeID] = Destination.AddVertex(oddVertIndex++, newVertLoc)->GetID();
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
	}

	Source = Destination;
}
