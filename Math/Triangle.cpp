#include "Triangle.h"

CTriangle::CTriangle()
{

}

CTriangle::CTriangle(QVector3D vert1, QVector3D vert2, QVector3D vert3)
	: mVertices{vert1, vert2, vert3}
{

}

CTriangle::CTriangle(SEdge edge1, SEdge edge2)
	: mVertices{edge1.Vertices[0], edge1.Vertices[1], edge2.Vertices[1]}
{}

const QVector3D &CTriangle::Vertices(size_t vertIndex) const
{
	return mVertices[vertIndex];
}

SEdge CTriangle::GetSideA() const
{
	return SEdge(mVertices[1], mVertices[2]);
}

SEdge CTriangle::GetSideB() const
{
	return SEdge(mVertices[2], mVertices[0]);
}

SEdge CTriangle::GetSideC() const
{
	return SEdge(mVertices[0], mVertices[1]);
}

double CTriangle::GetPerimeter() const
{
	return this->GetSideA().GetLength() + this->GetSideB().GetLength() + this->GetSideC().GetLength();
}

double CTriangle::GetArea() const
{
	// Heron's formula - https://en.wikipedia.org/wiki/Heron%27s_formula
	double semiP = this->GetPerimeter() / 2.0f;
	return	sqrt(semiP
				*(semiP-this->GetSideA().GetLength())
				*(semiP-this->GetSideB().GetLength())
				 *(semiP-this->GetSideC().GetLength()));
}

QVector3D CTriangle::GetNormal() const
{
	return QVector3D::normal(mVertices[0] - mVertices[1], mVertices[2] - mVertices[0]);
}

bool CTriangle::IsDegenerate() const
{
	return qFuzzyIsNull(this->GetArea());
}

bool CTriangle::Intersects(const QVector3D &origin, const QVector3D &dir) const
{
	// https://www.scratchapixel.com/lessons/3d-basic-rendering/ray-tracing-rendering-a-triangle/moller-trumbore-ray-triangle-intersection
	QVector3D v0v1 = Vertices(1) - Vertices(0);
	QVector3D v0v2 = Vertices(2) - Vertices(0);

	QVector3D pvec = QVector3D::crossProduct(dir, v0v2);

	float det = QVector3D::dotProduct(v0v1, pvec);

	if (det < 0.000001)
		return false;

	float invDet = 1.0 / det;

	QVector3D tvec = origin - Vertices(0);

	float u = QVector3D::dotProduct(tvec, pvec) * invDet;

	if (u < 0 || u > 1)
		return false;

	QVector3D qvec = QVector3D::crossProduct(tvec, v0v1);

	float v = QVector3D::dotProduct(dir, qvec) * invDet;

	if (v < 0 || u + v > 1)
		return false;

	// intersect point float t = QVector3D::dotProduct(v0v2, qvec) * invDet;

	return true;
}

