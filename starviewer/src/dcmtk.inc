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
    -lwrap \
    -ldcmnet \
    -ldcmdata \
    -ldcmimage \
    -ldcmimgle \
    -lofstd \
    -ldcmsr \
    -lssl \
    -ldcmqrdb \
    -ldcmtls \
    -ldcmdsig \
    -ldcmpstat \
    -ldcmjpeg \
    -lijg8 \ 
    -lijg12 \ 
    -lijg16 

unix {
    DEFINES += HAVE_CONFIG_H
}