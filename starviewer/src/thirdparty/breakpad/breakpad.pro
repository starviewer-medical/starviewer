# Compilem llibreries breakpad
#

include(../../compilationtype.inc)

CONFIG += debug_and_release \
          staticlib \
          warn_off  # No volem que els warnings de la llibreria "embrutin" el núm. de warnings propis

TEMPLATE = lib

DESTDIR = ./

INCLUDEPATH += ./

win32-msvc2010 {
    OBJECTS_DIR = ../../../tmp/obj/$${TARGET}
    }
    else {
    OBJECTS_DIR = ../../../tmp/obj
    }
UI_DIR = ../../../tmp/ui
MOC_DIR = ../../../tmp/moc
RCC_DIR = ../../../tmp/rcc

include(../../breakpad.inc)

# Problema de suport de breakpad per linux 64bits (idem a ../../breakpad.inc)
unix:!linux_x86_64 {

    HEADERS += client/minidump_file_writer.h \
               client/minidump_file_writer-inl.h \
               common/convert_UTF.h \
               common/string_conversion.h \
               google_breakpad/common/breakpad_types.h \
               google_breakpad/common/minidump_format.h \
               google_breakpad/common/minidump_size.h \
               processor/scoped_ptr.h \

    SOURCES += common/convert_UTF.c \
               client/minidump_file_writer.cc \
               common/string_conversion.cc \
}

macx {

    HEADERS += client/mac/handler/breakpad_exc_server.h \
               client/mac/handler/breakpad_nlist_64.h \
               client/mac/handler/dynamic_images.h \
               client/mac/handler/exception_handler.h \
               client/mac/handler/minidump_generator.h \
               client/mac/handler/protected_memory_allocator.h \
               common/mac/file_id.h \
               common/mac/macho_id.h \
               common/mac/macho_utilities.h \
               common/mac/macho_walker.h \
               common/mac/string_utilities.h

    SOURCES += client/mac/handler/exception_handler.cc \
               client/mac/handler/minidump_generator.cc \
               client/mac/handler/breakpad_exc_server.c \
               common/mac/file_id.cc \
               common/mac/macho_id.cc \
               common/mac/macho_walker.cc \
               client/mac/handler/dynamic_images.cc \
               client/mac/handler/protected_memory_allocator.cc \
               common/mac/macho_utilities.cc \
               common/mac/string_utilities.cc \
               client/mac/handler/breakpad_nlist_64.cc
}

win32 {

    HEADERS += client/windows/common/auto_critical_section.h \
               client/windows/common/ipc_protocol.h \
               client/windows/crash_generation/crash_generation_client.h \
               client/windows/crash_generation/minidump_generator.h \
               client/windows/handler/exception_handler.h \
               common/windows/guid_string.h \
               common/windows/string_utils-inl.h

    SOURCES += client/windows/handler/exception_handler.cc \
               common/windows/guid_string.cc \
               client/windows/crash_generation/crash_generation_client.cc \
               client/windows/crash_generation/minidump_generator.cc \
               common/windows/string_utils.cc
}

linux*:!linux_x86_64 {

    HEADERS += common/md5.h \
               client/linux/handler/exception_handler.h \
               client/linux/handler/linux_thread.h \
               client/linux/handler/minidump_generator.h \
               common/linux/file_id.h \
               common/linux/guid_creator.h
    SOURCES += common/md5.c \
               client/linux/handler/exception_handler.cc \
               client/linux/handler/linux_thread.cc \
               client/linux/handler/minidump_generator.cc \
               common/linux/file_id.cc \
               common/linux/guid_creator.cc
}

