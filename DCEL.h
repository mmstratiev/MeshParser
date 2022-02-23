#ifndef DCEL_H
#define DCEL_H

#include <QVector3D>
#include <unordered_map>

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
};

struct SDCEL_Edge
{
	TDCEL_VertPtr Origin;
	TDCEL_EdgePtr Twin;
	TDCEL_EdgePtr Next;
	TDCEL_EdgePtr Prev;
	TDCEL_FacePtr Face;
};

struct SDCEL_Face
{
	TDCEL_EdgePtr Edge;
};

// Doubly connected edge list - https://en.wikipedia.org/wiki/Doubly_connected_edge_list
class CDCEL
{
public:
	CDCEL();

	// twin will be calculated automatically form the id.
	TDCEL_EdgePtr	AddEdge(TDCEL_EdgeID ID, TDCEL_VertPtr origin = nullptr, TDCEL_EdgePtr next = nullptr, TDCEL_EdgePtr prev = nullptr, TDCEL_FacePtr face = nullptr);
	TDCEL_VertPtr	AddVertex(TDCEL_VertID ID, QVector3D vertex, TDCEL_EdgePtr halfEdge = nullptr);
	TDCEL_FacePtr	AddFace(TDCEL_FaceID ID, TDCEL_EdgePtr halfEdge = nullptr);

	// TDCE_EdgePtr	GetEdge(TDCE_EdgeID Id);
	// TDCE_VertPtr	GetVertex(TDCE_VertID Id);
	// TDCE_FacePtr	GetFace(TDCE_FaceID Id);

private:
	std::unordered_map<TDCEL_EdgeID, std::unique_ptr<SDCEL_Edge>, SDCEL_EdgeID>	Edges;
	std::unordered_map<TDCEL_VertID, std::unique_ptr<SDCEL_Vertex>>				Vertices;
	std::unordered_map<TDCEL_FaceID, std::unique_ptr<SDCEL_Face>>				Faces;
};


#endif // DCEL_H
