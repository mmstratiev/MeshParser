#include "VertexFacesIterator.h"
#include "DCEL/DCEL_Vertex.h"
#include "DCEL/DCEL_Edge.h"
#include <QDebug>

CVertexFacesIterator::CVertexFacesIterator(const CDCEL_Vertex* v, bool clockwise)
	: Vertex(v)
	, Edge(v->Edge())
	, bClockwise(clockwise)
{
}

void CVertexFacesIterator::operator++()
{
	Edge = this->GetAdjacentEdge();
	if (Edge == Vertex->Edge())
	{
		Edge = nullptr;
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
