#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include "GeometryObject.h"

QT_BEGIN_NAMESPACE
namespace Ui { class MainWindow; }
QT_END_NAMESPACE

class MainWindow : public QMainWindow
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    ~MainWindow();

private slots:	
	void on_FileChooseBtn_clicked();

private:
    Ui::MainWindow *ui;

	CGeometryObject GeometryObject;
};
#endif // MAINWINDOW_H
