#include "VertexFacesIterator.h"
#include "DCEL/DCEL_Vertex.h"
#include "DCEL/DCEL_Edge.h"

CVertexFacesIterator::CVertexFacesIterator(TDCEL_VertPtr v)
	: Vertex(v)
	, Edge(v->Edge())
{
}

void CVertexFacesIterator::operator++()
{
	if (Edge == Vertex->Edge())
	{
		Edge = nullptr;
	}
	else
	{
		TDCEL_EdgePtr twin = this->GetNextTwin();
		if(twin)
		{
			Edge = twin->Next();
		}
		else if(!bFoundBoundary)
		{
			// found boundary for this direction, reset and start iterating in the opposite direction
			bFoundBoundary = true;
			bClockwise = !bClockwise;
			this->Reset();
			this->operator++();
		}
		else
		{
			// found boundary for this direction also, all faces are now iterated, end iterator
			Edge = nullptr;
		}
	}
}

TDCEL_FacePtr CVertexFacesIterator::operator*()
{
	return Value();
}

TDCEL_FacePtr CVertexFacesIterator::Value()
{
	return Edge->Face();
}

bool CVertexFacesIterator::End()
{
	return Edge == nullptr;
}

void CVertexFacesIterator::Reset()
{
	Edge = Vertex->Edge();
}

TDCEL_EdgePtr CVertexFacesIterator::GetNextTwin()
{
	TDCEL_EdgePtr result;
	if(bClockwise)
	{
		result = Edge->Twin();
	}
	else
	{
		result = Edge->Prev()->Twin();
	}
	return result;
}
