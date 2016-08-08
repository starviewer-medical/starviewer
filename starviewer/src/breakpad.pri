include(defaultdirectories.pri)

INCLUDEPATH += $${BREAKPADINCLUDEDIR}

LIBS += -L$${BREAKPADLIBDIR} \
    -lbreakpad \
    -lbreakpad_client

macx {
    LIBS += -lcrypto
}

