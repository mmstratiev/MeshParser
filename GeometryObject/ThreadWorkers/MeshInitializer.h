#ifndef MESHINITIALIZER_H
#define MESHINITIALIZER_H

#include "ThreadWorker.h"

class CMeshInitializer : public CThreadWorker
{
    Q_OBJECT
public:
	explicit CMeshInitializer(CGeometryObject& inOutObject, const QJsonObject& jsonDataObject, QObject *parent = nullptr);

public:
	static QJsonObject	GetRawData(const QJsonObject& jsonDataObject);
	static QJsonObject	GetRawObject(const QJsonObject& jsonDataObject);

	static QJsonArray	GetVerticesRaw(const QJsonObject& jsonDataObject);
	static QJsonArray	GetTrianglesRaw(const QJsonObject& jsonDataObject);

	static qsizetype	GetVerticesCountRaw(const QJsonObject& jsonDataObject);
	static QVector3D	GetVertexRaw(const QJsonObject& jsonDataObject, qsizetype vertexIndex);

	static qsizetype	GetTrianglesCountRaw(const QJsonObject& jsonDataObject);
	static CTriangle	GetTriangleRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex);
	static qsizetype	GetTriangleVertexIndexRaw(const QJsonObject& jsonDataObject, qsizetype triangleIndex, qsizetype vertexNum);

protected:
	void Work() override;

private:
	QJsonObject				DataObject;

};

#endif // MESHINITIALIZER_H
