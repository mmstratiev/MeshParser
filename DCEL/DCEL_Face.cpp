#include "DCEL_Face.h"
#include "DCEL_Edge.h"

#include <QDebug>

CDCEL_Face::CDCEL_Face(TDCEL_FaceID id)
	: ID(id)
{}

CTriangle CDCEL_Face::Get() const
{
	return CTriangle(Edge()->Get(), Edge()->Next()->Get());
}

TDCEL_EdgePtr CDCEL_Face::Edge() const
{
	return mEdge;
}

void CDCEL_Face::SetEdge(TDCEL_EdgePtr newEdge)
{
	mEdge = newEdge;
}

CFaceEdgesIterator CDCEL_Face::GetFaceEdgesIterator() const
{
	return CFaceEdgesIterator(this);
}

CFaceVerticesIterator CDCEL_Face::GetFaceVerticesIterator() const
{
	return CFaceVerticesIterator(this);
}
