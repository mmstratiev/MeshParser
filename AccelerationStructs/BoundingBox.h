#ifndef CBOUNDINGBOX_H
#define CBOUNDINGBOX_H

#include <QVector3D>
#include "Math/Shapes.h"

class CBoundingBox
{
public:
	enum class EBoxSplitAxis
	{
		X, Y, Z
	};

	CBoundingBox();
	CBoundingBox(QVector3D bottomLeft, QVector3D topRight);

	const QVector3D &BottomLeft() const;
	const QVector3D &TopRight() const;

	double GetMinX() const;
	double GetMaxX() const;

	double GetMinY() const;
	double GetMaxY() const;

	double GetMinZ() const;
	double GetMaxZ() const;

	QVector3D GetCenter() const;

	bool IsPointInBox(const QVector3D& pt) const;
	bool Intersects(const STriangle& triangle) const;

	void ExtendTo(const QVector3D& extendPt);

	// Splits the bounds into two equal sized ones on the supplied axis
	std::vector<CBoundingBox> Split(EBoxSplitAxis splitAxis) const;

private:
	QVector3D mBottomLeft;
	QVector3D mTopRight;
};

#endif // CBOUNDINGBOX_H
