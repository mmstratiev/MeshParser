#ifndef COPENGLWIDGET_H
#define COPENGLWIDGET_H

#include <QtOpenGL>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "Camera3D.h"
#include "OpenGLVertex.h"
#include "InputManager.h"
#include "GeometryObject/GeometryObject.h"

enum class EShading
{
	Flat,
	Smooth
};

class COpenGLWidget : public QOpenGLWidget, protected QOpenGLFunctions
{
	Q_OBJECT
	public:
		explicit COpenGLWidget(QWidget *parent = nullptr);
		~COpenGLWidget();

		//OpenGL methods
		void initializeGL() override;
		void resizeGL(int width, int height) override;
		void paintGL() override;

		void TeardownGL();
		void Refresh();

		void SetCamera(const QVector3D& translation, const QQuaternion& rotation);
		void SetObjectToDraw(CGeometryObject *objectToDraw);
		void SetShading(EShading shading);
		void SetLightDirection(const QVector3D& dir);

		void mousePressEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *event) override;
		void keyPressEvent(QKeyEvent *event) override;
		void keyReleaseEvent(QKeyEvent *event) override;

	protected slots:
		void Update();

	private:
		EShading				Shading	= EShading::Flat;
		CGeometryObject			*objectToDraw;
		std::vector<COpenGLVertex>	*initializeVertex();

		QOpenGLBuffer				OpenGLBuffer;
		QOpenGLVertexArrayObject	OpenGLVertArray;
		QOpenGLShaderProgram*		OpenGLShader;

		int ModelToWorldCacheID;
		int WorldToViewCacheID;
		int ShadingCacheID;
		int LightDirCacheID;

		QMatrix4x4	Projection;
		CCamera3D	Camera;
		QVector3D	LightDir = QVector3D(0, 0, -1);
	};

#endif // COPENGLWIDGET_H
