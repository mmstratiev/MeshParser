#ifndef CVERTEXFACESITERATOR_H
#define CVERTEXFACESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CVertexFacesIterator
{
public:
	CVertexFacesIterator(const CDCEL_Vertex* v, bool clockwise = true);

	void			operator++();
	TDCEL_FacePtr	operator*();

	TDCEL_FacePtr	Value();
	bool			End();
	void			Reset();

private:
	TDCEL_EdgePtr	GetAdjacentEdge();

private:
	const CDCEL_Vertex*		Vertex			= nullptr;
	TDCEL_EdgePtr			Edge			= nullptr;
	bool					bClockwise		= true;

};

#endif // CVERTEXFACESITERATOR_H
