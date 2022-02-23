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

			GeometryObject.Init(meshJson.toJson(), [&] (){
				SMeshStats meshStats = GeometryObject.GetStats();
				ui->MinTriAreaLabel->setText(QString::number(meshStats.MinTriangleArea));
				ui->MaxTriAreaLabel->setText(QString::number(meshStats.MaxTriangleArea));
				ui->AvgTriAreaLabel->setText(QString::number(meshStats.GetAvgTriangleArea()));
			});

			GeometryObject.Wait([&] (){
				qsizetype vertCount = GeometryObject.GetVerticesCount();
				for(qsizetype vertIndex = 0; vertIndex < vertCount; vertIndex++)
				{
					SVertex vertex = GeometryObject.GetVertex(vertIndex);
					qInfo() << "Vertex: " << vertex.Location << "; Normal: " << vertex.Normal;
				}
			});

			ui->FileNameLabel->setText(fileInfo.fileName());
		}
	}
}


void MainWindow::on_ViewNormalsBtn_clicked()
{

}

