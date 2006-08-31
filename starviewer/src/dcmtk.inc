INCLUDEPATH += /usr/include/dcmtk/ \
	/usr/include/dcmtk/dcmdata/ \
	/usr/include/dcmtk/dcmimgle/ \
	/usr/include/dcmtk/dcmnet/ \
	/usr/include/dcmtk/dcmsign/ \
	/usr/include/dcmtk/ofstd/ \
	/usr/include/dcmtk/dcmimage/ \
	/usr/include/dcmtk/config \
	/usr/include/dcmtk/dcmjpeg

LIBS += -L/usr/lib/dcmtk/ \
	-lsqlite3 \
	-ldcmnet \
	-ldcmdata \
	-ldcmimage \
	-ldcmimgle \
	-lofstd \
	-ldcmjpeg
    