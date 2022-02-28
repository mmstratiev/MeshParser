#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

#include <QJsonDocument>
#include <QMessageBox>

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
	connect(&GeometryObject, &CGeometryObject::MadeProgress, this, &MainWindow::OnObjectMadeProgress);

	QSurfaceFormat format;
	format.setRenderableType(QSurfaceFormat::OpenGL);
	format.setProfile(QSurfaceFormat::CoreProfile);
	format.setVersion(3, 3);

	ui->OpenGLWidget->setFormat(format);
	ui->OpenGLWidget->SetObjectToDraw(&GeometryObject);
	ui->OpenGLWidget->SetShading(ui->SmoothShadingRadio->isChecked() ? EShading::Smooth : EShading::Flat);
	ui->OpenGLWidget->SetCamera(QVector3D(0, 0, 20.0f), QQuaternion());
	ui->ProgressBar->hide();

	this->RefreshLightDirecitonOpenGL();
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
	case CGeometryObject::EState::Subdividing:
	{
		ui->FileChooseBtn->setEnabled(false);
		ui->ExportBtn->setEnabled(false);
		ui->ViewNormalsBtn->setEnabled(false);
		ui->FitToObjectBtn->setEnabled(false);
		ui->SubdivideBtn->setEnabled(false);
		ui->CheckPointBtn->setEnabled(false);

		ui->ProgressBar->setFormat(this->GetProgressString());
		ui->ProgressBar->show();
		ui->TriCountLabel->setText(kCalcStr);
		ui->MinTriAreaLabel->setText(kCalcStr);
		ui->MaxTriAreaLabel->setText(kCalcStr);
		ui->AvgTriAreaLabel->setText(kCalcStr);
		ui->MeshType->setText(kCalcStr);
		break;
	}
	case CGeometryObject::EState::Idle:
	{
		ui->FileChooseBtn->setEnabled(true);
		ui->ExportBtn->setEnabled(true);
		ui->ViewNormalsBtn->setEnabled(true);
		ui->FitToObjectBtn->setEnabled(true);
		ui->SubdivideBtn->setEnabled(true);
		ui->CheckPointBtn->setEnabled(true);

		ui->ProgressBar->hide();
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

void MainWindow::OnObjectMadeProgress(int currentProgress, int maxProgress)
{
	ui->ProgressBar->setMaximum(maxProgress);
	ui->ProgressBar->setValue(currentProgress);
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
		std::vector<CTriangle> outTris;
		QVector3D dir(QRandomGenerator::global()->generateDouble(), QRandomGenerator::global()->generateDouble(), QRandomGenerator::global()->generateDouble());

		if(GeometryObject.RayTrace(ptToCheck, dir.normalized(), outTris))
		{
			if(outTris.size() % 2)
			{
				ptText = kPtInMeshClosed;
			}
		}
	}
	else
	{
		// Mesh is open, no way to define what "inside" means, do a simple bounds collison check
		if(GeometryObject.GetBoundingBox().IsPointInBox(ptToCheck))
		{
			ptText = kPtInMeshOpen;
		}
	}

	QMessageBox msgBox;
	msgBox.setText(ptText);
	msgBox.exec();
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


void MainWindow::on_LightX_valueChanged(double arg1)
{
	this->RefreshLigthDirectionSliders();
	this->RefreshLightDirecitonOpenGL();
}

void MainWindow::on_LightY_valueChanged(double arg1)
{
	this->RefreshLigthDirectionSliders();
	this->RefreshLightDirecitonOpenGL();
}

void MainWindow::on_LightZ_valueChanged(double arg1)
{
	this->RefreshLigthDirectionSliders();
	this->RefreshLightDirecitonOpenGL();
}

void MainWindow::RefreshLigthDirectionBoxes()
{
	ui->LightX->setValue((double)ui->LightSliderX->value() / 100.0f);
	ui->LightY->setValue((double)ui->LightSliderY->value() / 100.0f);
	ui->LightZ->setValue((double)ui->LightSliderZ->value() / 100.0f);
}

void MainWindow::RefreshLigthDirectionSliders()
{
	ui->LightSliderX->setValue(ui->LightX->value() * 100);
	ui->LightSliderY->setValue(ui->LightY->value() * 100);
	ui->LightSliderZ->setValue(ui->LightZ->value() * 100);
}

void MainWindow::RefreshLightDirecitonOpenGL()
{
	QVector3D dir(ui->LightX->value(), ui->LightY->value(), ui->LightZ->value());
	ui->OpenGLWidget->SetLightDirection(dir.normalized());
}

void MainWindow::on_LightSliderX_valueChanged(int value)
{
	this->RefreshLigthDirectionBoxes();
	this->RefreshLightDirecitonOpenGL();
}

void MainWindow::on_LightSliderY_valueChanged(int value)
{
	this->RefreshLigthDirectionBoxes();
	this->RefreshLightDirecitonOpenGL();
}

void MainWindow::on_LightSliderZ_valueChanged(int value)
{
	this->RefreshLigthDirectionBoxes();
	this->RefreshLightDirecitonOpenGL();
}

QString MainWindow::GetProgressString() const
{
	QString result = "Idle";
	switch (GeometryObject.GetState())
	{
	case CGeometryObject::EState::Initializing:
		result = "Initializing: %p%";
		break;
	case CGeometryObject::EState::Analyzing:
		result = "Analyzing: %p%";
		break;
	case CGeometryObject::EState::Subdividing:
		result = "Subdividing: %p%";
		break;
	case CGeometryObject::EState::Idle:
		break;
	}
	return result;
}


void MainWindow::on_FitToObjectBtn_clicked()
{
	if(!GeometryObject.IsInitialized()) return;
	const CBoundingBox& bounds = GeometryObject.GetBoundingBox();

	double x = bounds.GetCenter().x();
	double y = bounds.GetCenter().z();
	double z = bounds.GetMaxY() + (std::max(bounds.GetHeight(), bounds.GetWidth()) * 1.25f);

	ui->OpenGLWidget->SetCamera(QVector3D(x, y, z), QQuaternion());
}

