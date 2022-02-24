#ifndef DLGVERTEXNORMALS_H
#define DLGVERTEXNORMALS_H

#include <QDialog>
#include <QTableWidget>

#include "GeometryObject.h"

namespace Ui {
	class DlgVertexNormals;
}

class CDlgVertexNormals : public QDialog
{
	Q_OBJECT

public:
	explicit CDlgVertexNormals(QWidget *parent = nullptr);
	~CDlgVertexNormals();

	void SetGeometryObject(const CGeometryObject* geometryObject);

private:
	void LoadVertices();

signals:
	void AddedVertex(qsizetype index, SVertex data);

private slots:
	void OnAddedVertex(qsizetype index, SVertex data);

private:
	Ui::DlgVertexNormals*	ui;
	const CGeometryObject*	GeometryObject;
};

#endif // DLGVERTEXNORMALS_H
