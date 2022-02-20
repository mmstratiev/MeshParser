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

			GeometryObject.Init(meshJson.toJson());
			GeometryObject.GetStats([&](const SMeshStats& result) {
				ui->MinTriAreaLabel->setText(QString::number(result.MinTriangleArea));
				ui->MaxTriAreaLabel->setText(QString::number(result.MaxTriangleArea));
				ui->AvgTriAreaLabel->setText(QString::number(result.GetAvgTriangleArea()));
			} );

			ui->FileNameLabel->setText(fileInfo.fileName());
		}
	}
}

