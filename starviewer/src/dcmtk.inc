INCLUDEPATH += /usr/include/dcmtk/ \
	/usr/include/dcmtk/dcmdata/ \
	/usr/include/dcmtk/dcmimgle/ \
	/usr/include/dcmtk/dcmnet/ \
	/usr/include/dcmtk/dcmsign/ \
	/usr/include/dcmtk/ofstd/ \
	/usr/include/dcmtk/dcmimage/

LIBS += -L/usr/lib/dcmtk/ \
	-lsqlite \
	-ldcmnet \
	-ldcmdata \
	-ldcmimage \
	-ldcmimgle \
	-lofstd 
    