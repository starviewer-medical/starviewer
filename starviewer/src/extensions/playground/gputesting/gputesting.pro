# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/gputesting
# L'objectiu � una aplicaci??:  

FORMS += qgputestingextensionbase.ui 
HEADERS += gputestingextensionmediator.h \
           qgputestingextension.h \
           gputestingsettings.h \
           qgputestingviewer.h \
           camera.h \
           quaternion.h \
           matrix3.h \
           matrix4.h \
           gpuprogram.h 
SOURCES += gputestingextensionmediator.cpp \
           qgputestingextension.cpp \
           gputestingsettings.cpp \
           qgputestingviewer.cpp \
           camera.cpp \
           quaternion.cpp \
           matrix3.cpp \
           matrix4.cpp \
           gpuprogram.cpp 
RESOURCES = gputesting.qrc
QT += opengl

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
