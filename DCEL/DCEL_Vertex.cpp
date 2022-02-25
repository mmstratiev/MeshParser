#include "DCEL_Vertex.h"
#include "DCEL_Edge.h"
#include "DCEL_Face.h"

CDCEL_Vertex::CDCEL_Vertex(TDCEL_VertID id)
	: ID(id)
{}

QVector3D CDCEL_Vertex::Get() const
{
	return mVertex;
}


TDCEL_VertID CDCEL_Vertex::GetID() const
{
	return ID;
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

bool CDCEL_Vertex::IsBoundary() const
{
	bool result = false;
	TDCEL_EdgePtr currentEdge = this->Edge();
	do
	{
		currentEdge = currentEdge->AdjacentCW();
		if(!currentEdge)
		{
			result = true;
			break;
		}
	} while(currentEdge != this->Edge());
	return result;
}

QVector3D CDCEL_Vertex::GetNormal()
{
	QVector3D result;
	CVertexFacesIterator it = this->GetAdjacentFacesIterator();
	do
	{
		STriangle triangle = (*it)->Get();
		result += triangle.GetNormal();
		++it;
	} while(!it.End());

	return result.normalized();
}

//CVertexFacesIterator CDCEL_Vertex::GetAdjacentFacesIterator() const
//{
//	return CVertexFacesIterator(nullptr);//CVertexFacesIterator(this);
//}

CVertexFacesIterator CDCEL_Vertex::GetAdjacentFacesIterator()
{
	return CVertexFacesIterator(this);
}

