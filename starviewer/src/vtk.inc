
INCLUDEPATH += $(VTKINCLUDEDIR) 

LIBS += -L$(VTKLIBDIR) \
        -lvtkCommon \
        -lvtkDICOMParser \
        -lvtkFiltering \
        -lvtkGenericFiltering \
        -lvtkGraphics \
        -lvtkHybrid \
        -lvtkIO \
        -lvtkImaging \
        -lvtkRendering \
        -lvtkVolumeRendering \
        -lvtkftgl \
        -lvtksys \
        -lQVTK \
        -lvtkWidgets \
        -lvtkexoIIc \
        -lvtkNetCDF \
        -lvtkMPEG2Encode
