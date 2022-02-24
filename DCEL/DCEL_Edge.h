#ifndef DCEL_EDGE_H
#define DCEL_EDGE_H

#include "DCEL_Datatypes.h"

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

	void SetOrigin(TDCEL_VertPtr newOrigin);
	void SetTwin(TDCEL_EdgePtr newTwin);
	void SetNext(TDCEL_EdgePtr newNext);
	void SetPrev(TDCEL_EdgePtr newPrev);
	void SetFace(TDCEL_FacePtr newFace);

private:
	TDCEL_EdgeID ID;

	TDCEL_VertPtr mOrigin;
	TDCEL_EdgePtr mTwin;
	TDCEL_EdgePtr mNext;
	TDCEL_EdgePtr mPrev;
	TDCEL_FacePtr mFace;
};

#endif // DCEL_EDGE_H
