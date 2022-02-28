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

double CBoundingBox::GetHeight() const
{
	return std::abs(this->GetMinZ() - this->GetMaxZ());
}

double CBoundingBox::GetWidth() const
{
	return std::abs(this->GetMinX() - this->GetMaxX());
}

bool CBoundingBox::IsPointInBox(const QVector3D& pt) const
{
	return	pt.x() >= this->GetMinX() && pt.x() <= this->GetMaxX()
			&& pt.y() >= this->GetMinY() && pt.y() <= this->GetMaxY()
			&& pt.z() >= this->GetMinZ() && pt.z() <= this->GetMaxZ();
}

// https://www.scratchapixel.com/lessons/3d-basic-rendering/minimal-ray-tracer-rendering-simple-shapes/ray-box-intersection
bool CBoundingBox::Intersects(const QVector3D &origin, const QVector3D &dir) const
{
	float tmin = (this->GetMinX() - origin.x()) / dir.x();
	float tmax = (this->GetMaxX() - origin.x()) / dir.x();

	if (tmin > tmax) std::swap(tmin, tmax);

	float tymin = (this->GetMinY() - origin.y()) / dir.y();
	float tymax = (this->GetMaxY() - origin.y()) / dir.y();

	if (tymin > tymax) std::swap(tymin, tymax);

	if ((tmin > tymax) || (tymin > tmax))
		 return false;

	if (tymin > tmin)
		 tmin = tymin;

	if (tymax < tmax)
		 tmax = tymax;

	float tzmin = (this->GetMinZ() - origin.z()) / dir.z();
	float tzmax = (this->GetMaxZ() - origin.z()) / dir.z();

	if (tzmin > tzmax) std::swap(tzmin, tzmax);

	if ((tmin > tzmax) || (tzmin > tmax))
		 return false;

	if (tzmin > tmin)
		 tmin = tzmin;

	if (tzmax < tmax)
		 tmax = tzmax;

	return true;
}

bool CBoundingBox::Intersects(const CTriangle &triangle) const
{
	CBoundingBox triangleBounds;
	triangleBounds.ExtendTo(triangle.Vertices(0));
	triangleBounds.ExtendTo(triangle.Vertices(1));
	triangleBounds.ExtendTo(triangle.Vertices(2));

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
