#include "BVH_Node.h"
#include "DCEL/DCEL_Face.h"
#include <QDebug>

CBVH_Node::CBVH_Node()
{}

CBVH_Node::CBVH_Node(const CBoundingBox &data)
	: mData(data)
{}

TBVH_NodePtr CBVH_Node::Left() const
{
	return mLeft.get();
}

TBVH_NodePtr CBVH_Node::Right() const
{
	return mRight.get();
}

const CBoundingBox &CBVH_Node::Data() const
{
	return mData;
}

void CBVH_Node::SetLeft(TBVH_NodePtr newLeft)
{
	mLeft.reset(newLeft);
}

void CBVH_Node::SetRight(TBVH_NodePtr newRight)
{
	mRight.reset(newRight);
}

void CBVH_Node::SetData(const CBoundingBox &newData)
{
	mData = newData;
}

void CBVH_Node::AddLeaf(TDCEL_FacePtr leaf)
{
	STriangle triangle = leaf->Get();
	if(mLeft && mRight)
	{
		if(mLeft->Data().Intersects(triangle))
		{
			mLeft->AddLeaf(leaf);
		}
		if(mRight->Data().Intersects(triangle))
		{
			mRight->AddLeaf(leaf);
		}
	}
	else
	{
		Leafs.insert(leaf);
	}
}

void CBVH_Node::Split(CBoundingBox::EBoxSplitAxis splitAxis, size_t depth)
{
	if(depth == 0) return;

	std::vector<CBoundingBox> splitBounds = mData.Split(splitAxis);

	depth -= 1;
	switch(splitAxis)
	{
	case CBoundingBox::EBoxSplitAxis::X:
		splitAxis = CBoundingBox::EBoxSplitAxis::Y;
		break;
	case CBoundingBox::EBoxSplitAxis::Y:
		splitAxis = CBoundingBox::EBoxSplitAxis::Z;
		break;
	case CBoundingBox::EBoxSplitAxis::Z:
		splitAxis = CBoundingBox::EBoxSplitAxis::X;
		break;
	}

	mLeft = std::make_unique<CBVH_Node>(splitBounds[0]);
	mLeft->Split(splitAxis, depth);

	mRight = std::make_unique<CBVH_Node>(splitBounds[1]);
	mRight->Split(splitAxis, depth);
}
