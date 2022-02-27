#include "BoundingBox.h"

CBoundingBox::CBoundingBox()
{

}

CBoundingBox::CBoundingBox(QVector3D bottomLeft, QVector3D topRight)
{
	this->ExtendTo(bottomLeft);
	this->ExtendTo(topRight);
}

const QVector3D &CBoundingBox::BottomLeft() const
{
	return mBottomLeft;
}

const QVector3D &CBoundingBox::TopRight() const
{
	return mTopRight;
}

double CBoundingBox::GetMinX() const
{
	return mBottomLeft.x();
}

double CBoundingBox::GetMaxX() const
{
	return mTopRight.x();
}

double CBoundingBox::GetMinY() const
{
	return mBottomLeft.y();
}

double CBoundingBox::GetMaxY() const
{
	return mTopRight.y();
}

double CBoundingBox::GetMinZ() const
{
	return mBottomLeft.z();
}

double CBoundingBox::GetMaxZ() const
{
	return mTopRight.z();
}

bool CBoundingBox::IsPointInBox(const QVector3D& pt) const
{
	return	pt.x() >= this->GetMinX() && pt.x() <= this->GetMaxX()
			&& pt.y() >= this->GetMinY() && pt.y() <= this->GetMaxY()
			&& pt.z() >= this->GetMinZ() && pt.z() <= this->GetMaxZ();
}

void CBoundingBox::ExtendTo(const QVector3D &extendPt)
{
	if(extendPt.x() < this->GetMinX())
	{
		mBottomLeft.setX(extendPt.x());
	}
	if(extendPt.x() > this->GetMaxX())
	{
		mTopRight.setX(extendPt.x());
	}

	if(extendPt.y() < this->GetMinY())
	{
		mBottomLeft.setY(extendPt.y());
	}
	if(extendPt.y() > this->GetMaxY())
	{
		mTopRight.setY(extendPt.y());
	}

	if(extendPt.z() < this->GetMinZ())
	{
		mBottomLeft.setZ(extendPt.z());
	}
	if(extendPt.z() > this->GetMaxZ())
	{
		mTopRight.setZ(extendPt.z());
	}
}

std::vector<CBoundingBox> CBoundingBox::Split(EBoxSplitAxis splitAxis) const
{
	std::vector<CBoundingBox> result;

	QVector3D box1TopRight	= mTopRight;
	QVector3D box2BotLeft	= mBottomLeft;

	switch(splitAxis)
	{
	case CBoundingBox::EBoxSplitAxis::X:
	{
		double newZ = this->GetMinZ() + this->GetMaxZ() / 2.0f;
		box1TopRight.setZ(newZ);
		box2BotLeft.setZ(newZ);
		break;
	}
	case CBoundingBox::EBoxSplitAxis::Y:
	{
		double newY = this->GetMinY() + this->GetMaxY() / 2.0f;
		box1TopRight.setY(newY);
		box2BotLeft.setY(newY);
		break;
	}
	case CBoundingBox::EBoxSplitAxis::Z:
	{
		double newX = this->GetMinX() + this->GetMaxX() / 2.0f;
		box1TopRight.setZ(newX);
		box2BotLeft.setZ(newX);
		break;
	}
	}

	result.push_back(CBoundingBox(mBottomLeft, box1TopRight));
	result.push_back(CBoundingBox(box2BotLeft, mTopRight));

	return result;

}
