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
    qcreatedicomdirbase.ui \
    qoperationstatescreenbase.ui \
    queryscreenbase.ui \
    qadvancedsearchwidgetbase.ui \
    qbasicsearchwidgetbase.ui \
    qlocaldatabaseconfigurationscreenbase.ui \
    qlistenrisrequestsconfigurationscreenbase.ui \
    qinputoutputdicomdirwidgetbase.ui \
    qinputoutputlocaldatabasewidgetbase.ui \
    qinputoutputpacswidgetbase.ui \
    qdicomdirconfigurationscreenbase.ui \
    qwidgetselectpacstostoredicomimagebase.ui \
    qexporterbase.ui \
    qseriesthumbnailpreviewwidgetbase.ui \
    qthumbnailsspreviewwidgetbase.ui
HEADERS += databaseconnection.h \
    pacsdevicemanager.h \
    pacsconnection.h \
    dimsecservice.h \
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
    qseriesthumbnailpreviewwidget.h \
    qcreatedicomdir.h \
    qoperationstatescreen.h \
    queryscreen.h \
    qadvancedsearchwidget.h \
    qbasicsearchwidget.h \
    localdatabasemanager.h \
    localdatabasebasedal.h \
    localdatabasedisplayshutterdal.h \
    localdatabaseimagedal.h \
    localdatabaseseriesdal.h \
    localdatabasestudydal.h \
    localdatabasepatientdal.h \
    localdatabaseutildal.h \
    qdeleteoldstudiesthread.h \
    databaseinstallation.h \
    qlocaldatabaseconfigurationscreen.h \
    parsexmlrispierrequest.h \
    listenrisrequests.h \
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
    relatedstudiesmanager.h \
    risrequestwrapper.h \
    qwidgetselectpacstostoredicomimage.h \
    qrelatedstudieswidget.h \
    qexportertool.h \
    pacsjob.h \
    senddicomfilestopacsjob.h \
    pacsrequeststatus.h \
    retrievedicomfilesfrompacsjob.h \
    echotopacs.h \
    gdcmanonymizerstarviewer.h \
    dicomanonymizer.h \
    dicommasktodcmdataset.h \
    upgradedatabaserevisioncommands.h \
    upgradedatabasexmlparser.h \
    qthumbnailspreviewwidget.h \
    portinuse.h \
    portinusetest.h \
    echotopacstest.h \
    risrequestsportinusetest.h \
    incomingdicomconnectionsportinusetest.h \
    dicomdirburningapplicationtest.h \
    localdatabasepacsretrievedimagesdal.h \
    cachetest.h \
    usermessage.h \
    portinusebyanotherapplication.h \
    localdatabasevoilutdal.h \
    localdatabaseencapsulateddocumentdal.h
SOURCES += databaseconnection.cpp \
    pacsdevicemanager.cpp \
    pacsconnection.cpp \
    dimsecservice.cpp \
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
    qseriesthumbnailpreviewwidget.cpp \
    qcreatedicomdir.cpp \
    qoperationstatescreen.cpp \
    queryscreen.cpp \
    qadvancedsearchwidget.cpp \
    qbasicsearchwidget.cpp \
    localdatabasemanager.cpp \
    localdatabasebasedal.cpp \
    localdatabasedisplayshutterdal.cpp \
    localdatabaseimagedal.cpp \
    localdatabaseseriesdal.cpp \
    localdatabasestudydal.cpp \
    localdatabasepatientdal.cpp \
    localdatabaseutildal.cpp \
    qdeleteoldstudiesthread.cpp \
    databaseinstallation.cpp \
    qlocaldatabaseconfigurationscreen.cpp \
    parsexmlrispierrequest.cpp \
    listenrisrequests.cpp \
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
    relatedstudiesmanager.cpp \
    risrequestwrapper.cpp \
    qwidgetselectpacstostoredicomimage.cpp \
    qrelatedstudieswidget.cpp \
    qexportertool.cpp \
    pacsjob.cpp \
    senddicomfilestopacsjob.cpp  \
    retrievedicomfilesfrompacsjob.cpp \
    echotopacs.cpp \
    gdcmanonymizerstarviewer.cpp \
    dicomanonymizer.cpp \
    dicommasktodcmdataset.cpp \
    upgradedatabaserevisioncommands.cpp \
    upgradedatabasexmlparser.cpp \
    qthumbnailspreviewwidget.cpp \
    portinuse.cpp \
    portinusetest.cpp \
    echotopacstest.cpp \
    risrequestsportinusetest.cpp \
    incomingdicomconnectionsportinusetest.cpp \
    dicomdirburningapplicationtest.cpp \
    localdatabasepacsretrievedimagesdal.cpp \
    cachetest.cpp \
    usermessage.cpp \
    portinusebyanotherapplication.cpp \
    localdatabasevoilutdal.cpp \
    localdatabaseencapsulateddocumentdal.cpp
win32 {
    HEADERS += windowsportinusebyanotherapplication.h
    SOURCES += windowsportinusebyanotherapplication.cpp
}
INCLUDEPATH += ../core
TEMPLATE = lib
DESTDIR = ./
CONFIG += staticlib
include(../corelibsconfiguration.pri)
include(../vtk.pri)
include(../gdcm.pri)
include(../itk.pri)
include(../dcmtk.pri)
include(../compilationtype.pri)
include(../threadweaver.pri)
QT += xml \
    network \
    widgets \
    sql
