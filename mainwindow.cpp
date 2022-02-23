#include "mainwindow.h"
#include "ui_mainwindow.h"

#include <QDebug>
#include <QFileDialog>

#include <QJsonDocument>

MainWindow::MainWindow(QWidget *parent)
    : QMainWindow(parent)
    , ui(new Ui::MainWindow)
{
    ui->setupUi(this);
}

MainWindow::~MainWindow()
{
    delete ui;
}

void MainWindow::on_FileChooseBtn_clicked()
{
	QString filter = "JSON File (*.json)";
	QString title = "Choose a mesh file";

	QFileDialog meshFileDialog(this, title, "", filter);
	if(meshFileDialog.exec())
	{
		QString meshFilePath = meshFileDialog.selectedFiles().at(0);

		QFile meshFile(meshFilePath);
		QFileInfo fileInfo(meshFile);

		if (meshFile.open(QIODevice::ReadOnly | QIODevice::Text))
		{
			QByteArray meshJsonStr = meshFile.readAll();
			QJsonDocument meshJson = QJsonDocument::fromJson(meshJsonStr);

			ui->MinTriAreaLabel->setText("Calculating...");
			ui->MaxTriAreaLabel->setText("Calculating...");
			ui->AvgTriAreaLabel->setText("Calculating...");
			ui->MeshType->setText("Calculating...");

			GeometryObject.Init(meshJson.toJson(), [&] (){
				SMeshStats meshStats = GeometryObject.GetStats();
				ui->MinTriAreaLabel->setText(QString::number(meshStats.MinTriangleArea));
				ui->MaxTriAreaLabel->setText(QString::number(meshStats.MaxTriangleArea));
				ui->AvgTriAreaLabel->setText(QString::number(meshStats.TotalArea / GeometryObject.GetTrianglesCount()));
				ui->MeshType->setText(meshStats.bIsClosed ? "Closed" : "Open");
			});

			ui->FileNameLabel->setText(fileInfo.fileName());
		}
	}
}


void MainWindow::on_ViewNormalsBtn_clicked()
{
	DlgVertexNormals = new CDlgVertexNormals(this);
	DlgVertexNormals->SetGeometryObject(&GeometryObject);
	DlgVertexNormals->setModal(true);
	DlgVertexNormals->exec();
}

