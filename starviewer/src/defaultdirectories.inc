# Directoris per defecte de les diferents llibreries. Si vols que siguin uns altres, simplement has de declarar
# com a variables de sistema les que vulguis substituir. Ex.: export ITKLIBDIR=/usr/lib64/InsightToolkit

# DCMTK Libraries

DCMTKLIBDIR = $$(DCMTKLIBDIR)
isEmpty(DCMTKLIBDIR){
    unix:DCMTKLIBDIR = /usr/lib/dcmtk
    macx:DCMTKLIBDIR = /usr/local/dicom/lib
    win32:DCMTKLIBDIR = C:\dcmtk-3.5.4\lib
}
DCMTKINCLUDEDIR = $$(DCMTKINCLUDEDIR)
isEmpty(DCMTKINCLUDEDIR){
    unix:DCMTKINCLUDEDIR = /usr/include/dcmtk
    macx:DCMTKINCLUDEDIR = /usr/local/dicom/include/dcmtk
    win32:DCMTKINCLUDEDIR = C:\dcmtk-3.5.4\include\dcmtk
}


# VTK Libraries

VTKLIBDIR = $$(VTKLIBDIR)
isEmpty(VTKLIBDIR){
    unix:VTKLIBDIR = /usr/lib
    macx:VTKLIBDIR = /usr/local/lib/vtk-5.6
    win32:VTKLIBDIR = C:\vtk\vtk5.6\lib\vtk-5.6
}
VTKINCLUDEDIR = $$(VTKINCLUDEDIR)
isEmpty(VTKINCLUDEDIR){
    unix:VTKINCLUDEDIR = /usr/include/vtk-5.6
    macx:VTKINCLUDEDIR = /usr/local/include/vtk-5.6
    win32:VTKINCLUDEDIR = C:\vtk\vtk5.6\include\vtk-5.6
}


# ITK Libraries

ITKLIBDIR = $$(ITKLIBDIR)
isEmpty(ITKLIBDIR){
    unix:ITKLIBDIR = /usr/lib/InsightToolkit
    macx:ITKLIBDIR = /usr/local/lib/InsightToolkit
    win32:ITKLIBDIR = C:\InsightToolkit\lib\InsightToolkit
}
ITKINCLUDEDIR = $$(ITKINCLUDEDIR)
isEmpty(ITKINCLUDEDIR){
    unix:ITKINCLUDEDIR = /usr/include/InsightToolkit
    macx:ITKINCLUDEDIR = /usr/local/include/InsightToolkit
    win32:ITKINCLUDEDIR = C:\InsightToolkit\include\InsightToolkit
}

# GDCM Libraries

GDCMLIBDIR = $$(GDCMLIBDIR)
isEmpty(GDCMLIBDIR){
    unix:GDCMLIBDIR = /usr/lib/gdcm
    macx:GDCMLIBDIR = /usr/local/lib/gdcm
    win32:GDCMLIBDIR = C:\gdcm\GDCM-2.0\lib
}
GDCMINCLUDEDIR = $$(GDCMINCLUDEDIR)
isEmpty(GDCMINCLUDEDIR){
    unix:GDCMINCLUDEDIR = /usr/include/gdcm
    macx:GDCMINCLUDEDIR = /usr/local/include/gdcm
    win32:GDCMINCLUDEDIR = C:\gdcm\GDCM-2.0\include\gdcm-2.0
}

# Log4cxx Libraries

LOG4CXXLIBDIR = $$(LOG4CXXLIBDIR)
isEmpty(LOG4CXXLIBDIR){
    unix:LOG4CXXLIBDIR = /usr/lib
    macx:LOG4CXXLIBDIR = /usr/local/lib/log4cxx
    win32:LOG4CXXLIBDIR = C:\log4cxx
}
LOG4CXXINCLUDEDIR = $$(LOG4CXXINCLUDEDIR)
isEmpty(LOG4CXXINCLUDEDIR){
    unix:LOG4CXXINCLUDEDIR = /usr/include/log4cxx
    macx:LOG4CXXINCLUDEDIR = /usr/local/include/log4cxx
    win32:LOG4CXXINCLUDEDIR = C:\log4cxx\include
}

# Threadweaver libraries

THREADWEAVERLIBDIR = $$(THREADWEAVERLIBDIR)
isEmpty(THREADWEAVERLIBDIR){
    unix:THREADWEAVERLIBDIR = /usr/lib
    macx:THREADWEAVERLIBDIR = /usr/local/lib/threadweaver
    win32:THREADWEAVERLIBDIR = C:\kde\lib
}
THREADWEAVERINCLUDEDIR = $$(THREADWEAVERINCLUDEDIR)
isEmpty(THREADWEAVERINCLUDEDIR){
    unix:THREADWEAVERINCLUDEDIR = /usr/include
    macx:THREADWEAVERINCLUDEDIR = /usr/local/include
    win32:THREADWEAVERINCLUDEDIR = C:\KDE\include
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
