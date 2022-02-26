#include "FaceVerticesIterator.h"
#include "DCEL/DCEL_Face.h"
#include "DCEL/DCEL_Edge.h"

CFaceVerticesIterator::CFaceVerticesIterator(const CDCEL_Face* face)
	: Face(face)
	, Edge(face->Edge())
{}

CFaceVerticesIterator &CFaceVerticesIterator::operator+=(const int &increment)
{
	int remaining = increment;
	while(remaining-- > 0 && !this->End())
	{
		this->operator++();
	}
	return *this;
}

void CFaceVerticesIterator::operator++()
{
	if(!Edge) return;

	if (Edge->Next() == Face->Edge())
	{
		Edge = nullptr;
	}
	else
	{
		Edge = Edge->Next();
	}
}

TDCEL_VertPtr CFaceVerticesIterator::operator*()
{
	return this->Value();
}

TDCEL_VertPtr CFaceVerticesIterator::Value()
{
	return Edge->Origin();
}

bool CFaceVerticesIterator::End()
{
	return !Edge;
}

void CFaceVerticesIterator::Reset()
{
	Edge = Face->Edge();
}
