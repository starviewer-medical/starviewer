include(../../compilationtype.inc)
infile(config.pri, SOLUTIONS_LIBRARY, yes): CONFIG += qtsingleapplication-uselib
TEMPLATE_OLD = $$TEMPLATE
TEMPLATE = fakelib
QTSINGLEAPPLICATION_LIBNAME = $$qtLibraryTarget(QtSolutions_SingleApplication-2.6)
TEMPLATE = $$TEMPLATE_OLD
QTSINGLEAPPLICATION_LIBDIR = $$PWD/../../../bin
unix:qtsingleapplication-uselib:!qtsingleapplication-buildlib:QMAKE_RPATHDIR += $$QTSINGLEAPPLICATION_LIBDIR
