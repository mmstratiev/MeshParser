#ifndef CFACEVERTICESITERATOR_H
#define CFACEVERTICESITERATOR_H

#include "DCEL/DCEL_Datatypes.h"

class CFaceVerticesIterator
{
public:
	CFaceVerticesIterator(const CDCEL_Face* face);

	CFaceVerticesIterator&	operator+=(const int& increment);
	void					operator++();
	TDCEL_VertPtr			operator*();

	TDCEL_VertPtr	Value();
	bool			End();
	void			Reset();

private:
	const CDCEL_Face*	Face	= nullptr;
	TDCEL_EdgePtr		Edge	= nullptr;
};

#endif // CFACEVERTICESITERATOR_H
