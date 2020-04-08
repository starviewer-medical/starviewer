# Directoris per defecte de les diferents llibreries. Si vols que siguin uns altres, simplement has de declarar
# com a variables de sistema les que vulguis substituir. Ex.: export ITKLIBDIR=/usr/lib64/InsightToolkit

SDK_INSTALL_PREFIX = $$(SDK_INSTALL_PREFIX)
isEmpty(SDK_INSTALL_PREFIX){
    SDK_INSTALL_PREFIX = $$PWD/../../sdk-install
}

exists(/etc/debian_version):linux:LIB64 = lib/x86_64-linux-gnu  # Debian-based systems
!exists(/etc/debian_version):linux:LIB64 = lib64                # Other systems
macx:LIB64 = lib
win32:LIB64 = lib


# DCMTK Libraries

DCMTKLIBDIR = $$(DCMTKLIBDIR)
isEmpty(DCMTKLIBDIR){
    DCMTKLIBDIR = $$SDK_INSTALL_PREFIX/$$LIB64
}
DCMTKINCLUDEDIR = $$(DCMTKINCLUDEDIR)
isEmpty(DCMTKINCLUDEDIR){
    DCMTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/dcmtk
}


# VTK Libraries

VTKLIBDIR = $$(VTKLIBDIR)
isEmpty(VTKLIBDIR){
    VTKLIBDIR = $$SDK_INSTALL_PREFIX/$$LIB64
}
VTKINCLUDEDIR = $$(VTKINCLUDEDIR)
isEmpty(VTKINCLUDEDIR){
    VTKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/vtk-8.2
}


# ITK Libraries

ITKLIBDIR = $$(ITKLIBDIR)
isEmpty(ITKLIBDIR){
    ITKLIBDIR = $$SDK_INSTALL_PREFIX/lib
}
ITKINCLUDEDIR = $$(ITKINCLUDEDIR)
isEmpty(ITKINCLUDEDIR){
    ITKINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/ITK-5.0
}

# GDCM Libraries

GDCMLIBDIR = $$(GDCMLIBDIR)
isEmpty(GDCMLIBDIR){
    GDCMLIBDIR = $$SDK_INSTALL_PREFIX/lib
}
GDCMINCLUDEDIR = $$(GDCMINCLUDEDIR)
isEmpty(GDCMINCLUDEDIR){
    GDCMINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/gdcm-3.0
}

# Threadweaver libraries

THREADWEAVERLIBDIR = $$(THREADWEAVERLIBDIR)
isEmpty(THREADWEAVERLIBDIR){
    THREADWEAVERLIBDIR = $$SDK_INSTALL_PREFIX/$$LIB64
}
THREADWEAVERINCLUDEDIR = $$(THREADWEAVERINCLUDEDIR)
isEmpty(THREADWEAVERINCLUDEDIR){
    THREADWEAVERINCLUDEDIR = $$SDK_INSTALL_PREFIX/include/KF5
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
