# Compilem llibreries breakpad i configurem crash handler.
#

SOURCES += crashhandler.cpp
HEADERS += crashhandler.h


mac {
    DEFINES += STARVIEWER_CRASH_REPORTER_EXE=\\\""../../../"$${TARGET_STARVIEWER_CRASH_REPORTER}\\\"
}
linux {
    DEFINES += STARVIEWER_CRASH_REPORTER_EXE=\\\"$${TARGET_STARVIEWER_CRASH_REPORTER}\\\"
}
win32 {
    DEFINES += STARVIEWER_CRASH_REPORTER_EXE=\\\"$${TARGET_STARVIEWER_CRASH_REPORTER}.exe\\\"
}

INCLUDEPATH += src_breakpad

#Fitxers comuns en les diferents plataformes unix (linux, macx...)

unix {
	HEADERS += src_breakpad/client/minidump_file_writer.h \
               src_breakpad/client/minidump_file_writer-inl.h \
               src_breakpad/common/convert_UTF.h \
               src_breakpad/common/string_conversion.h \
               src_breakpad/google_breakpad/common/breakpad_types.h \
               src_breakpad/google_breakpad/common/minidump_format.h \
               src_breakpad/google_breakpad/common/minidump_size.h \
               src_breakpad/processor/scoped_ptr.h \

    SOURCES += src_breakpad/common/convert_UTF.c \
               src_breakpad/client/minidump_file_writer.cc \
			   src_breakpad/common/string_conversion.cc \
}

mac* {
    
    HEADERS += src_breakpad/client/mac/handler/breakpad_exc_server.h \
               src_breakpad/client/mac/handler/breakpad_nlist_64.h \
               src_breakpad/client/mac/handler/dynamic_images.h \
               src_breakpad/client/mac/handler/exception_handler.h \
               src_breakpad/client/mac/handler/minidump_generator.h \
               src_breakpad/client/mac/handler/protected_memory_allocator.h \
               src_breakpad/common/mac/file_id.h \
               src_breakpad/common/mac/macho_id.h \
               src_breakpad/common/mac/macho_utilities.h \
               src_breakpad/common/mac/macho_walker.h \
               src_breakpad/common/mac/string_utilities.h
    
    SOURCES += src_breakpad/client/mac/handler/exception_handler.cc \
               src_breakpad/client/mac/handler/minidump_generator.cc \
               src_breakpad/client/mac/handler/breakpad_exc_server.c \
               src_breakpad/common/mac/file_id.cc \
               src_breakpad/common/mac/macho_id.cc \
               src_breakpad/common/mac/macho_walker.cc \
               src_breakpad/client/mac/handler/dynamic_images.cc \
               src_breakpad/client/mac/handler/protected_memory_allocator.cc \
               src_breakpad/common/mac/macho_utilities.cc \
               src_breakpad/common/mac/string_utilities.cc

    LIBS += -lcrypto
}

win32 {
    
    HEADERS += src_breakpad/client/windows/common/auto_critical_section.h \
               src_breakpad/client/windows/common/ipc_protocol.h \
               src_breakpad/client/windows/crash_generation/crash_generation_client.h \
               src_breakpad/client/windows/crash_generation/minidump_generator.h \
               src_breakpad/client/windows/handler/exception_handler.h \
               src_breakpad/common/windows/guid_string.h \
               src_breakpad/common/windows/string_utils-inl.h

    SOURCES += src_breakpad/client/windows/handler/exception_handler.cc \
               src_breakpad/common/windows/guid_string.cc \
               src_breakpad/client/windows/crash_generation/crash_generation_client.cc \
               src_breakpad/client/windows/crash_generation/minidump_generator.cc \
               src_breakpad/common/windows/string_utils.cc
}

linux* {
    
    HEADERS += src_breakpad/common/md5.h \ 
               src_breakpad/client/linux/handler/exception_handler.h \
               src_breakpad/client/linux/handler/linux_thread.h \
               src_breakpad/client/linux/handler/minidump_generator.h \
               src_breakpad/common/linux/file_id.h \
               src_breakpad/common/linux/guid_creator.h
    SOURCES += src_breakpad/common/md5.c \ 
               src_breakpad/client/linux/handler/exception_handler.cc \
               src_breakpad/client/linux/handler/linux_thread.cc \
               src_breakpad/client/linux/handler/minidump_generator.cc \
               src_breakpad/common/linux/file_id.cc \
               src_breakpad/common/linux/guid_creator.cc
}
