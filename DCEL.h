#ifndef DCEL_H
#define DCEL_H

#include <QVector3D>
#include <unordered_map>

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


struct SDCEL_Vertex;
struct SDCEL_Edge;
struct SDCEL_Face;

struct SDCEL_EdgeID
{
	qsizetype ID1 = 0;
	qsizetype ID2 = 0;

	SDCEL_EdgeID() {}

	SDCEL_EdgeID(qsizetype id1, qsizetype id2)
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

using TDCEL_EdgeID	= SDCEL_EdgeID;
using TDCEL_VertID	= qsizetype;
using TDCEL_FaceID	= qsizetype;

using TDCEL_EdgePtr	= SDCEL_Edge*;
using TDCEL_VertPtr	= SDCEL_Vertex*;
using TDCEL_FacePtr	= SDCEL_Face*;

struct SDCEL_Vertex
{
	QVector3D		Vertex;
	TDCEL_EdgePtr	Edge;

	QVector3D Get() const
	{
		return Vertex;
	}
};

struct SDCEL_Edge
{
	TDCEL_VertPtr Origin;
	TDCEL_EdgePtr Twin;
	TDCEL_EdgePtr Next;
	TDCEL_EdgePtr Prev;
	TDCEL_FacePtr Face;

	SEdge Get() const
	{
		return SEdge(Origin->Get(), Next->Origin->Get());
	}
};

struct SDCEL_Face
{
	TDCEL_EdgePtr	Edge;

	STriangle Get() const
	{
		return STriangle(Edge->Get(), Edge->Next->Get());;
	}
};

// Doubly connected edge list - https://en.wikipedia.org/wiki/Doubly_connected_edge_list
class CDCEL
{
public:
	CDCEL();

	TDCEL_EdgePtr	AddEdge(TDCEL_EdgeID ID, TDCEL_VertPtr origin = nullptr, TDCEL_EdgePtr next = nullptr, TDCEL_EdgePtr prev = nullptr, TDCEL_FacePtr face = nullptr);
	TDCEL_VertPtr	AddVertex(TDCEL_VertID ID, QVector3D vertex, TDCEL_EdgePtr halfEdge = nullptr);
	TDCEL_FacePtr	AddFace(TDCEL_FaceID ID, TDCEL_EdgePtr halfEdge = nullptr);

	qsizetype		GetEdgesCount() const;
	TDCEL_EdgePtr	GetEdge(TDCEL_EdgeID Id) const;

	qsizetype		GetVerticesCount() const;
	TDCEL_VertPtr	GetVertex(TDCEL_VertID Id) const;
	QVector3D		GetVertexNormal(TDCEL_VertID Id) const;

	qsizetype		GetFacesCount() const;
	TDCEL_FacePtr	GetFace(TDCEL_FaceID Id) const;

private:
	std::unordered_map<TDCEL_EdgeID, std::unique_ptr<SDCEL_Edge>, SDCEL_EdgeID>	Edges;
	std::unordered_map<TDCEL_VertID, std::unique_ptr<SDCEL_Vertex>>				Vertices;
	std::unordered_map<TDCEL_FaceID, std::unique_ptr<SDCEL_Face>>				Faces;
};


#endif // DCEL_H
