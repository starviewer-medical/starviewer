# Directoris per defecte de les diferents llibreries. Si vols que siguin uns altres, simplement has de declarar
# com a variables de sistema les que vulguis substituir. Ex.: export ITKLIBDIR=/usr/lib64/InsightToolkit

# Default install prefix is /usr/local
SDK_INSTALL_PREFIX = $$(SDK_INSTALL_PREFIX)
isEmpty(SDK_INSTALL_PREFIX){
    unix:SDK_INSTALL_PREFIX = $$(HOME)/starviewer-sdk-0.15/usr/local
    win32:SDK_INSTALL_PREFIX = $$(USERPROFILE)/starviewer-sdk-0.15/32
    win32:contains(QMAKE_TARGET.arch, x86_64):SDK_INSTALL_PREFIX = $$(USERPROFILE)/starviewer-sdk-0.15/64
}

# DCMTK Libraries

DCMTKLIBDIR = $$(DCMTKLIBDIR)
isEmpty(DCMTKLIBDIR){
    unix:DCMTKLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:DCMTKLIBDIR = $$SDK_INSTALL_PREFIX/dcmtk/3.6.1_20120515/lib
}
DCMTKINCLUDEDIR = $$(DCMTKINCLUDEDIR)
isEmpty(DCMTKINCLUDEDIR){
    unix:DCMTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/dcmtk
    win32:DCMTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/dcmtk/3.6.1_20120515/include/dcmtk
}


# VTK Libraries

VTKLIBDIR = $$(VTKLIBDIR)
isEmpty(VTKLIBDIR){
    unix:VTKLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:VTKLIBDIR = $$SDK_INSTALL_PREFIX/VTK/8.1.1/lib
}
VTKINCLUDEDIR = $$(VTKINCLUDEDIR)
isEmpty(VTKINCLUDEDIR){
    unix:VTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/vtk-8.1
    win32:VTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/VTK/8.1.1/include/vtk-8.1
}


# ITK Libraries

ITKLIBDIR = $$(ITKLIBDIR)
isEmpty(ITKLIBDIR){
    unix:ITKLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:ITKLIBDIR = $$SDK_INSTALL_PREFIX/InsightToolkit/4.13.0/lib
}
ITKINCLUDEDIR = $$(ITKINCLUDEDIR)
isEmpty(ITKINCLUDEDIR){
    unix:ITKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/ITK-4.13
    win32:ITKINCLUDEDIR = $$SDK_INSTALL_PREFIX/InsightToolkit/4.13.0/include/ITK-4.13
}

# GDCM Libraries

GDCMLIBDIR = $$(GDCMLIBDIR)
isEmpty(GDCMLIBDIR){
    unix:GDCMLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:GDCMLIBDIR = $$SDK_INSTALL_PREFIX/gdcm/2.8.6/lib
}
GDCMINCLUDEDIR = $$(GDCMINCLUDEDIR)
isEmpty(GDCMINCLUDEDIR){
    unix:GDCMINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/gdcm-2.8
    win32:GDCMINCLUDEDIR = $$SDK_INSTALL_PREFIX/gdcm/2.8.6/include/gdcm-2.8
}

# Threadweaver libraries

THREADWEAVERLIBDIR = $$(THREADWEAVERLIBDIR)
isEmpty(THREADWEAVERLIBDIR){
    exists(/etc/debian_version):unix:THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/lib/x86_64-linux-gnu # Debian-based systems
    !exists(/etc/debian_version):unix:THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/lib64               # Other systems
    macx:THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/ThreadWeaver/5.46.0/lib
}
THREADWEAVERINCLUDEDIR = $$(THREADWEAVERINCLUDEDIR)
isEmpty(THREADWEAVERINCLUDEDIR){
    unix:THREADWEAVERINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/KF5
    win32:THREADWEAVERINCLUDEDIR = $$SDK_INSTALL_PREFIX/ThreadWeaver/5.46.0/include/KF5
}


# CUDA Libraries

CUDALIBDIR = $$(CUDALIBDIR)
isEmpty(CUDALIBDIR){
    unix:CUDALIBDIR = /usr/local/cuda/lib
}
CUDAINCLUDEDIR = $$(CUDAINCLUDEDIR)
isEmpty(CUDAINCLUDEDIR){
    unix:CUDAINCLUDEDIR = /usr/local/cuda/include
}
# De moment cal el CUDA SDK, però s'hauria de poder treballar sense ell
CUDASDKINCLUDEDIR = $$(CUDASDKINCLUDEDIR)
isEmpty(CUDASDKINCLUDEDIR){
    unix:CUDASDKINCLUDEDIR = /usr/local/cuda-sdk/common/inc
}
