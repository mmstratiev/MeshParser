#ifndef CAMERA3D_H
#define CAMERA3D_H

#include <QVector3D>
#include <QQuaternion>
#include <QMatrix4x4>

class CCamera3D
{
public:
	CCamera3D();

	//const
	static const QVector3D WorldForward;
	static const QVector3D WorldUp;
	static const QVector3D WorldRight;

	void Translate(const QVector3D &dt);
	void Translate(float dx, float dy, float dz);
	void Rotate(const QQuaternion &dr);
	void Rotate(float angle, const QVector3D &axis);
	void Rotate(float angle, float ax, float ay, float az);

	void SetTranslation(const QVector3D &t);
	void SetTranslation(float x, float y, float z);
	void SetRotation(const QQuaternion &r);
	void SetRotation(float angle, const QVector3D &axis);
	void SetRotation(float angle, float ax, float ay, float az);

	const QVector3D&	Translation() const;
	const QQuaternion&	Rotation() const;
	const QMatrix4x4&	ToMatrix();

	QVector3D Forward() const;
	QVector3D Right() const;
	QVector3D Up() const;

private:
	bool		bDirty = true;
	QVector3D	mTranslation;
	QQuaternion	mRotation;
	QMatrix4x4	mWorld;
};

#endif // CAMERA3D_H
