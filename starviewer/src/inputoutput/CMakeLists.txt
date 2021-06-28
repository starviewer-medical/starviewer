set(SOURCES
    cachetest.cpp
    convertdicomtolittleendian.cpp
    converttodicomdir.cpp
    createdicomdir.cpp
    databaseconnection.cpp
    databaseinstallation.cpp
    dicomanonymizer.cpp
    dicomdirburningapplication.cpp
    dicomdirburningapplicationtest.cpp
    dicomdirimporter.cpp
    dicomdirreader.cpp
    dicommask.cpp
    dicommasktodcmdataset.cpp
    dimsecservice.cpp
    echotopacs.cpp
    echotopacstest.cpp
    gdcmanonymizerstarviewer.cpp
    incomingdicomconnectionsportinusetest.cpp
    inputoutputsettings.cpp
    isoimagefilecreator.cpp
    listenrisrequests.cpp
    localdatabasebasedal.cpp
    localdatabasedisplayshutterdal.cpp
    localdatabaseencapsulateddocumentdal.cpp
    localdatabaseimagedal.cpp
    localdatabasemanager.cpp
    localdatabasepacsretrievedimagesdal.cpp
    localdatabasepatientdal.cpp
    localdatabaseseriesdal.cpp
    localdatabasestudydal.cpp
    localdatabaseutildal.cpp
    localdatabasevoilutdal.cpp
    pacsconnection.cpp
    pacsdevicemanager.cpp
    pacsjob.cpp
    pacsmanager.cpp
    parsexmlrispierrequest.cpp
    portinuse.cpp
    portinusebyanotherapplication.cpp
    portinusetest.cpp
    qadvancedsearchwidget.cpp
    qbasicsearchwidget.cpp
    qconfigurationscreen.cpp
    qcreatedicomdir.cpp
    qdeleteoldstudiesthread.cpp
    qdicomdirconfigurationscreen.cpp
    qexportertool.cpp
    qinputoutputdicomdirwidget.cpp
    qinputoutputlocaldatabasewidget.cpp
    qinputoutputpacswidget.cpp
    qlistenrisrequestsconfigurationscreen.cpp
    qlocaldatabaseconfigurationscreen.cpp
    qoperationstatescreen.cpp
    qpacslist.cpp
    qpopuprisrequestsscreen.cpp
    qrelatedstudieswidget.cpp
    qseriesthumbnailpreviewwidget.cpp
    qstudytreewidget.cpp
    qthumbnailspreviewwidget.cpp
    querypacs.cpp
    querypacsjob.cpp
    queryscreen.cpp
    qwidgetselectpacstostoredicomimage.cpp
    relatedstudiesmanager.cpp
    retrievedicomfilesfrompacs.cpp
    retrievedicomfilesfrompacsjob.cpp
    risrequestmanager.cpp
    risrequestsportinusetest.cpp
    risrequestwrapper.cpp
    senddicomfilestopacs.cpp
    senddicomfilestopacsjob.cpp
    status.cpp
    upgradedatabaserevisioncommands.cpp
    upgradedatabasexmlparser.cpp
    usermessage.cpp
    utils.cpp
)

if(WIN32)
    list(APPEND SOURCES
        windowsportinusebyanotherapplication.cpp
    )
endif()

set(FORMS
    qadvancedsearchwidgetbase.ui
    qbasicsearchwidgetbase.ui
    qconfigurationscreenbase.ui
    qcreatedicomdirbase.ui
    qdicomdirconfigurationscreenbase.ui
    qexporterbase.ui
    qinputoutputdicomdirwidgetbase.ui
    qinputoutputlocaldatabasewidgetbase.ui
    qinputoutputpacswidgetbase.ui
    qlistenrisrequestsconfigurationscreenbase.ui
    qlocaldatabaseconfigurationscreenbase.ui
    qoperationstatescreenbase.ui
    qpacslistbase.ui
    qseriesthumbnailpreviewwidgetbase.ui
    qstudytreewidgetbase.ui
    qthumbnailsspreviewwidgetbase.ui
    queryscreenbase.ui
    qwidgetselectpacstostoredicomimagebase.ui
)

add_translations(QM_FILES inputoutput inputoutput)

add_library(inputoutput ${SOURCES} ${FORMS} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(inputoutput PRIVATE ${CORE_INCLUDES})

# Circular dependency between both libraries. (Specified here because both targets must be already defined.)
target_link_libraries(core inputoutput)
target_link_libraries(inputoutput core)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Network
    Sql
    Widgets
)
target_link_libraries(inputoutput Qt5::Core Qt5::Network Qt5::Sql Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
)
include(${VTK_USE_FILE})
target_link_libraries(inputoutput ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
target_link_libraries(inputoutput ${ITK_LIBRARIES})

find_package(DCMTK REQUIRED)
target_include_directories(inputoutput PUBLIC
    ${DCMTK_INCLUDE_DIRS}
    ${DCMTK_INCLUDE_DIRS}/dcmtk/config
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmdata
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmnet
    ${DCMTK_INCLUDE_DIRS}/dcmtk/ofstd
)
target_link_libraries(inputoutput ${DCMTK_LIBRARIES})

find_package(GDCM REQUIRED)
include(${GDCM_USE_FILE})

find_package(KF5ThreadWeaver REQUIRED)
target_link_libraries(inputoutput KF5::ThreadWeaver)
