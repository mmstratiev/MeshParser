#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedDataPointer>

#include "GeometryObject.h"
#include "DlgVertexNormals.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

public slots:
	void OnObjectStateChanged(CGeometryObject::EState newState);

private slots:	
	void on_FileChooseBtn_clicked();
	void on_ViewNormalsBtn_clicked();

	void on_ExportBtn_clicked();

	void on_SubdivideBtn_clicked();

private:
    Ui::MainWindow *ui;

	CDlgVertexNormals* DlgVertexNormals;

	CGeometryObject GeometryObject;
};
#endif // MAINWINDOW_H
