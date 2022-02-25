#ifndef CFACEEDGESITERATOR_H
#define CFACEEDGESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CFaceEdgesIterator
{
public:
	CFaceEdgesIterator(TDCEL_FacePtr face);

	void			operator++();
	TDCEL_EdgePtr	operator*();

	TDCEL_EdgePtr	Value();
	bool			End();
	void			Reset();

private:
	bool			bBegun	= false;

	TDCEL_FacePtr	Face	= nullptr;
	TDCEL_EdgePtr	Edge	= nullptr;

};

#endif // CFACEEDGESITERATOR_H
