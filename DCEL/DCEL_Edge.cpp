#include "DCEL_Edge.h"
#include "DCEL_Vertex.h"

CDCEL_Edge::CDCEL_Edge(TDCEL_EdgeID id)
	: ID(id)
{

}

SEdge CDCEL_Edge::Get() const
{
	return SEdge(Origin()->Get(), Next()->Origin()->Get());
}

TDCEL_VertPtr CDCEL_Edge::Origin() const
{
	return mOrigin;
}

TDCEL_EdgePtr CDCEL_Edge::Twin() const
{
	return mTwin;
}

TDCEL_EdgePtr CDCEL_Edge::Prev() const
{
	return mPrev;
}

TDCEL_EdgePtr CDCEL_Edge::Next() const
{
	return mNext;
}

TDCEL_FacePtr CDCEL_Edge::Face() const
{
	return mFace;
}

TDCEL_EdgePtr CDCEL_Edge::AdjacentCW() const
{
	TDCEL_EdgePtr result = this->Twin();
	if(result)
	{
		result = result->Next();
	}
	return result;
}

TDCEL_EdgePtr CDCEL_Edge::AdjacentCCW() const
{
	return this->Prev()->Twin();
}

bool CDCEL_Edge::IsBoundary() const
{
	return !this->Twin();
}

void CDCEL_Edge::SetOrigin(TDCEL_VertPtr newOrigin)
{
	mOrigin = newOrigin;
}

void CDCEL_Edge::SetTwin(TDCEL_EdgePtr newTwin)
{
	mTwin = newTwin;
}

void CDCEL_Edge::SetNext(TDCEL_EdgePtr newNext)
{
	mNext = newNext;
}

void CDCEL_Edge::SetPrev(TDCEL_EdgePtr newPrev)
{
	mPrev = newPrev;
}

void CDCEL_Edge::SetFace(TDCEL_FacePtr newFace)
{
	mFace = newFace;
}

const TDCEL_EdgeID &CDCEL_Edge::GetID() const
{
	return ID;
}
