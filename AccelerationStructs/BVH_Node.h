#ifndef CBVH_NODE_H
#define CBVH_NODE_H

#include <unordered_set>
#include "DCEL/DCEL_Datatypes.h"
#include "BVH_Datatypes.h"
#include "BoundingBox.h"

class CBVH_Node
{
public:
	CBVH_Node();
	CBVH_Node(const CBoundingBox& data);

	TBVH_NodePtr Left() const;
	TBVH_NodePtr Right() const;
	const CBoundingBox& Data() const;

	void SetLeft(TBVH_NodePtr newLeft);
	void SetRight(TBVH_NodePtr newRight);
	void SetData(const CBoundingBox& newData);

	size_t GetLeafsCount() const;

	void AddLeaf(TDCEL_FacePtr leaf);

	// Splits the node into two nodes(Left and Right) with symmetrical bounding volumes
	void Split(CBoundingBox::EBoxSplitAxis splitAxis, size_t depth);

private:
	std::unique_ptr<CBVH_Node> mLeft	= nullptr;
	std::unique_ptr<CBVH_Node> mRight	= nullptr;

	CBoundingBox mData;

	std::unordered_set<TDCEL_FacePtr> Leafs;
};

#endif // CBVH_NODE_H
