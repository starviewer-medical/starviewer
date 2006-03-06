
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
        #-lvtkParallel \
        #-lvtkexpat \
        -lvtkfreetype \
        -lvtkftgl \
        #-lvtkjpeg \
        #-lvtkpng \
        -lvtksys \
        #-lvtktiff \
        #-lvtkzlib \
        #-lQVTK \
        -lvtkWidgets \
        -lvtkexoIIc \
        -lvtkNetCDF \
        -lvtkMPEG2Encode
