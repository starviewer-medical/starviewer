# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/gputesting
# L'objectiu és una aplicaci??:  

FORMS += qgputestingextensionbase.ui 
HEADERS += gputestingextensionmediator.h \
           qgputestingextension.h \
           qgputestingviewer.h \
           camera.h \
           quaternion.h \
           matrix3.h \
           matrix4.h \
           gpuprogram.h 
SOURCES += gputestingextensionmediator.cpp \
           qgputestingextension.cpp \
           qgputestingviewer.cpp \
           camera.cpp \
           quaternion.cpp \
           matrix3.cpp \
           matrix4.cpp \
           gpuprogram.cpp 
RESOURCES = gputesting.qrc
QT += opengl
LIBS += -lGLEW
CONFIG += release \
warn_on
TEMPLATE = app
include(../../basicconfextensions.inc)
