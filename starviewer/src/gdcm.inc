include(defaultdirectories.inc)

INCLUDEPATH += $${GDCMINCLUDEDIR}

LIBS += -L$${GDCMLIBDIR} \
        -lgdcmcharls \
        -lgdcmCommon \
        -lgdcmDICT \
        -lgdcmDSED \
        -lgdcmexpat \
        -lgdcmgetopt \
        -lgdcmIOD \
        -lgdcmjpeg8 \
        -lgdcmjpeg12 \
        -lgdcmjpeg16 \
        -lgdcmMSFF \
        -lgdcmopenjpeg \
        -lgdcmzlib
