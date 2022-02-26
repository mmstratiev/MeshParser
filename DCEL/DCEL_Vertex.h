#ifndef CDCEL_VERTEX_H
#define CDCEL_VERTEX_H

#include <QVector3D>
#include "DCEL_Datatypes.h"
#include "Iterators/VertexFacesIterator.h"
#include "Iterators/VertexVerticesIterator.h"

class CDCEL_Vertex
{
public:
	CDCEL_Vertex(TDCEL_VertID id);

	QVector3D Get() const;
	TDCEL_VertID GetID() const;

	void SetVertex(const QVector3D &newVertex);

	TDCEL_EdgePtr Edge() const;
	void SetEdge(TDCEL_EdgePtr newEdge);

	bool IsBoundary() const;
	QVector3D GetNormal() const;

	std::vector<TDCEL_VertPtr> GetAdjacentVertices() const;

	CVertexFacesIterator	GetAdjacentFacesIterator() const;
	CVertexVerticesIterator GetAdjacentVerticesIterator(bool clockwise = true) const;

private:
	TDCEL_VertID	ID = 0;

	QVector3D		mVertex;
	TDCEL_EdgePtr	mEdge = nullptr;
};

#endif // CDCEL_VERTEX_H
