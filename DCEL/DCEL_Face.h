#ifndef DCEL_FACE_H
#define DCEL_FACE_H

#include "DCEL_Datatypes.h"
#include "Iterators/FaceEdgesIterator.h"

class CDCEL_Face
{
public:
	CDCEL_Face(TDCEL_FaceID id);
	STriangle Get() const;

	TDCEL_EdgePtr Edge() const;
	void SetEdge(TDCEL_EdgePtr newEdge);

	CFaceEdgesIterator GetFaceEdgesIterator();

private:
	TDCEL_FaceID	ID = 0;

	TDCEL_EdgePtr	mEdge = nullptr;
};

#endif // DCEL_FACE_H
