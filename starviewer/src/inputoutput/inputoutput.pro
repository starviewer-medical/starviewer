# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/inputoutput
# L'objectiu és una biblioteca:
TRANSLATIONS += inputoutput_ca_ES.ts \
    inputoutput_es_ES.ts \
    inputoutput_en_GB.ts
FORMS += qconfigurationscreenbase.ui \
    qpacslistbase.ui \
    qstudytreewidgetbase.ui \
    qserieslistwidgetbase.ui \
    qcreatedicomdirbase.ui \
    qoperationstatescreenbase.ui \
    queryscreenbase.ui \
    qadvancedsearchwidgetbase.ui \
    qbasicsearchwidgetbase.ui \
    qlocaldatabaseconfigurationscreenbase.ui \
    qlistenrisrequestsconfigurationscreenbase.ui \
    qpopuprisrequestsscreenbase.ui \
    qinputoutputdicomdirwidgetbase.ui \
    qinputoutputlocaldatabasewidgetbase.ui \
    qinputoutputpacswidgetbase.ui \
    qdicomdirconfigurationscreenbase.ui \
    qwidgetselectpacstostoredicomimagebase.ui \
    qexporterbase.ui
HEADERS += databaseconnection.h \
    pacsdevicemanager.h \
    pacsdevice.h \
    pacsserver.h \
    retrievedicomfilesfrompacs.h \
    status.h \
    converttodicomdir.h \
    convertdicomtolittleendian.h \
    createdicomdir.h \
    dicomdirreader.h \
    senddicomfilestopacs.h \
    querypacs.h \
    dicommask.h \
    dicomdirimporter.h \
    qconfigurationscreen.h \
    qpacslist.h \
    qstudytreewidget.h \
    qserieslistwidget.h \
    qcreatedicomdir.h \
    qoperationstatescreen.h \
    queryscreen.h \
    errordcmtk.h \
    qadvancedsearchwidget.h \
    qbasicsearchwidget.h \
    localdatabasemanager.h \
    localdatabasebasedal.h \
    localdatabaseimagedal.h \
    localdatabaseseriesdal.h \
    localdatabasestudydal.h \
    localdatabasepatientdal.h \
    localdatabaseutildal.h \
    qdeleteoldstudiesthread.h \
    databaseinstallation.h \
    qlocaldatabaseconfigurationscreen.h \
    parsexmlrispierrequest.h \
    listenrisrequestthread.h \
    qlistenrisrequestsconfigurationscreen.h \
    qpopuprisrequestsscreen.h \
    utils.h \
    inputoutputsettings.h \
    qinputoutputdicomdirwidget.h \
    qinputoutputlocaldatabasewidget.h \
    qinputoutputpacswidget.h \
    qdicomdirconfigurationscreen.h \
    querypacsjob.h \
    pacsmanager.h \
    isoimagefilecreator.h \
    dicomdirburningapplication.h \
    risrequestmanager.h \
    previousstudiesmanager.h \
    qstarviewersapwrapper.h \
    qwidgetselectpacstostoredicomimage.h \
    qpreviousstudieswidget.h \
    qexportertool.h \
    pacsjob.h \
    senddicomfilestopacsjob.h \
    pacsrequeststatus.h \
    retrievedicomfilesfrompacsjob.h
SOURCES += databaseconnection.cpp \
    pacsdevicemanager.cpp \
    pacsdevice.cpp \
    pacsserver.cpp \
    retrievedicomfilesfrompacs.cpp \
    status.cpp \
    converttodicomdir.cpp \
    convertdicomtolittleendian.cpp \
    createdicomdir.cpp \
    dicomdirreader.cpp \
    senddicomfilestopacs.cpp \
    querypacs.cpp \
    dicommask.cpp \
    dicomdirimporter.cpp \
    qconfigurationscreen.cpp \
    qpacslist.cpp \
    qstudytreewidget.cpp \
    qserieslistwidget.cpp \
    qcreatedicomdir.cpp \
    qoperationstatescreen.cpp \
    queryscreen.cpp \
    qadvancedsearchwidget.cpp \
    qbasicsearchwidget.cpp \
    localdatabasemanager.cpp \
    localdatabasebasedal.cpp \
    localdatabaseimagedal.cpp \
    localdatabaseseriesdal.cpp \
    localdatabasestudydal.cpp \
    localdatabasepatientdal.cpp \
    localdatabaseutildal.cpp \
    qdeleteoldstudiesthread.cpp \
    databaseinstallation.cpp \
    qlocaldatabaseconfigurationscreen.cpp \
    parsexmlrispierrequest.cpp \
    listenrisrequestthread.cpp \
    qlistenrisrequestsconfigurationscreen.cpp \
    qpopuprisrequestsscreen.cpp \
    utils.cpp \
    inputoutputsettings.cpp \
    qinputoutputdicomdirwidget.cpp \
    qinputoutputlocaldatabasewidget.cpp \
    qdicomdirconfigurationscreen.cpp \
    qinputoutputpacswidget.cpp \
    querypacsjob.cpp \
    pacsmanager.cpp \
    isoimagefilecreator.cpp \
    dicomdirburningapplication.cpp \
    risrequestmanager.cpp \
    previousstudiesmanager.cpp \
    qstarviewersapwrapper.cpp \
    qwidgetselectpacstostoredicomimage.cpp \
    qpreviousstudieswidget.cpp \
    qexportertool.cpp \
    pacsjob.cpp \
    senddicomfilestopacsjob.cpp  \
    retrievedicomfilesfrompacsjob.cpp
INCLUDEPATH += ../core
DEPENDPATH += ../core
TEMPLATE = lib
DESTDIR = ./
include(../corelibsconfiguration.inc)
include(../vtk.inc)
include(../itk.inc)
include(../dcmtk.inc)
include(../log4cxx.inc)
include(../compilationtype.inc)
include(../threadweaver.inc)
QT += xml \
    network
