#include "BVH.h"
#include "DCEL/DCEL_Face.h"

CBVH::CBVH()
{
}

void CBVH::Init(CBoundingBox rootBounds, size_t depth)
{
	if(depth == 0) return;

	Root = std::make_unique<CBVH_Node>(rootBounds);
	Root->Split(CBoundingBox::EBoxSplitAxis::X, depth);
}

void CBVH::AddLeaf(TDCEL_FacePtr leaf)
{
	if(!Root) return;

	Root->AddLeaf(leaf);
}

bool CBVH::RayTrace(QVector3D origin, QVector3D dir) const
{
	TDCEL_FacePtr	result = nullptr;
	TBVH_NodePtr	root = Root.get();

	if(!root) return result;

	TLeavesSet leaves;
	this->SearchForLeavesRecursive(root, leaves, [&] (TBVH_NodePtr currentNode) -> bool
	{
		return currentNode->Data().Intersects(origin, dir);
	});

	for(TDCEL_FacePtr leaf : leaves)
	{
		CTriangle triangle = leaf->Get();
		if(triangle.Intersects(origin, dir))
		{

		}
	}

	return result;
}

void CBVH::SearchForLeavesRecursive(TBVH_NodePtr currentNode,TLeavesSet& outLeaves, std::function<bool(TBVH_NodePtr)> callback) const
{
	if(currentNode->Left())
	{
		if(callback(currentNode->Left()))
		{
			SearchForLeavesRecursive(currentNode->Left(), outLeaves, callback);
		}
	}
	else
	{
		const TLeavesSet& currentLeaves = currentNode->Leaves();
		outLeaves.insert(currentLeaves.begin(), currentLeaves.end());
	}

	if(currentNode->Right())
	{
		if(callback(currentNode->Right()))
		{
			SearchForLeavesRecursive(currentNode->Right(), outLeaves, callback);
		}
	}
	else
	{
		const TLeavesSet& currentLeaves = currentNode->Leaves();
		outLeaves.insert(currentLeaves.begin(), currentLeaves.end());
	}
}
