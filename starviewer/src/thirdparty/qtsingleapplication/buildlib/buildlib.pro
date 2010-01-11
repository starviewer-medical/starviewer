TEMPLATE=lib
CONFIG += qt dll qtsingleapplication-buildlib
mac:CONFIG += absolute_library_soname
win32|mac:!wince*:!win32-msvc:!macx-xcode:CONFIG += debug_and_release build_all
include(../src/qtsingleapplication.pri)
TARGET = $$QTSINGLEAPPLICATION_LIBNAME
DESTDIR = ../../../../bin
target.path = $$DESTDIR
INSTALLS += target
