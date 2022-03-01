#include "MeshAnalyzer.h"

#include <QQuaternion>

CMeshAnalyzer::CMeshAnalyzer(CGeometryObject& inOutObject, QObject *parent)
	: CThreadWorker(inOutObject, parent)
{}

void CMeshAnalyzer::Work()
{
	for(int triangleIndex = BeginIndex; triangleIndex < EndIndex; triangleIndex++)
	{
		TDCEL_FacePtr faceToCheck = GeometryObject.EdgeList.GetFace(triangleIndex);

		if(!faceToCheck)
		{
			GeometryObject.TempMaxProgress--;
		}
		else
		{
			CTriangle triangle = faceToCheck->Get();
			double triangleArea = triangle.GetArea();

			QMutexLocker locker(&GeometryObject.Mutex);
			GeometryObject.BoundingVolHierarchy.AddLeaf(faceToCheck);

			GeometryObject.MaxTriangleArea	= std::max<double>(GeometryObject.MaxTriangleArea, triangleArea);
			GeometryObject.MinTriangleArea	= std::min<double>(GeometryObject.MinTriangleArea, triangleArea);
			GeometryObject.TotalArea		= GeometryObject.TotalArea + triangleArea;

			// Find out if mesh is closed or open
			if(GeometryObject.IsClosed())
			{
				TDCEL_EdgePtr currentEdge = faceToCheck->Edge();
				do
				{
					GeometryObject.bIsClosed = currentEdge->Twin();
					currentEdge = currentEdge->Next();
				} while(currentEdge != faceToCheck->Edge() && GeometryObject.bIsClosed);
			}
			GeometryObject.TempProgress++;

			// OpenGL uses Y as height and Z as depth
			auto flipZY = [](QVector3D in) -> QVector3D
			{
				return QQuaternion::fromEulerAngles(-90, 0, 180).rotatedVector(in);
			};

			// Build OpenGL array
			QVector3D triangleNormal = triangle.GetNormal();
			CFaceVerticesIterator vertIterator = faceToCheck->GetFaceVerticesIterator();
			while(!vertIterator.End())
			{
				GeometryObject.OpenGLVertices.push_back(COpenGLVertex(flipZY((*vertIterator)->Get()), triangleNormal, (*vertIterator)->GetNormal()));
				++vertIterator;
			}
		}
		emit MadeProgress();
	}
}
