#include "DCEL.h"


CDCEL::CDCEL() {}

void CDCEL::Clear()
{
	Vertices.clear();
	Edges.clear();
	Faces.clear();
}

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

QVector3D CDCEL::GetVertexNormal(TDCEL_VertID Id) const
{
	QVector3D result;
	TDCEL_VertPtr vertex = this->GetVertex(Id);
	if(!vertex) return result;

	auto iterateAdjacentTris = [&] (TDCEL_EdgePtr start, bool clockwiseTraversal) -> bool
	{
		TDCEL_EdgePtr currentEdge = vertex->Edge;
		do
		{
			if(currentEdge->Origin == vertex)
			{
				STriangle triangle = currentEdge->Face->Get();
				if(triangle.IsDegenerate())
				{
					return false;
				}
				result += triangle.GetNormal();

				// go back to triangle on the left
				if(!clockwiseTraversal)
				{
					currentEdge = currentEdge->Prev;
				}
				currentEdge = currentEdge->Twin;

				if(!currentEdge) // non-manifold/open mesh
				{
					return false;
				}
			}
			else
			{
				currentEdge = currentEdge->Next;
			}
		} while(currentEdge != start);

		return true;
	};

	if(!iterateAdjacentTris(vertex->Edge, true))
	{
		iterateAdjacentTris(vertex->Edge, false);
	}

	return result.normalized();
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
