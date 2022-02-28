#ifndef MESHREADER_H
#define MESHREADER_H

#include <QObject>
#include <QRunnable>

#include "GeometryObject.h"

class CMeshReader : public QObject, public QRunnable
{
    Q_OBJECT
public:
	explicit CMeshReader(CGeometryObject& inOutObject, const QJsonObject& jsonDataObject, qsizetype beginIndex, qsizetype endIndex, QObject *parent = nullptr);
	~CMeshReader();

signals:
	void Finished();

	// QRunnable interface
public:
	void run() Q_DECL_OVERRIDE;

	static QJsonObject	GetRawData(const QJsonObject& jsonDataObject);
	static QJsonObject	GetRawObject(const QJsonObject& jsonDataObject);

	static QJsonArray	GetVerticesRaw(const QJsonObject& jsonDataObject);
	static QJsonArray	GetTrianglesRaw(const QJsonObject& jsonDataObject);

	static qsizetype	GetVerticesCountRaw(const QJsonObject& jsonDataObject);
	static QVector3D	GetVertexRaw(const QJsonObject& jsonDataObject, qsizetype vertexIndex);

	static qsizetype	GetTrianglesCountRaw(const QJsonObject& jsonDataObject);
	static CTriangle	GetTriangleRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex);
	static qsizetype	GetTriangleVertexIndexRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex, qsizetype vertexNum);

private:
	void Work();

private:
	CGeometryObject&		GeometryObject;
	QJsonObject				DataObject;
	qsizetype				BeginIndex	= 0;
	qsizetype				EndIndex	= 0;
};

#endif // MESHREADER_H
