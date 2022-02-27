#include "BVH.h"

CBVH::CBVH()
{
}

void CBVH::Init(CBoundingBox rootBounds, size_t depth)
{
	if(depth == 0) return;

	Root = std::make_unique<CBVH_Node>(rootBounds);
	Root->Split(CBoundingBox::EBoxSplitAxis::X, depth);
}
