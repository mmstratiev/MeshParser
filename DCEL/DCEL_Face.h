#ifndef DCEL_FACE_H
#define DCEL_FACE_H

#include "DCEL_Datatypes.h"
#include "Iterators/FaceEdgesIterator.h"
#include "Iterators/FaceVerticesIterator.h"
#include "Math/Triangle.h"

class CDCEL_Face
{
public:
	CDCEL_Face(TDCEL_FaceID id);
	CTriangle Get() const;

	TDCEL_EdgePtr	Edge() const;
	void			SetEdge(TDCEL_EdgePtr newEdge);

	CFaceEdgesIterator		GetFaceEdgesIterator() const;
	CFaceVerticesIterator	GetFaceVerticesIterator() const;

private:
	TDCEL_FaceID	ID = 0;

	TDCEL_EdgePtr	mEdge = nullptr;
};

#endif // DCEL_FACE_H
