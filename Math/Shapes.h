#ifndef SHAPES_H
#define SHAPES_H

#include <QVector3D>

struct SEdge
{
	QVector3D Vertices[2];

	SEdge(QVector3D Vertex1, QVector3D Vertex2)
	{
		Vertices[0] = Vertex1;
		Vertices[1] = Vertex2;
	}

	QVector3D ToVector() const
	{
		return Vertices[1] - Vertices[0];
	}

	double GetLength() const
	{
		return this->ToVector().length();
	}
};

#endif // SHAPES_H
