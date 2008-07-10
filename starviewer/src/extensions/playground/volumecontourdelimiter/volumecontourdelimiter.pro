# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground/volumecontourdelimiter
# L'objectiu ÃÂÃÂ¯ÃÂÃÂ¿ÃÂÃÂ½s una biblioteca:  

FORMS +=   qvolumecontourdelimiterextensionbase.ui
HEADERS += volumecontourdelimiterextensionmediator.h \
	   qvolumecontourdelimiterextension.h \
           point.h
	   
SOURCES += volumecontourdelimiterextensionmediator.cpp \
	   qvolumecontourdelimiterextension.cpp \
           point.cpp 
	   
RESOURCES += volumecontourdelimiter.qrc

include(../../basicconfextensions.inc)
