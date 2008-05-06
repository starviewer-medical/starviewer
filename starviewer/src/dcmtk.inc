include(defaultdirectories.inc)

INCLUDEPATH += $${DCMTKINCLUDEDIR} \
    $${DCMTKINCLUDEDIR}/dcmdata/ \
    $${DCMTKINCLUDEDIR}/dcmimgle/ \
    $${DCMTKINCLUDEDIR}/dcmnet/ \
    $${DCMTKINCLUDEDIR}/dcmsign/ \
    $${DCMTKINCLUDEDIR}/ofstd/ \
    $${DCMTKINCLUDEDIR}/dcmimage/ \
    $${DCMTKINCLUDEDIR}/config \
    $${DCMTKINCLUDEDIR}/dcmpstat \
    $${DCMTKINCLUDEDIR}/dcmsr \
    $${DCMTKINCLUDEDIR}/dcmjpeg

win32 {
INCLUDEPATH += $${DCMTKINCLUDEDIR}/../
LIBS += -lNetAPI32 -lWSock32 -lSnmpAPI

INCLUDEPATH += C:\sqlite3
LIBS += -LC:\sqlite3
}

LIBS += -L$${DCMTKLIBDIR} \
    -lsqlite3 \
    -ldcmnet \
    -ldcmdata \
    -lofstd \
    -ldcmimgle \
    -ldcmimage \
    -ldcmsr \
    -ldcmqrdb \
    -ldcmtls \
    -ldcmdsig \
    -ldcmpstat \
    -lijg8 \ 
    -lijg12 \ 
    -lijg16 \
    -ldcmjpeg
unix:LIBS += -lssl

unix {
    DEFINES += HAVE_CONFIG_H
}