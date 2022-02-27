#ifndef DCEL_EDGE_H
#define DCEL_EDGE_H

#include "DCEL_Datatypes.h"
#include "Math/Shapes.h"

class CDCEL_Edge
{
public:
	CDCEL_Edge(TDCEL_EdgeID id);
	SEdge Get() const;

	TDCEL_VertPtr Origin() const;
	TDCEL_EdgePtr Twin() const;
	TDCEL_EdgePtr Prev() const;
	TDCEL_EdgePtr Next() const;
	TDCEL_FacePtr Face() const;

	TDCEL_EdgePtr AdjacentCW() const;
	TDCEL_EdgePtr AdjacentCCW() const;

	bool IsBoundary() const;

	void SetOrigin(TDCEL_VertPtr newOrigin);
	void SetTwin(TDCEL_EdgePtr newTwin);
	void SetNext(TDCEL_EdgePtr newNext);
	void SetPrev(TDCEL_EdgePtr newPrev);
	void SetFace(TDCEL_FacePtr newFace);

	const TDCEL_EdgeID &GetID() const;

private:
	TDCEL_EdgeID ID;

	TDCEL_VertPtr mOrigin	= nullptr;
	TDCEL_EdgePtr mTwin		= nullptr;
	TDCEL_EdgePtr mNext		= nullptr;
	TDCEL_EdgePtr mPrev		= nullptr;
	TDCEL_FacePtr mFace		= nullptr;
};

#endif // DCEL_EDGE_H
