QT       += core gui opengl openglwidgets

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++11

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    AccelerationStructs/BVH.cpp \
    AccelerationStructs/BVH_Node.cpp \
    AccelerationStructs/BoundingBox.cpp \
    DCEL/DCEL.cpp \
    DCEL/DCEL_Edge.cpp \
    DCEL/DCEL_Face.cpp \
    DCEL/DCEL_Vertex.cpp \
    DCEL/Iterators/FaceEdgesIterator.cpp \
    DCEL/Iterators/FaceVerticesIterator.cpp \
    DCEL/Iterators/VertexFacesIterator.cpp \
    DCEL/Iterators/VertexVerticesIterator.cpp \
    GeometryObject.cpp \
    Math/Triangle.cpp \
    MeshAnalyzer.cpp \
    MeshReader.cpp \
    MeshSubdivider.cpp \
    OpenGL/Camera3D.cpp \
    OpenGL/InputManager.cpp \
    OpenGL/OpenGLWidget.cpp \
    OpenGL/Vertex.cpp \
    UI/DlgVertexNormals.cpp \
    UI/mainwindow.cpp \
    main.cpp \

HEADERS += \
    AccelerationStructs/BVH.h \
    AccelerationStructs/BVH_Datatypes.h \
    AccelerationStructs/BVH_Node.h \
    AccelerationStructs/BoundingBox.h \
    DCEL/DCEL.h \
    DCEL/DCEL_Datatypes.h \
    DCEL/DCEL_Edge.h \
    DCEL/DCEL_Face.h \
    DCEL/DCEL_Vertex.h \
    DCEL/Iterators/FaceEdgesIterator.h \
    DCEL/Iterators/FaceVerticesIterator.h \
    DCEL/Iterators/VertexFacesIterator.h \
    DCEL/Iterators/VertexVerticesIterator.h \
    Datatypes.h \
    GeometryObject.h \
    Math/Shapes.h \
    Math/Triangle.h \
    MeshAnalyzer.h \
    MeshReader.h \
    MeshSubdivider.h \
    OpenGL/Camera3D.h \
    OpenGL/InputManager.h \
    OpenGL/OpenGLWidget.h \
    OpenGL/Vertex.h \
    UI/DlgVertexNormals.h \
    UI/mainwindow.h \

FORMS += \
    UI/DlgVertexNormals.ui \
    UI/mainwindow.ui \

TRANSLATIONS += \
    MeshParser_en_US.ts
CONFIG += lrelease
CONFIG += embed_translations

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    OpenGL/Shaders/simple.frag \
    OpenGL/Shaders/simple.vert

RESOURCES += \
    resources.qrc
