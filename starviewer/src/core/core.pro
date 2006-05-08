# Fitxer generat pel gestor de qmake de kdevelop. 
# ------------------------------------------- 
# Subdirectori relatiu al directori principal del projecte: ./src/core
# L'objectiu és una biblioteca:  

HEADERS += extensionfactory.h \
           genericfactory.h \
           singleton.h \
           genericsingletonfactoryregister.h \
           extensionfactoryregister.h 
TEMPLATE = lib
CONFIG += debug \
warn_on \
thread \
staticlib \
stl
OBJECTS_DIR = ../../tmp/obj
UI_DIR = ../../tmp/ui
MOC_DIR = ../../tmp/moc
