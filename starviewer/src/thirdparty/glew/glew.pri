# Integració llibreries glew.
# De moment, només preparat per una única extensió. Si es vol per més extensions cal convertir-ho en una .lib
#

HEADERS +=  $$PWD/glew.h \
            $$PWD/glew.h \
            $$PWD/glxew.h \
            $$PWD/wglew.h

SOURCES +=  $$PWD/glew.c
