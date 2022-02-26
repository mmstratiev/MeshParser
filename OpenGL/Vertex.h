#ifndef VERTEX_H
#define VERTEX_H

#include <QVector3D>

class CVertex
{
public:
	CVertex();
	CVertex(const QVector3D &position);
	CVertex(const QVector3D &position, const QVector3D &flat, const QVector3D &smooth, const QVector3D& color = QVector3D(1.0f, 1.0f, 1.0f));

	const QVector3D& Position() const;
	const QVector3D& Color() const;
	const QVector3D& FlatNormal() const;
	const QVector3D& SmoothNormal() const;

	void SetPosition(const QVector3D& position);
	void SetColor(const QVector3D& color);
	void SetFlatNormal(const QVector3D& normal);
	void SetSmoothNormal(const QVector3D& normal);

	// OpenGL Helpers
	static const	int PositionTupleSize		= 3;
	static const	int ColorTupleSize			= 3;
	static const	int FlatNormalTupleSize		= 3;
	static const	int SmoothNormalTupleSize	= 3;

	static			int PositionOffset();
	static			int ColorOffset();
	static			int FlatNormalOffset();
	static			int SmoothNormalOffset();
	static			int Stride();

private:
	QVector3D mPosition;
	QVector3D mColor;
	QVector3D mFlatNormal;
	QVector3D mSmoothNormal;
};

#endif
