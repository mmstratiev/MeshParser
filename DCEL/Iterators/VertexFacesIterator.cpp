#include "VertexFacesIterator.h"
#include "DCEL/DCEL_Vertex.h"
#include "DCEL/DCEL_Edge.h"

CVertexFacesIterator::CVertexFacesIterator(const CDCEL_Vertex* v)
	: Vertex(v)
	, Edge(v->Edge())
{
}

void CVertexFacesIterator::operator++()
{
	if (bBegun && Edge == Vertex->Edge())
	{
		Edge = nullptr;
	}
	else
	{
		Edge = this->GetAdjacentEdge();
		if(!Edge)
		{
			if(!bFoundBoundary)
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
	bBegun = true;
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
	bBegun = false;
}

TDCEL_EdgePtr CVertexFacesIterator::GetAdjacentEdge()
{
	TDCEL_EdgePtr result;
	if(bClockwise)
	{
		result = Edge->AdjacentCW();
	}
	else
	{
		result = Edge->AdjacentCCW();
	}
	return result;
}
