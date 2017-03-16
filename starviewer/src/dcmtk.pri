include(defaultdirectories.pri)

INCLUDEPATH += $${DCMTKINCLUDEDIR} \
    $${DCMTKINCLUDEDIR}/../ \
    $${DCMTKINCLUDEDIR}/dcmdata/ \
    $${DCMTKINCLUDEDIR}/dcmimgle/ \
    $${DCMTKINCLUDEDIR}/dcmnet/ \
    $${DCMTKINCLUDEDIR}/dcmsign/ \
    $${DCMTKINCLUDEDIR}/ofstd/ \
    $${DCMTKINCLUDEDIR}/oflog/ \
    $${DCMTKINCLUDEDIR}/dcmimage/ \
    $${DCMTKINCLUDEDIR}/config \
    $${DCMTKINCLUDEDIR}/dcmpstat \
    $${DCMTKINCLUDEDIR}/dcmsr \
    $${DCMTKINCLUDEDIR}/dcmjpeg

LIBS += -L$${DCMTKLIBDIR} \
    -ldcmpstat \
    -ldcmnet \
    -ldcmdata \
    -ldcmimgle \
    -ldcmimage \
    -ldcmsr \
    -ldcmqrdb \
    -ldcmtls \
    -ldcmdsig \
    -ldcmjpeg \
    -lijg8 \ 
    -lijg12 \ 
    -lijg16 \
    -loflog \
    -lofstd

unix {
    DEFINES += HAVE_CONFIG_H
    LIBS += -lssl -lz
}

unix:!macx {
    LIBS += -lwrap
}

win32 {
    LIBS += -lNetAPI32 -lWSock32 -lSnmpAPI
    LIBS -= -lxml2
}
