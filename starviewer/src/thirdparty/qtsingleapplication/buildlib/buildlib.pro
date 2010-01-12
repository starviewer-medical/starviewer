TEMPLATE=lib
CONFIG += qt dll qtsingleapplication-buildlib
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
include(../src/qtsingleapplication.pri)

#HACK amb la opció qmake -tp vc, la dll de QtSingleApplication es genera com a IMPORT, quan s'hauria de generar com a export (amb qmake sense -tp vc es
# es genera com a export) a qtsingleapplication.pri tenim el següent codi
#win32 {
#    contains(TEMPLATE, lib):contains(CONFIG, shared):DEFINES += QT_QTSINGLEAPPLICATION_EXPORT
#    else:qtsingleapplication-uselib:DEFINES += QT_QTSINGLEAPPLICATION_IMPORT
#}
#En aquest codi es pot veure com si TEMPLATE conté el valor lib, es defineix que la dll s'ha de fer com a export, però ingorem perquè si es genera
#el projecte amb la opció "-tp vc" ignora el contingut de TEMPLATE i salta a l'else indicant que la dll es generi com a import (el contais(CONFIG,shared) 
#funciona correctament). Degut a aquest problema aquí redefinim el valor de DEFINES indicant com hardcoded que es generi la dll com a export.
win32:DEFINES = QT_QTSINGLEAPPLICATION_EXPORT

TARGET = $$QTSINGLEAPPLICATION_LIBNAME
DESTDIR = ../../../../bin
target.path = $$DESTDIR
INSTALLS += target
