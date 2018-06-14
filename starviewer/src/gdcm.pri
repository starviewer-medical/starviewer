include(defaultdirectories.pri)

INCLUDEPATH += $${GDCMINCLUDEDIR}

LIBS += -L$${GDCMLIBDIR} \
        -lgdcmcharls \
        -lgdcmCommon \
        -lgdcmDICT \
        -lgdcmDSED \
        -lgdcmexpat \
        -lgdcmIOD \
        -lgdcmjpeg8 \
        -lgdcmjpeg12 \
        -lgdcmjpeg16 \
        -lgdcmMSFF \
        -lvtkgdcm

win32 {
    LIBS += -lgdcmgetopt -lgdcmzlib
}
