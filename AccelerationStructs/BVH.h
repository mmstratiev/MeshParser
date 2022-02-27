#ifndef BVH_H
#define BVH_H

#include "BoundingBox.h"
#include "BVH_Node.h"

// Bounding volume hierarchy - https://en.wikipedia.org/wiki/Bounding_volume_hierarchy
class CBVH
{
public:
	CBVH();
	void Init(CBoundingBox rootBounds, size_t depth);

private:
	std::unique_ptr<CBVH_Node> Root = nullptr;
};

#endif // BVH_H
