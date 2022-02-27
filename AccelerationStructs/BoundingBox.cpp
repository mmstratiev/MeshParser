#include "BoundingBox.h"
#include <QDebug>

CBoundingBox::CBoundingBox()
{

}

CBoundingBox::CBoundingBox(QVector3D bottomLeft, QVector3D topRight)
	: mBottomLeft(bottomLeft)
	, mTopRight(topRight)
{
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

QVector3D CBoundingBox::GetCenter() const
{
	return QVector3D ( (this->GetMinX() + this->GetMaxX()) / 2.0f
					 , (this->GetMinY() + this->GetMaxY()) / 2.0f
					 , (this->GetMinZ() + this->GetMaxZ()) / 2.0f);
}

bool CBoundingBox::IsPointInBox(const QVector3D& pt) const
{
	return	pt.x() >= this->GetMinX() && pt.x() <= this->GetMaxX()
			&& pt.y() >= this->GetMinY() && pt.y() <= this->GetMaxY()
			&& pt.z() >= this->GetMinZ() && pt.z() <= this->GetMaxZ();
}

bool CBoundingBox::Intersects(const STriangle &triangle) const
{
	CBoundingBox triangleBounds;
	triangleBounds.ExtendTo(triangle.Vertices[0]);
	triangleBounds.ExtendTo(triangle.Vertices[1]);
	triangleBounds.ExtendTo(triangle.Vertices[2]);

	return	this->GetMaxX() > triangleBounds.GetMinX()
			&& this->GetMinX() < triangleBounds.GetMaxX()

			&& this->GetMaxY() > triangleBounds.GetMinY()
			&& this->GetMinY() < triangleBounds.GetMaxY()

			&& this->GetMaxZ() > triangleBounds.GetMinZ()
			&& this->GetMinZ() < triangleBounds.GetMaxZ();
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
	QVector3D center		= this->GetCenter();

	switch(splitAxis)
	{
	case CBoundingBox::EBoxSplitAxis::X:
	{
		box1TopRight.setZ(center.z());
		box2BotLeft.setZ(center.z());
		break;
	}
	case CBoundingBox::EBoxSplitAxis::Y:
	{
		box1TopRight.setY(center.y());
		box2BotLeft.setY(center.y());
		break;
	}
	case CBoundingBox::EBoxSplitAxis::Z:
	{
		box1TopRight.setX(center.x());
		box2BotLeft.setX(center.x());
		break;
	}
	}

	result.push_back(CBoundingBox(mBottomLeft, box1TopRight));
	result.push_back(CBoundingBox(box2BotLeft, mTopRight));

	return result;

}
