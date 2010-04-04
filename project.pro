######################################################################
# Automatically generated by qmake (2.01a) Fri Jan 29 14:32:31 2010
######################################################################

TEMPLATE = app
QT = gui core opengl
CONFIG += qt debug warn_on console opengl
DESTDIR = bin
OBJECTS_DIR = build
MOC_DIR = build
UI_DIR = build
LIBS += -lGLEW
CFLAGS += -Wno-unknown-pragmas
CXXFLAGS += -Wno-unknown-pragmas
DEPENDPATH += . src/main src/re330
INCLUDEPATH += . src/main src/re330

# Input
HEADERS += src/main/MainWidget.h \
           src/main/MainWindow.h \
           src/main/ObjReader.h \
           src/main/RenderWidget0.h \
           src/re330/BasicMath.h \
           src/re330/Camera.h \
           src/re330/Frustum.h \
           src/re330/glew.h \
           src/re330/GLRenderContext.h \
           src/re330/GLRenderWidget.h \
           src/re330/GLWidget.h \
           src/re330/Light.h \
           src/re330/Material.h \
           src/re330/Shader.h \
           src/re330/Texture.h \
           src/re330/Matrix4.h \
           src/re330/Node.h \
           src/re330/Object.h \
           src/re330/RE330_global.h \
           src/re330/RenderContext.h \
           src/re330/RenderWidget.h \
           src/re330/SceneManager.h \
           src/re330/Shapes.h \
           src/re330/Singleton.h \
           src/re330/Vector3.h \
           src/re330/Vector4.h \
           src/re330/VertexData.h \
           src/re330/VertexDeclaration.h \
           src/re330/wglew.h
SOURCES += src/main/main.cpp \
           src/main/MainWidget.cpp \
           src/main/MainWindow.cpp \
           src/main/ObjReader.cpp \
           src/main/RenderWidget0.cpp \
           src/re330/BasicMath.cpp \
           src/re330/GLRenderContext.cpp \
           src/re330/GLWidget.cpp \
           src/re330/Light.cpp \
           src/re330/Material.cpp \
           src/re330/Shader.cpp \
           src/re330/Texture.cpp \
           src/re330/Matrix4.cpp \
           src/re330/Camera.cpp \
           src/re330/Frustum.cpp \
           src/re330/RenderContext.cpp \
           src/re330/SceneManager.cpp \
           src/re330/Shapes.cpp \
           src/re330/VertexData.cpp \
           src/re330/VertexDeclaration.cpp
