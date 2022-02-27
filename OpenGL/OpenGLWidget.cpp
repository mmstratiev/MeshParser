#include <QOpenGLFunctions>
#include <QOpenGLShaderProgram>
#include <QOpenGLContext>
#include <QOpenGLVertexArrayObject>

#include "OpenGLWidget.h"

COpenGLWidget::COpenGLWidget(QWidget *parent)
	: QOpenGLWidget(parent)
{
	setFocusPolicy(Qt::StrongFocus);
	this->ResetCamera();
}

COpenGLWidget::~COpenGLWidget()
{
	this->TeardownGL();
}

void COpenGLWidget::initializeGL()
{
	// Builds the vertexes of the ObjectToDraw based on it's given parameters
	objectToDraw->BuildOpenGLVertexes();

	initializeOpenGLFunctions();

	// Starts the update "loop"
	connect(this, SIGNAL(frameSwapped()), this, SLOT(Update()));

	// OpenGL
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	{
		// Setting up the OpenGLShaderProgram
		OpenGLShader = new QOpenGLShaderProgram(this);
		OpenGLShader->addShaderFromSourceFile(QOpenGLShader::Vertex, ":Shaders/simple.vert");
		OpenGLShader->addShaderFromSourceFile(QOpenGLShader::Fragment, ":Shaders/simple.frag");

		OpenGLShader->link();
		OpenGLShader->bind();

		// Create cache for Uniform Locations
		ModelToWorldCacheID = OpenGLShader->uniformLocation("ModelToWorld");
		WorldToViewCacheID = OpenGLShader->uniformLocation("WorldToView");
		ShadingCacheID = OpenGLShader->uniformLocation("Shading");

		if(objectToDraw->IsInitialized())
		{
			// Setting up the OpenGL buffer(VBO)
			OpenGLBuffer.create();
			OpenGLBuffer.bind();
			OpenGLBuffer.setUsagePattern(QOpenGLBuffer::StaticDraw);

			//Allocates for the vertexes array
			const CVertex *sg_vertexes = objectToDraw->GetOpenGLVerticesBegin();
			OpenGLBuffer.allocate(sg_vertexes, sizeof(CVertex)*objectToDraw->GetOpenGLVerticesCount());
		}

		// Setting up OpenGL VertexArrayObject
		OpenGLVertArray.create();
		OpenGLVertArray.bind();
		OpenGLShader->enableAttributeArray(0);
		OpenGLShader->enableAttributeArray(1);
		OpenGLShader->enableAttributeArray(2);
		OpenGLShader->enableAttributeArray(3);
		OpenGLShader->setAttributeBuffer(0, GL_FLOAT, CVertex::PositionOffset(), CVertex::PositionTupleSize, CVertex::Stride());
		OpenGLShader->setAttributeBuffer(1, GL_FLOAT, CVertex::ColorOffset(), CVertex::ColorTupleSize, CVertex::Stride());
		OpenGLShader->setAttributeBuffer(2, GL_FLOAT, CVertex::FlatNormalOffset(), CVertex::FlatNormalTupleSize, CVertex::Stride());
		OpenGLShader->setAttributeBuffer(3, GL_FLOAT, CVertex::SmoothNormalOffset(), CVertex::SmoothNormalTupleSize, CVertex::Stride());

		// Releasing
		OpenGLVertArray.release();
		OpenGLBuffer.release();
		OpenGLShader->release();
	}
}

void COpenGLWidget::resizeGL(int width, int height)
{
	Projection.setToIdentity();
	Projection.perspective(45.0f, width / float(height), 0.01f, 1000.0f);
}

void COpenGLWidget::paintGL()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Sets view matrix
	OpenGLShader->bind();
	OpenGLShader->setUniformValue(WorldToViewCacheID, Projection);
	OpenGLShader->setUniformValue(ShadingCacheID, (int)Shading);
	{
		OpenGLVertArray.bind();
		OpenGLShader->setUniformValue(ModelToWorldCacheID, Camera.ToMatrix().inverted(NULL));

		if(objectToDraw->IsInitialized())
		{
			// Draws the object
			glDrawArrays(GL_TRIANGLES, 0, objectToDraw->GetOpenGLVerticesCount());
		}
		OpenGLVertArray.release();
	}
	OpenGLShader->release();
}

void COpenGLWidget::TeardownGL()
{
	makeCurrent();
	// Stops the update() "loop"
	disconnect(this, SIGNAL(frameSwapped()), this, SLOT(Update()));

	OpenGLVertArray.destroy();
	OpenGLBuffer.destroy();
	delete OpenGLShader;
}

void COpenGLWidget::Refresh()
{
	// Reinitializes the OpenGL window, updating the drawed object with new values
	this->TeardownGL();
	this->initializeGL();
}

void COpenGLWidget::ResetCamera()
{
	Camera.SetTranslation(0.0f, 0.0f, 15.0f);
	Camera.SetRotation(0.0f, 0.0f, 0.0f, 0.0f);
}

void COpenGLWidget::SetObjectToDraw(CGeometryObject *objectToDraw)
{
	this->objectToDraw = objectToDraw;
}

void COpenGLWidget::SetShading(EShading shading)
{
	Shading = shading;
}

void COpenGLWidget::mousePressEvent(QMouseEvent *event)
{
	CInputManager::SetButtonPressed(event->button(), true);
}

void COpenGLWidget::mouseReleaseEvent(QMouseEvent *event)
{
	CInputManager::SetButtonPressed(event->button(), false);
}

void COpenGLWidget::keyPressEvent(QKeyEvent *event)
{
	CInputManager::SetKeyPressed((Qt::Key)event->key(), true);
}

void COpenGLWidget::keyReleaseEvent(QKeyEvent *event)
{
	CInputManager::SetKeyPressed((Qt::Key)event->key(), false);
}

void COpenGLWidget::Update()
{
	CInputManager::UpdateMousePosition();

	// Speed of camera movement and rotation
	const float transSpeed = 0.5f;
	const float rotSpeed   = 0.5f;

	// Right mouse button enables camera rotation
	if(CInputManager::IsButtonPressed(Qt::RightButton))
	{
		Camera.Rotate(-rotSpeed*CInputManager::GetMouseDelta().x(), CCamera3D::WorldUp);
		Camera.Rotate(-rotSpeed*CInputManager::GetMouseDelta().y(), Camera.Right());
	}

	// Query keys to determine mouse movement direction
	QVector3D translation;
	if(CInputManager::IsKeyPressed(Qt::Key_W))
	{
		translation+= Camera.Forward();
	}
	if(CInputManager::IsKeyPressed(Qt::Key_S))
	{
		translation-= Camera.Forward();
	}
	if(CInputManager::IsKeyPressed(Qt::Key_A))
	{
		translation -= Camera.Right();
	}
	if(CInputManager::IsKeyPressed(Qt::Key_D))
	{
		translation += Camera.Right();
	}
	if(CInputManager::IsKeyPressed(Qt::Key_E))
	{
		translation += Camera.Up();
	}
	if(CInputManager::IsKeyPressed(Qt::Key_Q))
	{
		translation -= Camera.Up();
	}

	Camera.Translate(transSpeed*translation);

	// Schedule a redraw
	QOpenGLWidget::update();
}
