# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/registrationframework

FORMS += qregistrationframeworkextensionbase.ui 
HEADERS += qregistrationframeworkextension.h \
           registrationframeworkextensionmediator.h 
SOURCES += qregistrationframeworkextension.cpp \
           registrationframeworkextensionmediator.cpp 


RESOURCES += registrationframework.qrc

include(../../basicconfextensions.inc)
