#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

#include <QJsonDocument>

static const QString kCalcStr				= "Calculating...";
static const QString kMeshTypeClosedStr		= "Closed";
static const QString kMeshTypeOpenStr		= "Open";

static const QString kPtInMeshOpen			= "Point is in the object bounds.";
static const QString kPtInMeshClosed		= "Point is in the object.";
static const QString kPtNotInMesh			= "Point is not in the object.";


MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
	connect(&GeometryObject, &CGeometryObject::StateChanged, this, &MainWindow::OnObjectStateChanged);

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);

	ui->OpenGLWidget->setFormat(format);
	ui->OpenGLWidget->SetObjectToDraw(&GeometryObject);
	ui->OpenGLWidget->SetShading(ui->SmoothShadingRadio->isChecked() ? EShading::Smooth : EShading::Flat);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_FileChooseBtn_clicked()
{
	QString filter = "JSON File (*.json)";
	QString title = "Choose mesh file";

	QFileDialog meshFileDialog(this, title, "", filter);
	if(meshFileDialog.exec())
	{
		QString meshFilePath = meshFileDialog.selectedFiles().at(0);

		QFile meshFile(meshFilePath);
		QFileInfo fileInfo(meshFile);

		if (meshFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			ui->FileNameLabel->setText(fileInfo.fileName());

			QByteArray meshJsonStr = meshFile.readAll();
			QJsonDocument meshJson = QJsonDocument::fromJson(meshJsonStr);

			GeometryObject.Init(meshJson.toJson());
		}
	}
}


void MainWindow::on_ViewNormalsBtn_clicked()
{
	if(!GeometryObject.IsInitialized()) return;

	DlgVertexNormals = new CDlgVertexNormals(this);
	DlgVertexNormals->SetGeometryObject(&GeometryObject);
	DlgVertexNormals->setModal(true);
	DlgVertexNormals->exec();
}

void MainWindow::OnObjectStateChanged(CGeometryObject::EState newState)
{
	switch (newState)
	{
		case CGeometryObject::EState::Initializing:
		case CGeometryObject::EState::Analyzing:
		{
			ui->TriCountLabel->setText(kCalcStr);
			ui->MinTriAreaLabel->setText(kCalcStr);
			ui->MaxTriAreaLabel->setText(kCalcStr);
			ui->AvgTriAreaLabel->setText(kCalcStr);
			ui->MeshType->setText(kCalcStr);
			break;
		}
		case CGeometryObject::EState::Idle:
		{
			ui->TriCountLabel->setText(QString::number(GeometryObject.GetTrianglesCount()));
			ui->MinTriAreaLabel->setText(QString::number(GeometryObject.GetMinTriangleArea()));
			ui->MaxTriAreaLabel->setText(QString::number(GeometryObject.GetMaxTriangleArea()));
			ui->AvgTriAreaLabel->setText(QString::number(GeometryObject.GetTotalArea() / GeometryObject.GetTrianglesCount()));
			ui->MeshType->setText(GeometryObject.IsClosed() ? kMeshTypeClosedStr : kMeshTypeOpenStr);
			ui->OpenGLWidget->Refresh();
			break;
		}
	}
}

void MainWindow::on_ExportBtn_clicked()
{
	if(!GeometryObject.IsInitialized()) return;

	QString filter = "JSON File (*.json)";
	QString title = "Choose mesh file";

	QFileDialog meshFileDialog(this, title, "", filter);
	QString fileName = QFileDialog::getSaveFileName(this, title, ui->FileNameLabel->text(), filter);

	QFile meshFile(fileName);
	if (meshFile.open(QIODevice::WriteOnly | QIODevice::Truncate))
	{
		QJsonArray vertsJson;
		qsizetype vertsCount = GeometryObject.GetVerticesCount();
		for(qsizetype vertID = 0; vertID < vertsCount; vertID++)
		{
			SVertex vert;
			if(GeometryObject.GetVertex(vertID, vert))
			{
				vertsJson.append(vert.Location.x());
				vertsJson.append(vert.Location.y());
				vertsJson.append(vert.Location.z());
			}
		}

		QJsonArray trianglesJson;
		qsizetype trianglesCount = GeometryObject.GetTrianglesCount();
		for(qsizetype triangleID = 0; triangleID < trianglesCount; triangleID++)
		{
			CTriangle triangle;
			if(GeometryObject.GetTriangle(triangleID, triangle))
			{
				int vert1ID = GeometryObject.GetTriangleVertID(triangleID, 0);
				trianglesJson.append(vert1ID);

				int vert2ID = GeometryObject.GetTriangleVertID(triangleID, 1);
				trianglesJson.append(vert2ID);

				int vert3ID = GeometryObject.GetTriangleVertID(triangleID, 2);
				trianglesJson.append(vert3ID);
			}
		}

		QJsonObject geometricObjJson;
		geometricObjJson.insert("vertices", vertsJson);
		geometricObjJson.insert("triangles", trianglesJson);

		QJsonObject rootJson;
		rootJson.insert("geometry_object", geometricObjJson);

		QJsonDocument document(rootJson);
		meshFile.write(document.toJson(QJsonDocument::Compact));
	}
}


void MainWindow::on_SubdivideBtn_clicked()
{
	if(!GeometryObject.IsInitialized()) return;

	GeometryObject.Subdivide((ESubdivisionAlgorithm)ui->SubdivideMethodCombo->currentIndex());
}


void MainWindow::on_CheckPointBtn_clicked()
{
	if(!GeometryObject.IsInitialized()) return;

	QVector3D	ptToCheck(ui->PtXBox->value(), ui->PtYBox->value(), ui->PtZBox->value());
	QString		ptText = kPtNotInMesh;

	if(GeometryObject.IsClosed())
	{
		// Mesh is closed, we can easily define what "inside" means, do a complex ray trace
		ptText = kPtInMeshClosed;
	}
	else
	{
		// Mesh is open, no way to define what "inside" means, do a simple bounds collison check
		if(GeometryObject.GetBoundingBox().IsPointInBox(ptToCheck))
		{
			ptText = kPtInMeshOpen;
		}
	}
	ui->PointInMeshLabel->setText(ptText);
}

void MainWindow::on_SmoothShadingRadio_toggled(bool checked)
{
	if(!checked) return;
	ui->OpenGLWidget->SetShading(EShading::Smooth);
}


void MainWindow::on_FlatShadingRadio_toggled(bool checked)
{
	if(!checked) return;
	ui->OpenGLWidget->SetShading(EShading::Flat);
}

