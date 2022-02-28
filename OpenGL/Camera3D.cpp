#include "Camera3D.h"

const QVector3D CCamera3D::WorldForward(0.0f, 0.0f, -1.0f);
const QVector3D CCamera3D::WorldUp(0.0f, 1.0f, 0.0f);
const QVector3D CCamera3D::WorldRight(1.0f, 0.0f, 0.0f);

CCamera3D::CCamera3D() {}

void CCamera3D::Translate(const QVector3D &dt)
{
	bDirty = true;
	mTranslation += dt;
}

void CCamera3D::Translate(float dx, float dy, float dz)
{
	Translate(QVector3D(dx, dy, dz));
}

void CCamera3D::Rotate(const QQuaternion &dr)
{
	bDirty = true;
	mRotation = dr * mRotation;
}

void CCamera3D::Rotate(float angle, const QVector3D &axis)
{
	Rotate(QQuaternion::fromAxisAndAngle(axis, angle));
}

void CCamera3D::Rotate(float angle, float ax, float ay, float az)
{
	Rotate(QQuaternion::fromAxisAndAngle(ax, ay, az, angle));
}

void CCamera3D::SetTranslation(const QVector3D &t)
{
	bDirty = true;
	mTranslation = t;
}

void CCamera3D::SetTranslation(float x, float y, float z)
{
	SetTranslation(QVector3D(x, y, z));
}

void CCamera3D::SetRotation(const QQuaternion &r)
{
	 bDirty = true;
	 mRotation = r;
}

void CCamera3D::SetRotation(float angle, const QVector3D &axis)
{
	SetRotation(QQuaternion::fromAxisAndAngle(axis, angle));
}

void CCamera3D::SetRotation(float angle, float ax, float ay, float az)
{
	SetRotation(QQuaternion::fromAxisAndAngle(ax, ay, az, angle));
}

const QVector3D &CCamera3D::Translation() const
{
	return mTranslation;
}

const QQuaternion &CCamera3D::Rotation() const
{
	return mRotation;
}

const QMatrix4x4 &CCamera3D::ToMatrix()
{
	if (bDirty)
	{
		bDirty = false;
		mWorld.setToIdentity();
		mWorld.translate(mTranslation);
		mWorld.rotate(mRotation);
	}
	return mWorld;
}


QVector3D CCamera3D::Forward() const
{
	return mRotation.rotatedVector(WorldForward);
}

QVector3D CCamera3D::Right() const
{
	return mRotation.rotatedVector(WorldRight);
}

QVector3D CCamera3D::Up() const
{
	return mRotation.rotatedVector(WorldUp);
}

