#ifndef DCEL_H
#define DCEL_H

#include <QVector3D>
#include <unordered_map>

#include "DCEL_Edge.h"
#include "DCEL_Vertex.h"
#include "DCEL_Face.h"

// Doubly connected edge list - https://en.wikipedia.org/wiki/Doubly_connected_edge_list
class CDCEL
{
public:
	CDCEL();

	void Clear();

	TDCEL_EdgePtr	AddEdge(TDCEL_EdgeID ID, TDCEL_VertPtr origin = nullptr, TDCEL_EdgePtr next = nullptr, TDCEL_EdgePtr prev = nullptr, TDCEL_FacePtr face = nullptr);
	TDCEL_VertPtr	AddVertex(TDCEL_VertID ID, QVector3D vertex, TDCEL_EdgePtr halfEdge = nullptr);
	TDCEL_FacePtr	AddFace(TDCEL_FaceID ID, TDCEL_EdgePtr halfEdge = nullptr);

	qsizetype		GetEdgesCount() const;
	TDCEL_EdgePtr	GetEdge(TDCEL_EdgeID Id) const;

	qsizetype		GetVerticesCount() const;
	TDCEL_VertPtr	GetVertex(TDCEL_VertID Id) const;
	QVector3D		GetVertexNormal(TDCEL_VertID Id) const;

	qsizetype		GetFacesCount() const;
	TDCEL_FacePtr	GetFace(TDCEL_FaceID Id) const;

private:
	std::unordered_map<TDCEL_EdgeID, std::unique_ptr<CDCEL_Edge>, SDCEL_EdgeID>	Edges;
	std::unordered_map<TDCEL_VertID, std::unique_ptr<CDCEL_Vertex>>				Vertices;
	std::unordered_map<TDCEL_FaceID, std::unique_ptr<CDCEL_Face>>				Faces;
};


#endif // DCEL_H
