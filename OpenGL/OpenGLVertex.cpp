#include "OpenGL/OpenGLVertex.h"

COpenGLVertex::COpenGLVertex()
{}

COpenGLVertex::COpenGLVertex(const QVector3D &position)
	: mPosition(position)
	, mColor(1.0f, 1.0f, 1.0f)
{}

COpenGLVertex::COpenGLVertex(const QVector3D &position, const QVector3D &flat, const QVector3D &smooth, const QVector3D& color)
	: mPosition(position)
	, mColor(color)
	, mFlatNormal(flat)
	, mSmoothNormal(smooth)
{}

const QVector3D &COpenGLVertex::Position() const
{
	return mPosition;
}

const QVector3D &COpenGLVertex::Color() const
{
	return mColor;
}

const QVector3D &COpenGLVertex::FlatNormal() const
{
	return mFlatNormal;
}

const QVector3D &COpenGLVertex::SmoothNormal() const
{
	return mSmoothNormal;
}

void COpenGLVertex::SetPosition(const QVector3D &position)
{
	mPosition = position;
}

void COpenGLVertex::SetColor(const QVector3D &color)
{
	mColor = color;
}

void COpenGLVertex::SetFlatNormal(const QVector3D &normal)
{
	mFlatNormal = normal;
}

void COpenGLVertex::SetSmoothNormal(const QVector3D &normal)
{
	mSmoothNormal = normal;
}

int COpenGLVertex::PositionOffset()
{
	return offsetof(COpenGLVertex, mPosition);
}

int COpenGLVertex::ColorOffset()
{
	return offsetof(COpenGLVertex, mColor);
}

int COpenGLVertex::FlatNormalOffset()
{
	return offsetof(COpenGLVertex, mFlatNormal);
}

int COpenGLVertex::SmoothNormalOffset()
{
	return offsetof(COpenGLVertex, mSmoothNormal);
}

int COpenGLVertex::Stride()
{
	return sizeof(COpenGLVertex);
}
