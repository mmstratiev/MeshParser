#include "FaceEdgesIterator.h"
#include "DCEL/DCEL_Face.h"
#include "DCEL/DCEL_Edge.h"

CFaceEdgesIterator::CFaceEdgesIterator(TDCEL_FacePtr face)
	: Face(face)
	, Edge(face->Edge())
{}

void CFaceEdgesIterator::operator++()
{
	if (Edge->Next() == Face->Edge())
	{
		Edge = nullptr;
	}
	else
	{
		Edge = Edge->Next();
	}
}

TDCEL_EdgePtr CFaceEdgesIterator::operator*()
{
	return Edge;
}

TDCEL_EdgePtr CFaceEdgesIterator::Value()
{
	return Edge;
}

bool CFaceEdgesIterator::End()
{
	return !Edge;
}

void CFaceEdgesIterator::Reset()
{
	Edge = Face->Edge();
}
