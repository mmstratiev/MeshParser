#ifndef GEOMETRYOBJECT_H
#define GEOMETRYOBJECT_H

#include <QJsonObject>

class QByteArray;

class CGeometryObject
{
public:
    CGeometryObject(const QByteArray& jsonByteArr);

    QJsonArray GetVertices() const;
    QJsonArray GetTriangles() const;

private:
    QJsonObject GetInnerObject() const;

private:
    QJsonObject Object;

};

#endif // GEOMETRYOBJECT_H
