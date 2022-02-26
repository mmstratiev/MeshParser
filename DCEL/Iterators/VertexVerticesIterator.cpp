#include "VertexVerticesIterator.h"
#include "DCEL/DCEL_Vertex.h"
#include "DCEL/DCEL_Edge.h"

CVertexVerticesIterator::CVertexVerticesIterator(const CDCEL_Vertex* v, bool clockwise)
	: Vertex(v)
	, Edge(v->Edge())
	, bClockwise(clockwise)
{}

void CVertexVerticesIterator::operator++()
{
	Edge = this->GetAdjacentEdge();
	if (Edge == Vertex->Edge())
	{
		Edge = nullptr;
	}
}

TDCEL_VertPtr CVertexVerticesIterator::operator*()
{
	return Value();
}

TDCEL_VertPtr CVertexVerticesIterator::Value()
{
	return Edge->Next()->Origin();
}

bool CVertexVerticesIterator::End()
{
	return Edge == nullptr;
}

void CVertexVerticesIterator::Reset()
{
	Edge = Vertex->Edge();
}

void CVertexVerticesIterator::SetClockwise(bool clockwise)
{
	bClockwise = clockwise;
}

TDCEL_EdgePtr CVertexVerticesIterator::GetAdjacentEdge()
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
