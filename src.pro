TEMPLATE	= lib 
CONFIG		+=  warn_on qt shared windows embed_manifest_exe dll
SOURCES		= src\LayoutPODExportMain.cpp	src\servmain.c  src\shutdown.c src\startup.c src\username.c  src\MainDialog.cpp   src\Database.cpp src\SceneItemOptions.cpp \
			  src\Vector3.cpp src\Vector2.cpp src\Vector4.cpp src\Quat.cpp  src\Matrix3.cpp src\Matrix4.cpp src\MathUtils.cpp \
			  src\com_eulerangle.c src\com_math.c src\lay_transform.c src\com_transform.c src\com_vector.c src\com_quat.c src\com_matrix.c src\vecmat.cpp \
			  src\GlobalOptionsDialog.cpp src\AboutDialog.cpp src\SceneHierarchyDialog.cpp
			  
			  
HEADERS		= src\LayoutPODExportMain.h src\MainDialog.h  src\Database.h src\SceneItemOptions.h src\Vector3.h src\Vector2.h src\Vector4.h src\Quat.h  src\Matrix3.h src\Matrix4.h src\MathUtils.h \
			  src\lwcomlib.h src\lay_transform.h src\com_math.h src\com_eulerangle.h src\com_vecmatquat.h src\lwlaylib.h src\com_transform.h src\com_vecmatquat.h src\vecmat.h \
			  src\GlobalOptionsDialog.h src\AboutDialog.h src\SceneHierarchyDialog.h
DEFINES     += _MSWIN
DEFINES     += _X86_
DEF_FILE  = src\serv.def 
TARGET		= LayoutPODExport
QMAKE_LFLAGS+= /incremental:yes
DEPENDPATH =libs
LIBS += User32.lib 
win32:debug:LIBS += Dependencies\debug\OGLESTools.lib
win32:release:LIBS += Dependencies\release\OGLESTools.lib
INCLUDEPATH+= src
INCLUDEPATH+="C:\Program Files\NewTek\LightWave11.5\sdk\include"
INCLUDEPATH+="Dependencies"
INCLUDEPATH+="Dependencies\OGLES"
INCLUDEPATH+="Dependencies\OGLES\Include"

RESOURCES    = application.qrc

win32:debug:CONFIG += console [->if you want to receive qDebug() messages on Windows...]