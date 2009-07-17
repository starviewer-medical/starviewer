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
    -ldcmjpeg \
    -lijg8 \ 
    -lijg12 \ 
    -lijg16 \
    -lxml2

unix {
    DEFINES += HAVE_CONFIG_H
    LIBS += -lssl \
            -lwrap
}

macx {
    INCLUDEPATH += $${DCMTKINCLUDEDIR}/../
}

win32 {
    INCLUDEPATH += $${DCMTKINCLUDEDIR}/../
    LIBS += -lNetAPI32 -lWSock32 -lSnmpAPI

    INCLUDEPATH += C:\sqlite3
    LIBS += -LC:\sqlite3
    LIBS -= -lxml2
}

