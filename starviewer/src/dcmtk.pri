include(defaultdirectories.pri)

INCLUDEPATH += \
    $${DCMTKINCLUDEDIR} \
    $${DCMTKINCLUDEDIR}/.. \
    $${DCMTKINCLUDEDIR}/config \
    $${DCMTKINCLUDEDIR}/dcmdata \
    $${DCMTKINCLUDEDIR}/dcmimage \
    $${DCMTKINCLUDEDIR}/dcmimgle \
    $${DCMTKINCLUDEDIR}/dcmjpeg \
    $${DCMTKINCLUDEDIR}/dcmnet \
    $${DCMTKINCLUDEDIR}/dcmpstat \
    $${DCMTKINCLUDEDIR}/ofstd

LIBS += \
    -L$${DCMTKLIBDIR} \
    -ldcmimage \
    -ldcmimgle \
    -ldcmjpeg \
    -ldcmpstat \
    -ldcmnet \
    -ldcmdata \
    -lijg8 \
    -lijg12 \
    -lijg16 \
    -loflog \
    -lofstd \
    -lz

macx:LIBS += -lcharset -liconv

#win32:LIBS += -lNetAPI32 -lWSock32 -lSnmpAPI
