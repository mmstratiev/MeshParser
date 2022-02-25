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

	TDCEL_VertPtr	AddVertex(TDCEL_VertID ID, QVector3D vertex);
	TDCEL_FacePtr	Connect(TDCEL_VertID vert1, TDCEL_VertID vert2, TDCEL_VertID vert3, TDCEL_FaceID newFaceId);
	TDCEL_FacePtr	Connect(TDCEL_VertID vert1, TDCEL_VertID vert2, TDCEL_VertID vert3);

	qsizetype		GetEdgesCount() const;
	TDCEL_EdgePtr	GetEdge(TDCEL_EdgeID Id) const;

	qsizetype		GetVerticesCount() const;
	TDCEL_VertPtr	GetVertex(TDCEL_VertID Id) const;

	qsizetype		GetFacesCount() const;
	TDCEL_FacePtr	GetFace(TDCEL_FaceID Id) const;

private:
	TDCEL_FacePtr	AddFace(TDCEL_FaceID ID);
	TDCEL_EdgePtr	AddEdge(TDCEL_EdgeID ID);

private:
	std::unordered_map<TDCEL_EdgeID, std::unique_ptr<CDCEL_Edge>, SDCEL_EdgeID>	Edges;
	std::unordered_map<TDCEL_VertID, std::unique_ptr<CDCEL_Vertex>>				Vertices;
	std::unordered_map<TDCEL_FaceID, std::unique_ptr<CDCEL_Face>>				Faces;
};

#endif // DCEL_H
