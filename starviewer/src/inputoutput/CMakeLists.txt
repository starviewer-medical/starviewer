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
    dicomjsondecoder.cpp
    dicommask.cpp
    dicommasktodcmdataset.cpp
    dimsecservice.cpp
    dimsequerystudyoperationresult.cpp
    dimseretrievestudyoperationresult.cpp
    dimsestorestudyoperationresult.cpp
    downloadedfilesprocessor.cpp
    echotopacs.cpp
    echotopacstest.cpp
    echotowadopacs.cpp
    externalstudyrequestmanager.cpp
    firewallaccess.cpp
    firewallaccesstest.cpp
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
    pacsdevicemodel.cpp
    pacsjob.cpp
    pacsmanager.cpp
    parsexmlrispierrequest.cpp
    portinuse.cpp
    portinusebyanotherapplication.cpp
    portinusetest.cpp
    qadvancedsearchwidget.cpp
    qbasicsearchwidget.cpp
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
    qpacsconfigurationscreen.cpp
    qpacsdialog.cpp
    qpacslist.cpp
    qpopupexternalstudyrequestsscreen.cpp
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
    risrequestsportinusetest.cpp
    senddicomfilestopacs.cpp
    senddicomfilestopacsjob.cpp
    status.cpp
    studyoperationresult.cpp
    studyoperationsservice.cpp
    upgradedatabaserevisioncommands.cpp
    upgradedatabasexmlparser.cpp
    usermessage.cpp
    utils.cpp
    wadoinstancedownloader.cpp
    wadorequest.cpp
    wadorequestmanager.cpp
    wadoretrieverequest.cpp
    wadoretrievestudyoperationresult.cpp
    wadosearchrequest.cpp
    wadosearchstudyoperationresult.cpp
    wadostorerequest.cpp
    wadostorestudyoperationresult.cpp
)

if(WIN32)
    list(APPEND SOURCES
        windowsfirewallaccess.cpp
        windowsportinusebyanotherapplication.cpp
    )
elseif(UNIX AND NOT APPLE)
    list(APPEND SOURCES
        linuxfirewallaccess.cpp
    )
endif()

set(FORMS
    qadvancedsearchwidgetbase.ui
    qbasicsearchwidgetbase.ui
    qcreatedicomdirbase.ui
    qdicomdirconfigurationscreenbase.ui
    qexporterbase.ui
    qinputoutputdicomdirwidgetbase.ui
    qinputoutputlocaldatabasewidgetbase.ui
    qinputoutputpacswidgetbase.ui
    qlistenrisrequestsconfigurationscreenbase.ui
    qlocaldatabaseconfigurationscreenbase.ui
    qoperationstatescreenbase.ui
    qpacsconfigurationscreenbase.ui
    qpacsdialogbase.ui
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
target_include_directories(inputoutput PUBLIC ${CORE_INCLUDES})

target_link_libraries(inputoutput core)


################ External library dependencies ################

target_link_qt(inputoutput
    Core
    Gui
    Network
    Sql
    Widgets
)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkCommonDataModel
    vtkIOImage
    vtkRenderingCore
)
target_link_libraries(inputoutput ${VTK_LIBRARIES})

find_package(DCMTK REQUIRED)
target_include_directories(inputoutput SYSTEM PUBLIC
    ${DCMTK_INCLUDE_DIRS}/dcmtk/config
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmdata
    ${DCMTK_INCLUDE_DIRS}/dcmtk/dcmnet
    ${DCMTK_INCLUDE_DIRS}/dcmtk/ofstd
)
target_link_libraries(inputoutput ${DCMTK_LIBRARIES})

find_package(GDCM REQUIRED)
target_link_libraries(inputoutput gdcmCommon gdcmDICT gdcmDSED gdcmIOD gdcmMSFF)

find_package(KF5ThreadWeaver REQUIRED)
target_link_libraries(inputoutput KF5::ThreadWeaver)
