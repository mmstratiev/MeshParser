#include "DlgVertexNormals.h"
#include "ui_DlgVertexNormals.h"
#include <QThreadPool>

CDlgVertexNormals::CDlgVertexNormals(QWidget *parent) :
	QDialog(parent),
	ui(new Ui::DlgVertexNormals)
{
	ui->setupUi(this);

	ui->normalsTable->setColumnCount(2);
	QStringList columns;
	columns << "Vertex" << "Normal";
	ui->normalsTable->setHorizontalHeaderLabels(columns);

	ui->normalsTable->horizontalHeader()->setSectionResizeMode(QHeaderView::Stretch);
	ui->normalsTable->setSizeAdjustPolicy(QAbstractScrollArea::SizeAdjustPolicy::AdjustToContents);
}

CDlgVertexNormals::~CDlgVertexNormals()
{
	delete ui;
}

void CDlgVertexNormals::SetGeometryObject(const CGeometryObject* geometryObject)
{
	GeometryObject = geometryObject;

	ui->progressBar->setMinimum(0);
	ui->progressBar->setValue(0);

	this->LoadVertices();
}

void CDlgVertexNormals::LoadVertices()
{
	connect(this, &CDlgVertexNormals::AddedVertex, this, &CDlgVertexNormals::OnAddedVertex);

	GeometryObject->WaitForIdle([&] (){
		qsizetype vertsCount = GeometryObject->GetVerticesCount();
		ui->progressBar->setMaximum(vertsCount-1);
		ui->normalsTable->setRowCount(vertsCount);

		// Separate thread to load the data
		QThreadPool::globalInstance()->start([&] () {
			qsizetype vertsCount = GeometryObject->GetVerticesCount();
			for(qsizetype vertIndex = 0; vertIndex < vertsCount; vertIndex++)
			{
				// Emit a signal for the main thread to handle filling in the data.
				// It appears Qt doesn't like it if we try to edit some UI stuff outside the GUI thread.
				// This makes the window stutter while filling, but the progress bar makes up for it.
				SVertex vertex;
				if(GeometryObject->GetVertex(vertIndex, vertex))
				{
					emit AddedVertex(vertIndex, vertex);
				}
			}
		});
	} );
}

void CDlgVertexNormals::OnAddedVertex(qsizetype index, SVertex data)
{
	QString vertexStr(QString::number(data.Location.x()) + ", " + QString::number(data.Location.y()) + ", " + QString::number(data.Location.z()));
	QString normalStr(QString::number(data.Normal.x()) + ", " + QString::number(data.Normal.y()) + ", " + QString::number(data.Normal.z()));

	ui->normalsTable->setItem(index, 0, new QTableWidgetItem(vertexStr));
	ui->normalsTable->setItem(index, 1, new QTableWidgetItem(normalStr));

	ui->progressBar->setValue(index);
	if(ui->progressBar->maximum() == index)
	{
		ui->progressBar->hide();
	}
}
