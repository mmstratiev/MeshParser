#include "DCEL_Face.h"
#include "DCEL_Edge.h"

#include <QDebug>

CDCEL_Face::CDCEL_Face(TDCEL_FaceID id)
	: ID(id)
{}

STriangle CDCEL_Face::Get() const
{
	return STriangle(Edge()->Get(), Edge()->Next()->Get());
}

TDCEL_EdgePtr CDCEL_Face::Edge() const
{
	return mEdge;
}

void CDCEL_Face::SetEdge(TDCEL_EdgePtr newEdge)
{
	mEdge = newEdge;
}

CFaceEdgesIterator CDCEL_Face::GetFaceEdgesIterator()
{
	return CFaceEdgesIterator(this);
}
