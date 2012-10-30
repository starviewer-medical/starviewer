# Configuraci� gen�rica de core, interfaces, inputoutput i main

CONFIG += warn_on \
          thread \
          qt \
          stl
          
win32-msvc2010 {
	OBJECTS_DIR = ../../tmp/obj/$${TARGET}
	}
	else {
	OBJECTS_DIR = ../../tmp/obj
	}

UI_DIR = ../../tmp/ui
MOC_DIR = ../../tmp/moc
RCC_DIR = ../../tmp/rcc
