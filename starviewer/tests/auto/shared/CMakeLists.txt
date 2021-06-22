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

get_target_includes(CORE_INCLUDES core NO)
target_include_directories(test_shared PRIVATE ${CORE_INCLUDES})

get_target_includes(INPUTOUTPUT_INCLUDES inputoutput NO)
target_include_directories(test_shared PRIVATE ${INPUTOUTPUT_INCLUDES})

get_target_includes(INTERFACE_INCLUDES interface NO)
target_include_directories(test_shared PRIVATE ${INTERFACE_INCLUDES})

get_target_includes(Q2DVIEWER_INCLUDES q2dviewer NO)
target_include_directories(test_shared PRIVATE ${Q2DVIEWER_INCLUDES})

get_target_includes(Q3DVIEWER_INCLUDES q3dviewer NO)
target_include_directories(test_shared PRIVATE ${Q3DVIEWER_INCLUDES})

get_target_includes(MPR2D_INCLUDES mpr2d NO)
target_include_directories(test_shared PRIVATE ${MPR2D_INCLUDES})


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
)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})

find_package(DCMTK REQUIRED)
target_include_directories(test_shared SYSTEM PRIVATE
    ${DCMTK_INCLUDE_DIRS}
    ${DCMTK_INCLUDE_DIRS}/dcmtk/ofstd
)
