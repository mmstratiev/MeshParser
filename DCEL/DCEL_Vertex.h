#ifndef CDCEL_VERTEX_H
#define CDCEL_VERTEX_H

#include <QVector3D>
#include "DCEL_Datatypes.h"
#include "Iterators/VertexFacesIterator.h"

class CDCEL_Vertex
{
public:
	CDCEL_Vertex(TDCEL_VertID id);

	QVector3D Get() const;
	void SetVertex(const QVector3D &newVertex);

	TDCEL_EdgePtr Edge() const;
	void SetEdge(TDCEL_EdgePtr newEdge);

	CVertexFacesIterator GetAdjacentFacesIterator();

private:
	TDCEL_VertID	ID = 0;

	QVector3D		mVertex;
	TDCEL_EdgePtr	mEdge;
};

#endif // CDCEL_VERTEX_H
