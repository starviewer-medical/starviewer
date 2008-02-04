
SET (CTEST_DART_SERVER_VERSION "2")
SET (CTEST_PROJECT_NAME "Starviewer")
SET (CTEST_NIGHTLY_START_METHOD "00:00:01 CET")
SET (CTEST_COMPRESSION_SUBMISSION ON)

# Configuració per a CTestConfig.cmake
SET (CTEST_DROP_METHOD xmlrpc) 
SET (CTEST_DROP_SITE "http://trueta.udg.edu/dart")
SET (CTEST_DROP_LOCATION "Starviewer")

# Entorn
SET (CTEST_SITE "$ENV{HOSTNAME}")
SET (CTEST_ENVIRONMENT "LC_ALL=en_US.UTF-8")
SET ($ENV{LC_ALL} "en_US.UTF-8")
SET ($ENV{LC_MESSAGES} "en_EN" )

# set any extra envionment varibles here
SET (CTEST_INITIAL_CACHE "
CMAKE_GENERATOR:INTERNAL=Unix Makefiles
MAKE_MAKE_PROGRAM:FILEPATH=make
SITE:STRING=$ENV{HOSTNAME}
")

# Subversion
SET (CTEST_UPDATE_COMMAND "/usr/bin/svn")
SET (CTEST_SVN_COMMAND "/usr/bin/svn")
SET (CTEST_SVN_UPDATE_OPTIONS "${CTEST_SVN_COMMAND} co https://trueta.udg.edu/repos/starviewer/trunk/starviewer")

# Configure and build
SET (CTEST_SOURCE_DIRECTORY ".")
SET (CTEST_BINARY_DIRECTORY ".")
SET (CTEST_CONFIGURE_COMMAND "/usr/bin/qmake")
SET (CTEST_BUILD_COMMAND "/usr/bin/make")

IF (${CTEST_SCRIPT_ARG} MATCHES Experimental)
    SET (CTEST_COMMAND
        "/usr/bin/ctest -D ExperimentalStart"
        "/usr/bin/ctest -D ExperimentalUpdate"
        "/usr/bin/ctest -D ExperimentalConfigure"
        "/usr/bin/ctest -D ExperimentalBuild"
        "/usr/bin/ctest -D ExperimentalTest"
        "/usr/bin/ctest -D ExperimentalSubmit"
    )
ENDIF (${CTEST_SCRIPT_ARG} MATCHES Experimental)

# Track Select
IF (${CTEST_SCRIPT_ARG} MATCHES Nightly)
    SET (CTEST_COMMAND
        "/usr/bin/ctest -D NightlyStart"
        "/usr/bin/ctest -D NightlyUpdate"
        "/usr/bin/ctest -D NightlyConfigure"
        "/usr/bin/ctest -D NightlyBuild"
        "/usr/bin/ctest -D NightlyTest"
        "/usr/bin/ctest -D NightlySubmit"
    )
ENDIF (${CTEST_SCRIPT_ARG} MATCHES Nightly)

IF (${CTEST_SCRIPT_ARG} MATCHES Continuous)
    # Continuous parameters
    SET (CTEST_CONTINUOUS_DURATION 600)
    SET (CTEST_CONTINUOUS_MINIMUM_INTERVAL 10)
    SET (CTEST_START_WITH_EMPTY_BINARY_DIRECTORY_ONCE 0)
    SET (CTEST_COMMAND
        "/usr/bin/ctest -D Continuous Start Update Configure Build Test Submit"
    )
ENDIF (${CTEST_SCRIPT_ARG} MATCHES Continuous)

IF (${CTEST_SCRIPT_ARG} MATCHES Style)
    SET (CTEST_COMMAND
        "/usr/bin/ctest -D NightlyStart"
        "/usr/bin/ctest -D NightlyUpdate"
        "/usr/bin/ctest -D NightlyConfigure"
        "KWStyle -xml kws.xml -html KWStyle -lesshtml -kwsurl http://localhost:8081/Starviewer/ -dart ${CTEST_BINARY_DIRECTORY} -1 1 -D kwsFiles.txt"
        "/usr/bin/ctest -D NightlySubmit"
    )
ENDIF (${CTEST_SCRIPT_ARG} MATCHES Style)

