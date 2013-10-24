TEMPLATE = app
CONFIG += console
CONFIG -= qt

SOURCES += main.cpp \
    useful.cpp \
    scene.cpp \
    NeighborMesh.cpp \
    Mesh.cpp \
    SamplingBasedOnOctree.cpp \
    PCA_utilities.cpp \
    Robust_PCA.cpp \
    Octree.cpp

HEADERS += \
    useful.h \
    Stopwatch.h \
    scene.h \
    NeighborMesh.h \
    Mesh.h \
    constante.h \
    SamplingBasedOnOctree.h \
    PCA.h \
    Robust_PCA.h \
    Octree.h

LIBS += -lglut \
        -lGLU \
        -lGL \



QT += core gui opengl
