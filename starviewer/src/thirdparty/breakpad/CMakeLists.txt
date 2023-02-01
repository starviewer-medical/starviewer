set(CMAKE_AUTOMOC OFF)
set(CMAKE_AUTORCC OFF)
set(CMAKE_AUTOUIC OFF)

if(WIN32)
    set(SOURCES
        client/windows/crash_generation/crash_generation_client.cc
        client/windows/handler/exception_handler.cc
        common/windows/guid_string.cc
        common/windows/string_utils.cc
    )
elseif(UNIX)
    set(SOURCES
        client/minidump_file_writer.cc
        common/convert_UTF.c
        common/linux/linux_libc_support.cc
        common/string_conversion.cc
    )

    if(APPLE)   # macOS
        list(APPEND SOURCES
            client/mac/crash_generation/crash_generation_client.cc
            client/mac/crash_generation/crash_generation_server.cc
            client/mac/handler/breakpad_nlist_64.cc
            client/mac/handler/dynamic_images.cc
            client/mac/handler/exception_handler.cc
            client/mac/handler/minidump_generator.cc
            common/mac/bootstrap_compat.cc
            common/mac/file_id.cc
            common/mac/macho_id.cc
            common/mac/macho_utilities.cc
            common/mac/macho_walker.cc
            common/mac/string_utilities.cc
            common/md5.cc
            common/mac/MachIPC.mm
        )
    else()      # Linux
        list(APPEND SOURCES
            client/linux/crash_generation/crash_generation_client.cc
            client/linux/dump_writer_common/thread_info.cc
            client/linux/dump_writer_common/ucontext_reader.cc
            client/linux/handler/exception_handler.cc
            client/linux/handler/minidump_descriptor.cc
            client/linux/log/log.cc
            client/linux/microdump_writer/microdump_writer.cc
            client/linux/minidump_writer/linux_dumper.cc
            client/linux/minidump_writer/linux_ptrace_dumper.cc
            client/linux/minidump_writer/minidump_writer.cc
            common/linux/elfutils.cc
            common/linux/file_id.cc
            common/linux/guid_creator.cc
            common/linux/memory_mapped_file.cc
            common/linux/safe_readlink.cc
        )
    endif()
endif()

add_library(breakpad ${SOURCES})
