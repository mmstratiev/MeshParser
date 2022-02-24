#ifndef CVERTEXFACESITERATOR_H
#define CVERTEXFACESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CVertexFacesIterator
{
public:
	CVertexFacesIterator(TDCEL_VertPtr v);

	void			operator++();
	TDCEL_FacePtr	operator*();

	TDCEL_FacePtr	Value();
	bool			End();
	void			Reset();

private:
	TDCEL_EdgePtr	GetNextTwin();

private:
	bool			bFoundBoundary	= false;
	bool			bClockwise		= true;
	TDCEL_VertPtr	Vertex;
	TDCEL_EdgePtr	Edge;


};

#endif // CVERTEXFACESITERATOR_H
