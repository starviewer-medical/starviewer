# Directoris per defecte de les diferents llibreries. Si vols que siguin uns altres, simplement has de declarar
# com a variables de sistema les que vulguis substituir. Ex.: export ITKLIBDIR=/usr/lib64/InsightToolkit

# DCMTK Libraries

DCMTKLIBDIR = $$(DCMTKLIBDIR)
isEmpty(DCMTKLIBDIR){
    DCMTKLIBDIR = /usr/lib/dcmtk
}
DCMTKINCLUDEDIR = $$(DCMTKINCLUDEDIR)
isEmpty(DCMTKINCLUDEDIR){
    DCMTKINCLUDEDIR = /usr/include/dcmtk
}


# VTK Libraries

VTKLIBDIR = $$(VTKLIBDIR)
isEmpty(VTKLIBDIR){
    VTKLIBDIR = /usr/lib
}
VTKINCLUDEDIR = $$(VTKINCLUDEDIR)
isEmpty(VTKINCLUDEDIR){
    VTKINCLUDEDIR = /usr/include/vtk-5.0
}


# ITK Libraries

ITKLIBDIR = $$(ITKLIBDIR)
isEmpty(ITKLIBDIR){
    ITKLIBDIR = /usr/lib/InsightToolkit
}
ITKINCLUDEDIR = $$(ITKINCLUDEDIR)
isEmpty(ITKINCLUDEDIR){
    ITKINCLUDEDIR = /usr/include/InsightToolkit
}
