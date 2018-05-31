include(../../compilationtype.pri)

QT -= core gui

TEMPLATE = lib
CONFIG += staticlib
DESTDIR = ./

HEADERS += easylogging++.h
SOURCES += easylogging++.cc
