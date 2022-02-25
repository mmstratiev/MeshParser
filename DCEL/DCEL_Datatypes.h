#ifndef DCEL_DATATYPES_H
#define DCEL_DATATYPES_H

#include <cstddef>
#include <QVector3D>

/////////////////////////////////////////////////////////
//////////////////////// Structs ////////////////////////
/////////////////////////////////////////////////////////
struct SDCEL_EdgeID
{
	size_t ID1 = 0;
	size_t ID2 = 0;

	SDCEL_EdgeID() {}

	SDCEL_EdgeID(size_t id1, size_t id2)
		: ID1(id1)
		, ID2(id2)
	{}

	SDCEL_EdgeID GetTwin() const
	{
		return SDCEL_EdgeID(ID2, ID1);
	}

	bool operator==(const SDCEL_EdgeID &other) const
	{
		return ID1 == other.ID1 && ID2 == other.ID2;
	}

	std::size_t operator() (const SDCEL_EdgeID &node) const
	{
		// some random hashing function
		size_t h = (size_t(node.ID1)<<32)+size_t(node.ID2);
		h *= 1231231557ull; // "random" uneven integer
		h ^= (h>>32);
		return h;
	}
};

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

/////////////////////////////////////////////////////////
//////////////////////// Typedefs ///////////////////////
/////////////////////////////////////////////////////////
using TDCEL_EdgeID	= struct SDCEL_EdgeID;
using TDCEL_VertID	= size_t;
using TDCEL_FaceID	= size_t;

using TDCEL_EdgePtr	= class CDCEL_Edge*;
using TDCEL_FacePtr	= class CDCEL_Face*;
using TDCEL_VertPtr	= class CDCEL_Vertex*;

using TDCEL_VertConstPtr	= const class CDCEL_Vertex*;

#endif // DCEL_DATATYPES_H
