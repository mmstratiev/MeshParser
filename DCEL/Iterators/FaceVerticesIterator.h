#ifndef CFACEVERTICESITERATOR_H
#define CFACEVERTICESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CFaceVerticesIterator
{
public:
	CFaceVerticesIterator(TDCEL_FacePtr face);

	CFaceVerticesIterator&	operator+=(const int& increment);
	void					operator++();
	TDCEL_VertPtr			operator*();

	TDCEL_VertPtr	Value();
	bool			End();
	void			Reset();

private:
	TDCEL_FacePtr	Face	= nullptr;
	TDCEL_EdgePtr	Edge	= nullptr;
};

#endif // CFACEVERTICESITERATOR_H
