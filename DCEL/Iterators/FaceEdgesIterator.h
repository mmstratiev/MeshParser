#ifndef CFACEEDGESITERATOR_H
#define CFACEEDGESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CFaceEdgesIterator
{
public:
	CFaceEdgesIterator(const CDCEL_Face* face);

	void			operator++();
	TDCEL_EdgePtr	operator*();

	TDCEL_EdgePtr	Value();
	bool			End();
	void			Reset();

private:
	const CDCEL_Face*	Face	= nullptr;
	TDCEL_EdgePtr		Edge	= nullptr;

};

#endif // CFACEEDGESITERATOR_H
