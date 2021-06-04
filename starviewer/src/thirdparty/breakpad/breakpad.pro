# Compilem llibreries breakpad
#

include(../../compilationtype.pri)

CONFIG += debug_and_release \
          staticlib \
          warn_off  # No volem que els warnings de la llibreria "embrutin" el núm. de warnings propis

TEMPLATE = lib

DESTDIR = ./

INCLUDEPATH += ./

OBJECTS_DIR = ../../../tmp/obj
UI_DIR = ../../../tmp/ui
MOC_DIR = ../../../tmp/moc
RCC_DIR = ../../../tmp/rcc

HEADERS += \
    common/scoped_ptr.h \
    google_breakpad/common/breakpad_types.h \
    google_breakpad/common/minidump_cpu_amd64.h \
    google_breakpad/common/minidump_cpu_arm.h \
    google_breakpad/common/minidump_cpu_arm64.h \
    google_breakpad/common/minidump_cpu_mips.h \
    google_breakpad/common/minidump_cpu_ppc.h \
    google_breakpad/common/minidump_cpu_ppc64.h \
    google_breakpad/common/minidump_cpu_sparc.h \
    google_breakpad/common/minidump_cpu_x86.h \
    google_breakpad/common/minidump_exception_linux.h \
    google_breakpad/common/minidump_exception_mac.h \
    google_breakpad/common/minidump_exception_ps3.h \
    google_breakpad/common/minidump_exception_solaris.h \
    google_breakpad/common/minidump_exception_win32.h \
    google_breakpad/common/minidump_format.h

win32 {
    HEADERS += \
        client/windows/common/ipc_protocol.h \
        client/windows/crash_generation/crash_generation_client.h \
        client/windows/handler/exception_handler.h \
        common/windows/guid_string.h \
        common/windows/string_utils-inl.h

    SOURCES += \
        client/windows/crash_generation/crash_generation_client.cc \
        client/windows/handler/exception_handler.cc \
        common/windows/guid_string.cc \
        common/windows/string_utils.cc
}

unix {
    # Yes, linux_libc_support is needed in macOS
    HEADERS += \
        client/minidump_file_writer.h \
        client/minidump_file_writer-inl.h \
        common/convert_UTF.h \
        common/linux/linux_libc_support.h \
        common/memory_allocator.h \
        common/string_conversion.h \
        common/using_std_string.h \
        google_breakpad/common/minidump_size.h

    SOURCES += \
        client/minidump_file_writer.cc \
        common/convert_UTF.c \
        common/linux/linux_libc_support.cc \
        common/string_conversion.cc
}

macx {
    HEADERS += \
        client/mac/crash_generation/client_info.h \
        client/mac/crash_generation/crash_generation_client.h \
        client/mac/crash_generation/crash_generation_server.h \
        client/mac/handler/breakpad_nlist_64.h \
        client/mac/handler/dynamic_images.h \
        client/mac/handler/exception_handler.h \
        client/mac/handler/mach_vm_compat.h \
        client/mac/handler/minidump_generator.h \
        client/mac/handler/ucontext_compat.h \
        common/mac/bootstrap_compat.h \
        common/mac/byteswap.h \
        common/mac/file_id.h \
        common/mac/MachIPC.h \
        common/mac/macho_id.h \
        common/mac/macho_utilities.h \
        common/mac/macho_walker.h \
        common/mac/scoped_task_suspend-inl.h \
        common/mac/string_utilities.h \
        common/md5.h

    SOURCES += \
        client/mac/crash_generation/crash_generation_client.cc \
        client/mac/crash_generation/crash_generation_server.cc \
        client/mac/handler/breakpad_nlist_64.cc \
        client/mac/handler/dynamic_images.cc \
        client/mac/handler/exception_handler.cc \
        client/mac/handler/minidump_generator.cc \
        common/mac/bootstrap_compat.cc \
        common/mac/file_id.cc \
        common/mac/macho_id.cc \
        common/mac/macho_utilities.cc \
        common/mac/macho_walker.cc \
        common/mac/string_utilities.cc \
        common/md5.cc

    OBJECTIVE_SOURCES += \
        common/mac/MachIPC.mm
}

linux {
    HEADERS += \
        client/linux/crash_generation/crash_generation_client.h \
        client/linux/dump_writer_common/mapping_info.h \
        client/linux/dump_writer_common/raw_context_cpu.h \
        client/linux/dump_writer_common/thread_info.h \
        client/linux/dump_writer_common/ucontext_reader.h \
        client/linux/handler/exception_handler.h \
        client/linux/handler/microdump_extra_info.h \
        client/linux/handler/minidump_descriptor.h \
        client/linux/log/log.h \
        client/linux/microdump_writer/microdump_writer.h \
        client/linux/minidump_writer/cpu_set.h \
        client/linux/minidump_writer/directory_reader.h \
        client/linux/minidump_writer/line_reader.h \
        client/linux/minidump_writer/linux_dumper.h \
        client/linux/minidump_writer/linux_ptrace_dumper.h \
        client/linux/minidump_writer/minidump_writer.h \
        client/linux/minidump_writer/proc_cpuinfo_reader.h \
        common/basictypes.h \
        common/linux/eintr_wrapper.h \
        common/linux/elf_gnu_compat.h \
        common/linux/elfutils.h \
        common/linux/elfutils-inl.h \
        common/linux/file_id.h \
        common/linux/guid_creator.h \
        common/linux/ignore_ret.h \
        common/linux/memory_mapped_file.h \
        common/linux/safe_readlink.h \
        common/memory_range.h \
        common/minidump_type_helper.h \
        third_party/lss/linux_syscall_support.h

    SOURCES += \
        client/linux/crash_generation/crash_generation_client.cc \
        client/linux/dump_writer_common/thread_info.cc \
        client/linux/dump_writer_common/ucontext_reader.cc \
        client/linux/handler/exception_handler.cc \
        client/linux/handler/minidump_descriptor.cc \
        client/linux/log/log.cc \
        client/linux/microdump_writer/microdump_writer.cc \
        client/linux/minidump_writer/linux_dumper.cc \
        client/linux/minidump_writer/linux_ptrace_dumper.cc \
        client/linux/minidump_writer/minidump_writer.cc \
        common/linux/elfutils.cc \
        common/linux/file_id.cc \
        common/linux/guid_creator.cc \
        common/linux/memory_mapped_file.cc \
        common/linux/safe_readlink.cc
}
