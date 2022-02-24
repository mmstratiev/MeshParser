#include "DCEL_Vertex.h"

CDCEL_Vertex::CDCEL_Vertex(TDCEL_VertID id)
	: ID(id)
{}

QVector3D CDCEL_Vertex::Get() const
{
	return mVertex;
}

void CDCEL_Vertex::SetVertex(const QVector3D &newVertex)
{
	mVertex = newVertex;
}

TDCEL_EdgePtr CDCEL_Vertex::Edge() const
{
	return mEdge;
}

void CDCEL_Vertex::SetEdge(TDCEL_EdgePtr newEdge)
{
	mEdge = newEdge;
}

CVertexFacesIterator CDCEL_Vertex::GetAdjacentFacesIterator()
{
	return CVertexFacesIterator(this);
}

