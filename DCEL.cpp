#include "DCEL.h"


CDCEL::CDCEL() {}

TDCEL_EdgePtr CDCEL::AddEdge(TDCEL_EdgeID ID, TDCEL_VertPtr origin, TDCEL_EdgePtr next, TDCEL_EdgePtr prev, TDCEL_FacePtr face)
{
	TDCEL_EdgePtr result;

	auto edgeIt = Edges.find(ID);
	if(edgeIt == Edges.end())
	{
		auto insertIt = Edges.insert({ ID, std::make_unique<SDCEL_Edge>() });
		result = insertIt.first->second.get();
	}
	else
	{
		result = edgeIt->second.get();
	}

	auto twinIt = Edges.find(ID.GetTwin());
	if(twinIt != Edges.end())
	{
		twinIt->second->Twin = result;
		result->Twin = twinIt->second.get();
	}

	result->Origin		= origin;
	result->Next		= next;
	result->Prev		= prev;
	result->Face		= face;
	return result;
}

TDCEL_VertPtr CDCEL::AddVertex(TDCEL_VertID ID, QVector3D vertex, TDCEL_EdgePtr halfEdge)
{
	TDCEL_VertPtr result;

	auto vertIt = Vertices.find(ID);
	if(vertIt == Vertices.end())
	{
		auto insertIt = Vertices.insert({ ID, std::make_unique<SDCEL_Vertex>() });
		result = insertIt.first->second.get();
	}
	else
	{
		result = vertIt->second.get();
	}

	result->Vertex		= vertex;
	result->Edge		= halfEdge;
	return result;
}

TDCEL_FacePtr CDCEL::AddFace(TDCEL_FaceID ID, TDCEL_EdgePtr halfEdge)
{
	TDCEL_FacePtr result;

	auto faceIt = Faces.find(ID);
	if(faceIt == Faces.end())
	{
		auto insertIt = Faces.insert({ ID, std::make_unique<SDCEL_Face>() });
		result = insertIt.first->second.get();
	}
	else
	{
		result = faceIt->second.get();
	}

	result->Edge		= halfEdge;
	return result;
}
