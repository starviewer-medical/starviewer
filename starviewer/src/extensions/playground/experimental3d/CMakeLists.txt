set(SOURCES
    ambientvoxelshader2.cpp
    camera.cpp
    celshadingvoxelshader.cpp
    colorbleedingvoxelshader.cpp
    colorvomivoxelshader.cpp
    coolwarmvoxelshader.cpp
    directilluminationvoxelshader2.cpp
    experimental3dextensionmediator.cpp
    experimental3dsettings.cpp
    experimental3dvolume.cpp
    filteringambientocclusionmapvoxelshader.cpp
    filteringambientocclusionstipplingvoxelshader.cpp
    filteringambientocclusionvoxelshader.cpp
    histogram.cpp
    imivoxelshader.cpp
    informationtheory.cpp
    matrix3.cpp
    matrix4.cpp
    obscurance.cpp
    obscurancemainthread.cpp
    obscurancethread.cpp
    obscurancevoxelshader.cpp
    opacityvoxelshader.cpp
    optimizetransferfunctioncommand.cpp
    qbasicgraphictransferfunctioneditor.cpp
    qexperimental3dextension.cpp
    qexperimental3dviewer.cpp
    qgraphictransferfunctioneditor.cpp
    quaternion.cpp
    qviewpointdistributionwidget.cpp
    viewpointinformationchannel.cpp
    vmivoxelshader2.cpp
    volumereslicer.cpp
    vomicoolwarmvoxelshader.cpp
    vomigammavoxelshader.cpp
    vomivoxelshader.cpp
    voxelsaliencyvoxelshader.cpp
    vtkVolumeRayCastCompositeFunction.cxx
    vtkVolumeRayCastFunction.cxx
    vtkVolumeRayCastMapper.cxx
    vtkVolumeRayCastVoxelShaderCompositeFunction.cxx
    whitevoxelshader.cpp
)

#[[ Untested draft for optional CUDA support
if(USE_CUDA)
    list(APPEND SOURCES
        cudafiltering.cu
        cudaviewpointinformationchannel.cu
        qcudarenderwindow.cpp
        viewpointintensityinformationchannel.cpp
    )
endif()
]]

set(FORMS
    qexperimental3dextensionbase.ui
    qgraphictransferfunctioneditorbase.ui
    qviewpointdistributionwidgetbase.ui
)

set(RESOURCES
    experimental3d.qrc
)

add_translations(QM_FILES experimental3d translations)

add_library(experimental3d ${SOURCES} ${FORMS} ${RESOURCES} ${QM_FILES})


################ Internal library dependencies ################

get_target_includes(CORE_INCLUDES core YES)
target_include_directories(experimental3d PUBLIC ${CORE_INCLUDES})

get_target_includes(GLEW_INCLUDES glew YES)
target_include_directories(experimental3d PUBLIC ${GLEW_INCLUDES})
target_link_libraries(experimental3d glew)


################ External library dependencies ################

find_package(Qt5 REQUIRED COMPONENTS
    Concurrent
    Core
    Widgets
)
target_link_libraries(experimental3d Qt5::Concurrent Qt5::Core Qt5::Widgets)

find_package(VTK REQUIRED COMPONENTS
    vtkCommonDataModel
)
include(${VTK_USE_FILE})

find_package(ITK REQUIRED COMPONENTS
    ITKCommon
)
set(ITK_NO_IO_FACTORY_REGISTER_MANAGER ON)
include(${ITK_USE_FILE})
