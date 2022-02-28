#ifndef CTRIANGLE_H
#define CTRIANGLE_H

#include <QVector3D>
#include "Shapes.h"

class CTriangle
{
public:
	CTriangle();
	CTriangle(QVector3D vert1, QVector3D vert2, QVector3D vert3);
	CTriangle(SEdge edge1, SEdge edge2);

	const QVector3D& Vertices(size_t vertIndex) const;

	SEdge GetSideA() const;
	SEdge GetSideB() const;
	SEdge GetSideC() const;

	double GetPerimeter() const;
	double GetArea() const;

	QVector3D GetNormal() const;

	bool IsDegenerate() const;
	bool Intersects(const QVector3D& origin, const QVector3D& dir) const;

private:
	QVector3D mVertices[3];

};

#endif // CTRIANGLE_H
