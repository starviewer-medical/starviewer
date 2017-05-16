# Compilem llibreries breakpad
#

include(../../compilationtype.pri)

CONFIG += debug_and_release \
          staticlib \
          warn_off  # No volem que els warnings de la llibreria "embrutin" el núm. de warnings propis

TEMPLATE = lib

DESTDIR = ./

INCLUDEPATH += ./

win32-msvc2013 {
    OBJECTS_DIR = ../../../tmp/obj/$${TARGET}
    }
    else {
    OBJECTS_DIR = ../../../tmp/obj
    }
UI_DIR = ../../../tmp/ui
MOC_DIR = ../../../tmp/moc
RCC_DIR = ../../../tmp/rcc

include(../../breakpad.pri)

HEADERS += common/scoped_ptr.h

unix {

    HEADERS += client/minidump_file_writer.h \
               client/minidump_file_writer-inl.h \
               common/convert_UTF.h \
               common/string_conversion.h \
               google_breakpad/common/breakpad_types.h \
               google_breakpad/common/minidump_format.h \
               google_breakpad/common/minidump_size.h \
               common/linux/linux_libc_support.h \
               common/using_std_string.h \
               common/memory.h \
               common/md5.h

    SOURCES += common/convert_UTF.c \
               client/minidump_file_writer.cc \
               common/string_conversion.cc \
               common/linux/linux_libc_support.cc \
               common/md5.cc
}

macx {

    HEADERS += client/mac/handler/ucontext_compat.h \
               client/mac/handler/breakpad_nlist_64.h \
               client/mac/handler/dynamic_images.h \
               client/mac/handler/exception_handler.h \
               client/mac/handler/minidump_generator.h \
               client/mac/handler/protected_memory_allocator.h \
               client/mac/handler/mach_vm_compat.h \
               client/mac/crash_generation/crash_generation_client.h \
               client/mac/crash_generation/crash_generation_server.h \
               client/mac/crash_generation/client_info.h \
               common/mac/file_id.h \
               common/mac/macho_id.h \
               common/mac/macho_utilities.h \
               common/mac/macho_walker.h \
               common/mac/string_utilities.h \
               common/mac/MachIPC.h \
               common/mac/bootstrap_compat.h \
               common/mac/scoped_task_suspend-inl.h \
               common/mac/byteswap.h

    SOURCES += client/mac/handler/exception_handler.cc \
               client/mac/handler/minidump_generator.cc \
               client/mac/crash_generation/crash_generation_client.cc \
               client/mac/crash_generation/crash_generation_server.cc \
               common/mac/file_id.cc \
               common/mac/macho_id.cc \
               common/mac/macho_walker.cc \
               client/mac/handler/dynamic_images.cc \
               client/mac/handler/protected_memory_allocator.cc \
               common/mac/macho_utilities.cc \
               common/mac/string_utilities.cc \
               client/mac/handler/breakpad_nlist_64.cc \
               common/mac/bootstrap_compat.cc

    OBJECTIVE_SOURCES += common/mac/MachIPC.mm
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

linux* {

    HEADERS += client/linux/handler/exception_handler.h \
               client/linux/handler/minidump_descriptor.h \
               client/linux/handler/microdump_extra_info.h \
               client/linux/crash_generation/crash_generation_client.h \
               client/linux/minidump_writer/minidump_writer.h \
               client/linux/minidump_writer/linux_dumper.h \
               client/linux/minidump_writer/line_reader.h \
               client/linux/minidump_writer/cpu_set.h \
               client/linux/minidump_writer/linux_ptrace_dumper.h \
               client/linux/minidump_writer/proc_cpuinfo_reader.h \
               client/linux/minidump_writer/directory_reader.h \
               client/linux/dump_writer_common/mapping_info.h \
               client/linux/dump_writer_common/thread_info.h \
               client/linux/dump_writer_common/raw_context_cpu.h \
               client/linux/dump_writer_common/ucontext_reader.h \
               client/linux/log/log.h \
               client/linux/microdump_writer/microdump_writer.h \
               common/linux/file_id.h \
               common/linux/guid_creator.h \
               common/linux/eintr_wrapper.h \
               common/linux/ignore_ret.h \
               common/linux/elfutils.h \
               common/linux/elfutils-inl.h \
               common/linux/memory_mapped_file.h \
               common/linux/safe_readlink.h \
               common/linux/elf_gnu_compat.h \
               common/basictypes.h \
               common/memory_range.h \
               common/minidump_type_helper.h \
               third_party/lss/linux_syscall_support.h
    SOURCES += client/linux/handler/exception_handler.cc \
               client/linux/handler/minidump_descriptor.cc \
               client/linux/crash_generation/crash_generation_client.cc \
               client/linux/minidump_writer/minidump_writer.cc \
               client/linux/minidump_writer/linux_dumper.cc \
               client/linux/minidump_writer/linux_ptrace_dumper.cc \
               client/linux/dump_writer_common/thread_info.cc \
               client/linux/dump_writer_common/ucontext_reader.cc \
               client/linux/log/log.cc \
               client/linux/microdump_writer/microdump_writer.cc \
               common/linux/file_id.cc \
               common/linux/guid_creator.cc \
               common/linux/elfutils.cc \
               common/linux/memory_mapped_file.cc \
               common/linux/safe_readlink.cc
}

