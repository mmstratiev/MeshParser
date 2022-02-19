#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>
#include <ThreadedObj.h>
#include <QFunctionPointer>
class QByteArray;

struct SMeshStats
{
	double MinTriangleArea = 0.0f;
	double MaxTriangleArea = 0.0f;
	double AvgTriangleArea = 0.0f;
};

class CGeometryObject : public CThreadedObj
{
public:
    CGeometryObject(const QByteArray& jsonByteArr);

    QJsonArray GetVertices() const;
    QJsonArray GetTriangles() const;

	// Multithreaded method
	void GetStats(std::function<void(const SMeshStats&)> funcCallback);

private:
    QJsonObject GetInnerObject() const;

private:
    QJsonObject Object;

};

#endif // GEOMETRYOBJECT_H
