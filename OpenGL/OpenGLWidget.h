#ifndef COPENGLWIDGET_H
#define COPENGLWIDGET_H

#include <QtOpenGL>
#include <QtOpenGLWidgets/QOpenGLWidget>

#include "Camera3D.h"
#include "Vertex.h"
#include "InputManager.h"
#include "GeometryObject.h"

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
		void ResetCamera();

		void SetObjectToDraw(CGeometryObject *objectToDraw);
		void SetShading(EShading shading);

		void mousePressEvent(QMouseEvent *event) override;
		void mouseReleaseEvent(QMouseEvent *event) override;
		void keyPressEvent(QKeyEvent *event) override;
		void keyReleaseEvent(QKeyEvent *event) override;

	protected slots:
		void Update();

	private:
		EShading				Shading	= EShading::Flat;
		CGeometryObject			*objectToDraw;
		std::vector<CVertex>	*initializeVertex();

		QOpenGLBuffer				OpenGLBuffer;
		QOpenGLVertexArrayObject	OpenGLVertArray;
		QOpenGLShaderProgram*		OpenGLShader;

		int ModelToWorldCacheID;
		int WorldToViewCacheID;
		int ShadingCacheID;

		QMatrix4x4	Projection;
		CCamera3D	Camera;
	};

#endif // COPENGLWIDGET_H
