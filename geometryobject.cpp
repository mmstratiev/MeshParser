#include "geometryobject.h"

#include <QJsonDocument>
#include <QJsonArray>

CGeometryObject::CGeometryObject(const QByteArray& jsonByteArr)
{
    Object = QJsonDocument::fromJson(jsonByteArr).object();
}

QJsonArray CGeometryObject::GetVertices() const
{
    QJsonArray result;
    QJsonObject innerObject = this->GetInnerObject();

    auto verticesIt = innerObject.find("vertices");
    if(verticesIt != innerObject.end())
    {
        result = verticesIt.value().toArray();
    }

    return result;
}

QJsonArray CGeometryObject::GetTriangles() const
{
    QJsonArray result;
    QJsonObject innerObject = this->GetInnerObject();

    auto verticesIt = innerObject.find("triangles");
    if(verticesIt != innerObject.end())
    {
        result = verticesIt.value().toArray();
    }

    return result;
}

QJsonObject CGeometryObject::GetInnerObject() const
{
    QJsonObject result;

    auto objectIt = Object.find("geometry_object");
    if(objectIt != Object.end())
    {
        result = objectIt.value().toObject();
    }

    return result;
}
