#ifndef SHAPES_H
#define SHAPES_H

#include <QVector3D>

struct SEdge
{
	SEdge(QVector3D Vertex1, QVector3D Vertex2)
	{
		Vertices[0] = Vertex1;
		Vertices[1] = Vertex2;
	}

	QVector3D Vertices[2];

	QVector3D ToVector() const
	{
		return Vertices[1] - Vertices[0];
	}

	double GetLength() const
	{
		return this->ToVector().length();
	}
};

struct STriangle
{
	QVector3D Vertices[3];

	STriangle(){}

	STriangle(QVector3D vert1, QVector3D vert2, QVector3D vert3)
	{
		Vertices[0] = vert1;
		Vertices[1] = vert2;
		Vertices[2] = vert3;
	}

	STriangle(SEdge edge1, SEdge edge2)
	{
		Vertices[0] = edge1.Vertices[0];
		Vertices[1] = edge1.Vertices[1];
		Vertices[2] = edge2.Vertices[1];
	}

	SEdge GetSideA() const
	{
		return SEdge(Vertices[1], Vertices[2]);
	}

	SEdge GetSideB() const
	{
		return SEdge(Vertices[2], Vertices[0]);
	}

	SEdge GetSideC() const
	{
		return SEdge(Vertices[0], Vertices[1]);
	}

	double GetPerimeter() const
	{
		return this->GetSideA().GetLength() + this->GetSideB().GetLength() + this->GetSideC().GetLength();
	}

	// Heron's formula - https://en.wikipedia.org/wiki/Heron%27s_formula
	double GetArea() const
	{
		double semiP = this->GetPerimeter() / 2.0f;
		return	sqrt(semiP
					*(semiP-this->GetSideA().GetLength())
					*(semiP-this->GetSideB().GetLength())
					*(semiP-this->GetSideC().GetLength()));
	}

	QVector3D GetNormal() const
	{
		return QVector3D::normal(Vertices[0] - Vertices[1], Vertices[2] - Vertices[0]);
	}

	bool IsDegenerate() const
	{
		return qFuzzyIsNull(this->GetArea());
	}
};

#endif // SHAPES_H
