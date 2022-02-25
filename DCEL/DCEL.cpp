#include "DCEL.h"
#include <QDebug>

CDCEL::CDCEL() {}

void CDCEL::Clear()
{
	Vertices.clear();
	Edges.clear();
	Faces.clear();
}

TDCEL_VertPtr CDCEL::AddVertex(TDCEL_VertID ID, QVector3D vertex)
{
	TDCEL_VertPtr result;

	auto vertIt = Vertices.find(ID);
	if(vertIt == Vertices.end())
	{
		auto insertIt = Vertices.insert({ ID, std::make_unique<CDCEL_Vertex>(ID) });
		result = insertIt.first->second.get();
	}
	else
	{
		result = vertIt->second.get();
	}

	result->SetVertex(vertex);
	return result;
}

TDCEL_FacePtr CDCEL::Connect(TDCEL_VertID vert1Id, TDCEL_VertID vert2Id, TDCEL_VertID vert3Id, TDCEL_FaceID newFaceId)
{
	TDCEL_VertPtr vert1 = this->GetVertex(vert1Id);
	TDCEL_VertPtr vert2 = this->GetVertex(vert2Id);
	TDCEL_VertPtr vert3 = this->GetVertex(vert3Id);

	TDCEL_EdgeID edge1ID(vert1Id, vert2Id);
	TDCEL_EdgePtr edge1Ptr = this->AddEdge(edge1ID);

	TDCEL_EdgeID edge2ID(vert2Id, vert3Id);
	TDCEL_EdgePtr edge2Ptr = this->AddEdge(edge2ID);

	TDCEL_EdgeID edge3ID(vert3Id, vert1Id);
	TDCEL_EdgePtr edge3Ptr = this->AddEdge(edge3ID);

	TDCEL_FaceID faceID = newFaceId;
	TDCEL_FacePtr facePtr = this->AddFace(faceID);

	vert1->SetEdge(edge1Ptr);
	vert2->SetEdge(edge2Ptr);
	vert3->SetEdge(edge3Ptr);

	facePtr->SetEdge(edge1Ptr);

	edge1Ptr->SetNext(edge2Ptr);
	edge1Ptr->SetPrev(edge3Ptr);
	edge1Ptr->SetOrigin(vert1);
	edge1Ptr->SetFace(facePtr);

	edge2Ptr->SetNext(edge3Ptr);
	edge2Ptr->SetPrev(edge1Ptr);
	edge2Ptr->SetOrigin(vert2);
	edge2Ptr->SetFace(facePtr);

	edge3Ptr->SetNext(edge1Ptr);
	edge3Ptr->SetPrev(edge2Ptr);
	edge3Ptr->SetOrigin(vert3);
	edge3Ptr->SetFace(facePtr);

	return facePtr;
}

TDCEL_FacePtr CDCEL::Connect(TDCEL_VertID vert1, TDCEL_VertID vert2, TDCEL_VertID vert3)
{
	return this->Connect(vert1, vert2, vert3, Faces.size());
}

qsizetype CDCEL::GetEdgesCount() const
{
	return Edges.size();
}

TDCEL_EdgePtr CDCEL::GetEdge(TDCEL_EdgeID Id) const
{
	TDCEL_EdgePtr result = nullptr;

	auto edgeIt = Edges.find(Id);
	if(edgeIt != Edges.end())
	{
		result = edgeIt->second.get();
	}

	return result;
}

qsizetype CDCEL::GetVerticesCount() const
{
	return Vertices.size();
}

TDCEL_VertPtr CDCEL::GetVertex(TDCEL_VertID Id) const
{
	TDCEL_VertPtr result = nullptr;

	auto vertIt = Vertices.find(Id);
	if(vertIt != Vertices.end())
	{
		result = vertIt->second.get();
	}

	return result;
}

qsizetype CDCEL::GetFacesCount() const
{
	return Faces.size();
}

TDCEL_FacePtr CDCEL::GetFace(TDCEL_FaceID Id) const
{
	TDCEL_FacePtr result = nullptr;

	auto faceIt = Faces.find(Id);
	if(faceIt != Faces.end())
	{
		result = faceIt->second.get();
	}

	return result;
}

TDCEL_FacePtr CDCEL::AddFace(TDCEL_FaceID ID)
{
	TDCEL_FacePtr result;

	auto faceIt = Faces.find(ID);
	if(faceIt == Faces.end())
	{
		auto insertIt = Faces.insert({ ID, std::make_unique<CDCEL_Face>(ID) });
		result = insertIt.first->second.get();
	}
	else
	{
		result = faceIt->second.get();
	}

	return result;
}

TDCEL_EdgePtr CDCEL::AddEdge(TDCEL_EdgeID ID)
{
	TDCEL_EdgePtr result;

	auto edgeIt = Edges.find(ID);
	if(edgeIt == Edges.end())
	{
		auto insertIt = Edges.insert({ ID, std::make_unique<CDCEL_Edge>(ID) });
		result = insertIt.first->second.get();
	}
	else
	{
		result = edgeIt->second.get();
	}

	auto twinIt = Edges.find(ID.GetTwin());
	if(twinIt != Edges.end())
	{
		twinIt->second->SetTwin(result);;
		result->SetTwin(twinIt->second.get());
	}

	return result;
}
