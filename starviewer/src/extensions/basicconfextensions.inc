TRANSLATIONS += $$EXTENSION_DIR/translations_ca_ES.ts \
                $$EXTENSION_DIR/translations_es_ES.ts \
                $$EXTENSION_DIR/translations_en_GB.ts

DESTDIR = ./

INCLUDEPATH += ../../../core \
               ../../../inputoutput
DEPENDPATH += ../../../core \
               ../../../inputoutput
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
include(../gdcm.inc)
include(../dcmtk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)

