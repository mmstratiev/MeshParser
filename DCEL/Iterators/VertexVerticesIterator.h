#ifndef CVERTEXVERTICESITERATOR_H
#define CVERTEXVERTICESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CVertexVerticesIterator
{
public:
	CVertexVerticesIterator(const CDCEL_Vertex* v, bool clockwise = true);

	void			operator++();
	TDCEL_VertPtr	operator*();

	TDCEL_VertPtr	Value();
	bool			End();
	void			Reset();

	void			SetClockwise(bool clockwise);

private:
	TDCEL_EdgePtr	GetAdjacentEdge();

private:
	const CDCEL_Vertex*		Vertex			= nullptr;

	TDCEL_VertPtr			CurretnVertex	= nullptr;
	TDCEL_EdgePtr			Edge			= nullptr;
	bool					bClockwise		= true;

};

#endif // CVERTEXVERTICESITERATOR_H
