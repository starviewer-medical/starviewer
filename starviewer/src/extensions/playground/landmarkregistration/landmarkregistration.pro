# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/landmarkregistration
# L'objectiu � una biblioteca:  

FORMS += qlandmarkregistrationextensionbase.ui 
HEADERS += landmarkregistrator.h \
           leastsquareddistancecostfunction.h \
           qlandmarkregistrationextension.h \
           landmarkregistrationsettings.h \
           reglandmark.h \
           landmarkregistrationextensionmediator.h 
SOURCES += landmarkregistrator.cpp \
           leastsquareddistancecostfunction.cpp \
           qlandmarkregistrationextension.cpp \
           landmarkregistrationsettings.cpp \
           reglandmark.cpp \
           landmarkregistrationextensionmediator.cpp 

RESOURCES += landmarkregistration.qrc

EXTENSION_DIR = $$PWD
include(../../basicconfextensions.inc)
