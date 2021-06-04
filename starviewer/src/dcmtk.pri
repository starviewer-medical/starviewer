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
    -lofstd

unix:LIBS += -lz
macx:LIBS += -lcharset -liconv
win32:LIBS += -lWS2_32 -lNetAPI32 -llibiconv_o -lcharset_o
