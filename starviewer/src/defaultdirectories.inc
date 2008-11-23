# Directoris per defecte de les diferents llibreries. Si vols que siguin uns altres, simplement has de declarar
# com a variables de sistema les que vulguis substituir. Ex.: export ITKLIBDIR=/usr/lib64/InsightToolkit

# DCMTK Libraries

DCMTKLIBDIR = $$(DCMTKLIBDIR)
isEmpty(DCMTKLIBDIR){
    unix:DCMTKLIBDIR = /usr/lib/dcmtk
    win32:DCMTKLIBDIR = C:\dcmtk-3.5.4\lib
}
DCMTKINCLUDEDIR = $$(DCMTKINCLUDEDIR)
isEmpty(DCMTKINCLUDEDIR){
    unix:DCMTKINCLUDEDIR = /usr/include/dcmtk
    win32:DCMTKINCLUDEDIR = C:\dcmtk-3.5.4\include\dcmtk
}


# VTK Libraries

VTKLIBDIR = $$(VTKLIBDIR)
isEmpty(VTKLIBDIR){
    unix:VTKLIBDIR = /usr/lib
    win32:VTKLIBDIR = C:\vtk\vtk5.0\lib
}
VTKINCLUDEDIR = $$(VTKINCLUDEDIR)
isEmpty(VTKINCLUDEDIR){
    unix:VTKINCLUDEDIR = /usr/include/vtk-5.0
    win32:VTKINCLUDEDIR = C:\vtk\vtk5.0\include\vtk-5.0
}


# ITK Libraries

ITKLIBDIR = $$(ITKLIBDIR)
isEmpty(ITKLIBDIR){
    unix:ITKLIBDIR = /usr/lib/InsightToolkit
    win32:ITKLIBDIR = C:\InsightToolkit\lib\InsightToolkit
}
ITKINCLUDEDIR = $$(ITKINCLUDEDIR)
isEmpty(ITKINCLUDEDIR){
    unix:ITKINCLUDEDIR = /usr/include/InsightToolkit
    win32:ITKINCLUDEDIR = C:\InsightToolkit\include\InsightToolkit
}

# Log4cxx Libraries

LOG4CXXLIBDIR = $$(LOG4CXXLIBDIR)
isEmpty(LOG4CXXLIBDIR){
    unix:LOG4CXXLIBDIR = /usr/lib
    win32:LOG4CXXLIBDIR = C:\log4cxx
}
LOG4CXXINCLUDEDIR = $$(LOG4CXXINCLUDEDIR)
isEmpty(LOG4CXXINCLUDEDIR){
    unix:LOG4CXXINCLUDEDIR = /usr/include/log4cxx
    win32:LOG4CXXINCLUDEDIR = C:\log4cxx\include
}
