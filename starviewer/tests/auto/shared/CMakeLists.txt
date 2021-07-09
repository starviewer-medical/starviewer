set(SOURCES
    databasetesthelper.cpp
    dicomsourcetesthelper.cpp
    drawerpolygontesthelper.cpp
    fuzzycomparetesthelper.cpp
    hangingprotocoldisplaysettesthelper.cpp
    hangingprotocolimagesettesthelper.cpp
    hangingprotocoltesthelper.cpp
    imageoverlaytesthelper.cpp
    imagetesthelper.cpp
    itkandvtkimagetesthelper.cpp
    pacsdevicetesthelper.cpp
    patienttesthelper.cpp
    seriestesthelper.cpp
    studytesthelper.cpp
    testingdecaycorrectionfactorformulacalculator.cpp
    testingdicomtagreader.cpp
    testingmammographyimagehelper.cpp
    testingpacsconnection.cpp
    testingportinuse.cpp
    testingportinusebyanotherapplication.cpp
    testingsenddicomfilestopacs.cpp
    testingsettings.cpp
    testingvolume.cpp
    testingvolumereader.cpp
    volumepixeldatatesthelper.cpp
    volumetesthelper.cpp
)

add_library(test_shared OBJECT ${SOURCES})


################ Internal library dependencies ################

target_link_libraries(test_shared core inputoutput interface q2dviewer q3dviewer mpr2d)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Core
    Gui
    Network
    Test
)
target_link_libraries(test_shared Qt5::Core Qt5::Gui Qt5::Network Qt5::Test)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonCore
    vtkCommonDataModel
    vtkRenderingCore
)
target_link_libraries(test_shared ${VTK_LIBRARIES})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
target_link_libraries(test_shared ${ITK_LIBRARIES})
