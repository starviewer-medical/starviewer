# Notes:
# - STARVIEWER_CE shall not be used with STARVIEWER_BETA or STARVIEWER_LITE.
# - CE should be combined with SEND_HOST_DATA and USE_QTCONF.
# - USE_CRASH_REPORTER and USE_PACS_COMPRESSION should always be left ON under normal circumstances.
# - USE_CUDA is totally untested, probably would not work without changes.

option(STARVIEWER_BETA "Build Starviewer with a beta warning." OFF)
option(STARVIEWER_CE "Build Starviewer with CE marking." OFF)
option(STARVIEWER_LITE "Build Starviewer Lite instead of full Starviewer." OFF)

option(SEND_HOST_DATA "Starviewer will send hashed MAC and work group id when checking for updates." OFF)
option(USE_QTCONF "Include a qt.conf file in resources." OFF)

option(USE_CRASH_REPORTER "Enable the crash reporter." ON)
option(USE_PACS_COMPRESSION "Enable lossless compression when requesting studies from PACS." ON)
#option(USE_CUDA "Use CUDA." OFF)   # untested
