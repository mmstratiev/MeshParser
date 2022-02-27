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
