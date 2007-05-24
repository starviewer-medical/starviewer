include(defaultdirectories.inc)

INCLUDEPATH += $${VTKINCLUDEDIR}

LIBS += -L$${VTKLIBDIR} \
        -lvtkCommon \
        -lvtkGenericFiltering \
        -lvtkGraphics \
        -lvtkHybrid \
        -lvtkImaging \
        -lvtkRendering \
        -lvtkVolumeRendering \
        -lvtksys \
        -lQVTK \
        -lvtkWidgets
 
#        -lvtkexoIIc \
#        -lvtkNetCDF \
#        -lvtkMPEG2Encode \
#        -lvtkParallel 
#        -lvtkDICOMParser \
#        -lvtkFiltering \
#        -lvtkftgl \
#        -lvtkIO \        
#	 -lvtkjpeg \
#	 -lvtkpng \
#	 -lvtktiff \
#	 -lvtkzlib \
#	 -lvtkexpat \
#	 -lvtkfreetype \