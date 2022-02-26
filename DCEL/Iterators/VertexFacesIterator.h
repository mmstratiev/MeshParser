#ifndef CVERTEXFACESITERATOR_H
#define CVERTEXFACESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CVertexFacesIterator
{
public:
	CVertexFacesIterator(const CDCEL_Vertex* v);

	void			operator++();
	TDCEL_FacePtr	operator*();

	TDCEL_FacePtr	Value();
	bool			End();
	void			Reset();

private:
	TDCEL_EdgePtr	GetAdjacentEdge();

private:
	bool					bBegun			= false;
	bool					bFoundBoundary	= false;
	bool					bClockwise		= true;
	const CDCEL_Vertex*		Vertex			= nullptr;
	TDCEL_EdgePtr			Edge			= nullptr;

};

#endif // CVERTEXFACESITERATOR_H
