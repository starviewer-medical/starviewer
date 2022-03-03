# Notes:
# - STARVIEWER_CE shall not be used with STARVIEWER_BETA or STARVIEWER_LITE.
# - STARVIEWER_CE should be combined with SEND_HOST_DATA and USE_QTCONF.
# - USE_QTCONF should be disabled for development, otherwise Qt plugins are not found in Qt folder.
# - USE_CRASH_REPORTER and USE_PACS_COMPRESSION should always be left ON under normal circumstances.
# - USE_CUDA is totally untested, probably would not work without changes.
# - STARVIEWER_BUILD_DEBUG_WITH_RELEASE_LIBS can be used when building in debug mode with MSVC to link against release libraries.

option(STARVIEWER_BETA "Build Starviewer with a beta warning." OFF)
option(STARVIEWER_CE "Build Starviewer with CE marking. Requires the deployment repository." OFF)
option(STARVIEWER_LITE "Build Starviewer Lite instead of full Starviewer." OFF)

option(SEND_HOST_DATA "Starviewer will send hashed MAC and work group id when checking for updates." OFF)
option(USE_QTCONF "Include a qt.conf file in resources. Enable if you intend to install Starviewer." OFF)
option(USE_SPECIFIC_HANGING_PROTOCOLS "Include institution-specific hanging protocols from the deployment repository." OFF)

option(USE_CRASH_REPORTER "Enable the crash reporter." ON)
option(USE_PACS_COMPRESSION "Enable lossless compression when requesting studies from PACS." ON)
#option(USE_CUDA "Use CUDA." OFF)   # untested

option(BUILD_PDF_MANUALS "Convert manuals to PDF when building." ON)

if(MSVC)
    option(STARVIEWER_BUILD_DEBUG_WITH_RELEASE_LIBS "Use release libraries with debug build of Starviewer." OFF)
endif()
