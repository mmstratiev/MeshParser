#ifndef MAINWINDOW_H
#define MAINWINDOW_H

#include <QMainWindow>
#include <QSharedDataPointer>

#include "GeometryObject/GeometryObject.h"
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

private slots:
	void OnObjectStateChanged(CGeometryObject::EState newState);
	void OnObjectMadeProgress(int currentProgress, int maxProgress);

	void on_FileChooseBtn_clicked();
	void on_ViewNormalsBtn_clicked();
	void on_ExportBtn_clicked();
	void on_SubdivideBtn_clicked();
	void on_CheckPointBtn_clicked();

	void on_SmoothShadingRadio_toggled(bool checked);
	void on_FlatShadingRadio_toggled(bool checked);

	void on_LightX_valueChanged(double arg1);
	void on_LightY_valueChanged(double arg1);
	void on_LightZ_valueChanged(double arg1);

	void on_LightSliderX_valueChanged(int value);
	void on_LightSliderY_valueChanged(int value);
	void on_LightSliderZ_valueChanged(int value);

	void on_FitToObjectBtn_clicked();

private:
	QString GetProgressString() const;

	void RefreshLigthDirectionBoxes();
	void RefreshLigthDirectionSliders();
	void RefreshLightDirecitonOpenGL();

private:
    Ui::MainWindow *ui;

	CDlgVertexNormals* DlgVertexNormals;

	CGeometryObject GeometryObject;
};
#endif // MAINWINDOW_H
