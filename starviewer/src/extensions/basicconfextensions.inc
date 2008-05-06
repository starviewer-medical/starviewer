TRANSLATIONS += translations_ca_ES.ts \
                translations_es_ES.ts \
                translations_en_GB.ts

DESTDIR = ./

INCLUDEPATH += ../../../core
DEPENDPATH += ../../../core
MOC_DIR = ../../../../tmp/moc
UI_DIR = ../../../../tmp/ui
OBJECTS_DIR = ../../../../tmp/obj
RCC_DIR = ../../../../tmp/rcc
CONFIG += release \
          warn_on \
          qt \
          opengl \
          thread \
          x11 \
          staticlib \
          exceptions \
          stl
TEMPLATE = lib

include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)

