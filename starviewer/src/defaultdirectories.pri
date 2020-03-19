# Directoris per defecte de les diferents llibreries. Si vols que siguin uns altres, simplement has de declarar
# com a variables de sistema les que vulguis substituir. Ex.: export ITKLIBDIR=/usr/lib64/InsightToolkit

# Default install prefix is /usr/local
SDK_INSTALL_PREFIX = $$(SDK_INSTALL_PREFIX)
isEmpty(SDK_INSTALL_PREFIX){
    unix:SDK_INSTALL_PREFIX = $$(HOME)/starviewer-sdk-0.15/usr/local
    win32:SDK_INSTALL_PREFIX = $$(USERPROFILE)/starviewer-sdk-0.15/32
    win32:contains(QMAKE_TARGET.arch, x86_64):SDK_INSTALL_PREFIX = $$(USERPROFILE)/starviewer-sdk-0.15/64
}

exists(/etc/debian_version):linux:LIB64 = lib/x86_64-linux-gnu  # Debian-based systems
!exists(/etc/debian_version):linux:LIB64 = lib64                # Other systems
macx:LIB64 = lib


# DCMTK Libraries

DCMTKLIBDIR = $$(DCMTKLIBDIR)
isEmpty(DCMTKLIBDIR){
    unix:DCMTKLIBDIR = $$SDK_INSTALL_PREFIX/$$LIB64
    win32:DCMTKLIBDIR = $$SDK_INSTALL_PREFIX/dcmtk/3.6.5/lib
}
DCMTKINCLUDEDIR = $$(DCMTKINCLUDEDIR)
isEmpty(DCMTKINCLUDEDIR){
    unix:DCMTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/dcmtk
    win32:DCMTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/dcmtk/3.6.5/include/dcmtk
}


# VTK Libraries

VTKLIBDIR = $$(VTKLIBDIR)
isEmpty(VTKLIBDIR){
    unix:VTKLIBDIR = $$SDK_INSTALL_PREFIX/$$LIB64
    win32:VTKLIBDIR = $$SDK_INSTALL_PREFIX/VTK/8.2.0/lib
}
VTKINCLUDEDIR = $$(VTKINCLUDEDIR)
isEmpty(VTKINCLUDEDIR){
    unix:VTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/vtk-8.2
    win32:VTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/VTK/8.2.0/include/vtk-8.2
}


# ITK Libraries

ITKLIBDIR = $$(ITKLIBDIR)
isEmpty(ITKLIBDIR){
    unix:ITKLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:ITKLIBDIR = $$SDK_INSTALL_PREFIX/InsightToolkit/5.0.1/lib
}
ITKINCLUDEDIR = $$(ITKINCLUDEDIR)
isEmpty(ITKINCLUDEDIR){
    unix:ITKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/ITK-5.0
    win32:ITKINCLUDEDIR = $$SDK_INSTALL_PREFIX/InsightToolkit/5.0.1/include/ITK-5.0
}

# GDCM Libraries

GDCMLIBDIR = $$(GDCMLIBDIR)
isEmpty(GDCMLIBDIR){
    unix:GDCMLIBDIR = $$SDK_INSTALL_PREFIX/lib
    win32:GDCMLIBDIR = $$SDK_INSTALL_PREFIX/gdcm/3.0.4/lib
}
GDCMINCLUDEDIR = $$(GDCMINCLUDEDIR)
isEmpty(GDCMINCLUDEDIR){
    unix:GDCMINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/gdcm-3.0
    win32:GDCMINCLUDEDIR = $$SDK_INSTALL_PREFIX/gdcm/3.0.4/include/gdcm-3.0
}

# Threadweaver libraries

THREADWEAVERLIBDIR = $$(THREADWEAVERLIBDIR)
isEmpty(THREADWEAVERLIBDIR){
    unix:THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/$$LIB64
    win32:THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/ThreadWeaver/5.64.0/lib
}
THREADWEAVERINCLUDEDIR = $$(THREADWEAVERINCLUDEDIR)
isEmpty(THREADWEAVERINCLUDEDIR){
    unix:THREADWEAVERINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/KF5
    win32:THREADWEAVERINCLUDEDIR = $$SDK_INSTALL_PREFIX/ThreadWeaver/5.64.0/include/KF5
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
