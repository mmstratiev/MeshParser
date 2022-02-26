#include "Vertex.h"

CVertex::CVertex()
{}

CVertex::CVertex(const QVector3D &position)
	: mPosition(position)
	, mColor(1.0f, 1.0f, 1.0f)
{}

CVertex::CVertex(const QVector3D &position, const QVector3D &flat, const QVector3D &smooth, const QVector3D& color)
	: mPosition(position)
	, mColor(color)
	, mFlatNormal(flat)
	, mSmoothNormal(smooth)
{}

const QVector3D &CVertex::Position() const
{
	return mPosition;
}

const QVector3D &CVertex::Color() const
{
	return mColor;
}

const QVector3D &CVertex::FlatNormal() const
{
	return mFlatNormal;
}

const QVector3D &CVertex::SmoothNormal() const
{
	return mSmoothNormal;
}

void CVertex::SetPosition(const QVector3D &position)
{
	mPosition = position;
}

void CVertex::SetColor(const QVector3D &color)
{
	mColor = color;
}

void CVertex::SetFlatNormal(const QVector3D &normal)
{
	mFlatNormal = normal;
}

void CVertex::SetSmoothNormal(const QVector3D &normal)
{
	mSmoothNormal = normal;
}

int CVertex::PositionOffset()
{
	return offsetof(CVertex, mPosition);
}

int CVertex::ColorOffset()
{
	return offsetof(CVertex, mColor);
}

int CVertex::FlatNormalOffset()
{
	return offsetof(CVertex, mFlatNormal);
}

int CVertex::SmoothNormalOffset()
{
	return offsetof(CVertex, mSmoothNormal);
}

int CVertex::Stride()
{
	return sizeof(CVertex);
}
