#!/bin/bash

# NOTES REGARDING SETUP.SH
# ========================
# - Feel free to change default values in this file, but be careful with the functions and conditions.
# - Many variables can be externally set when they are already specified as an environment variable.
# - The two commas in ${SOMETHING,,} convert the variable value to lower case.

# Exportable variables
# ====================
# The following variable collects a list of variables of interest when setting the environment and to generate the prefix files.
# The list is expanded as variables are set...
VARS_TO_EXPORT=()

# VCVARSALL if not done before
# ============================
# Assuming you are on Windows then VCVARSALL is only applied once.
# The existance of VCToolsVersion is checked to know if the application had been successful.
if [[ $(uname) == 'MSYS_NT'* ]]
then
    if [[ -z $VCToolsVersion ]]
    then
        echo "Trying to apply VCVARSALL."
        . "$SCRIPTS_ROOT/config/vcvarsall.sh" x64
    else
        echo "VCVARSALL already applied."
    fi
fi

# LD_LIBRARY_PATH preservation
# ============================
# Used to avoid duplications when this script is invoked multiple times
if [[ -z "$UNTOUCHED_LD_LIBRARY_PATH" ]]
then
    UNTOUCHED_LD_LIBRARY_PATH=$LD_LIBRARY_PATH
fi
# LD_LIBRARY_PATH variable is exported to prefix.sh, but is treated in a special way.
LD_LIBRARY_PATH=$UNTOUCHED_LD_LIBRARY_PATH

# PATH preservation
# =================
# Used to avoid duplications when this script is invoked multiple times
if [[ -z "$UNTOUCHED_PATH" ]]
then
    UNTOUCHED_PATH=$PATH
fi
# PATH variable is exportedto prefix.sh, but is treated in a special way.
PATH=$UNTOUCHED_PATH

# Reversing PATH variable
# =======================
# Done in order to make the executables established by vcvarsall.bat PATH prevail over the MSYS ones.
# The IFS is an internal bash variable that establishes the bash interpreter separator.
# if [[ $(uname) == 'MSYS_NT'* ]]
# then
    # UNTOUCHED_IFS=$IFS
    # IFS=':'
    #We put /c/ /d/ etc. paths on top.
    # unset PATH_REVERSED
    # for PATH_ITEM in $PATH
    # do
        # PATH_REVERSED=$PATH_ITEM${PATH_REVERSED:+:${PATH_REVERSED}}
        # echo "     -> $PATH_ITEM"
    # done
    # PATH=$PATH_REVERSED	
    # IFS=$UNTOUCHED_IFS
# fi

# Auxiliary function
# =================
download_and_verify()
{
    EXPECTED_HASH=$1
    FILENAME=$2
    URL=$3
    if [ ! -e "${DOWNLOAD_PREFIX}/${FILENAME}" ]
    then
        wget -O "${DOWNLOAD_PREFIX}/${FILENAME}" -nc --directory-prefix="${DOWNLOAD_PREFIX}" "${URL}"
    else
    echo "The ${DOWNLOAD_PREFIX}/${FILENAME} exists, skipping the download."
    fi
    HASH=`shasum -a 256 "${DOWNLOAD_PREFIX}/${FILENAME}" | cut -f 1 -d " "`

    if [ "${EXPECTED_HASH}" != "${HASH}" ]
    then
    echo "ERROR: Checksum verification failed for ${FILENAME}."
    echo ''
    echo '[!] WARNING [!]'
    echo ''
    echo 'If the download has been over HTTPS then:'
    echo '   (i) the originating server has altered files that ought to be immutable;'
    echo '  (ii) your system is affected by some sort of security bug;'
    echo ' (iii) you have trusted bogus root certificates;'
    echo '  (iv) some powerful attacker has control over the PKI (public key infraestructure); or'
    echo '   (v) your computer has been hit by a cosmic ray.'
    echo ''
    echo 'DO NOT USE THE DOWNLOADED FILE; delete it.'
    echo ''
    rm -i "${DOWNLOAD_PREFIX}/${FILENAME}"
    exit
    fi
}

# Base directory for everything related to the SDK.
SDK_BASE_PREFIX=${SDK_BASE_PREFIX:-"$SCRIPTS_ROOT/../../.."}
VARS_TO_EXPORT+=("SDK_BASE_PREFIX")

# Where the libraries are dowloaded.
DOWNLOAD_PREFIX=${DOWNLOAD_PREFIX:-"$SDK_BASE_PREFIX/sdk-download"}

# Location of SDK sources to build.
SDK_SOURCE_DIR_BASE=${SDK_SOURCE_DIR_BASE:-"$SDK_BASE_PREFIX/sdk-build"}
VARS_TO_EXPORT+=("SDK_SOURCE_DIR_BASE")

# Where to install the SDK libraries once compiled.
SDK_INSTALL_PREFIX=${SDK_INSTALL_PREFIX:-"$SDK_BASE_PREFIX/sdk-install"}
VARS_TO_EXPORT+=("SDK_INSTALL_PREFIX")

# List of build types to use. Possible values: Debug, Release, RelWithDebInfo, MinSizeRel.
SDK_BUILD_TYPE=${SDK_BUILD_TYPE:-"Release"}
VARS_TO_EXPORT+=("SDK_BUILD_TYPE")

# Where the user will be asked to install Qt.
INSTALL_QTDIR=${INSTALL_QTDIR:-"$SDK_INSTALL_PREFIX/qt"}

# Directory where the specific version of Qt is installed.
if [[ $(uname) == 'Linux' ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.12.6/gcc_64"}
elif [[ $(uname) == 'Darwin' ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.12.6/clang_64"}
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    QTDIR=${QTDIR:-"$INSTALL_QTDIR/5.12.6/msvc2017_64"}
else
    echo "Error: Qt platform not considered."
    exit 1
fi

QTBINDIR=${QTBINDIR:-$QTDIR/bin}
QTLIBDIR=${QTLIBDIR:-$QTDIR/lib}
QTPLUGINSDIR=${QTPLUGINSDIR:-$QTDIR/plugins}
QTQMLDIR=${QTQMLDIR:-$QTDIR/qml}
QTTRANSLATIONSDIR=${QTTRANSLATIONSDIR:-$QTDIR/translations}

VARS_TO_EXPORT+=("QTBINDIR")
VARS_TO_EXPORT+=("QTLIBDIR")
VARS_TO_EXPORT+=("QTPLUGINSDIR")
VARS_TO_EXPORT+=("QTQMLDIR")
VARS_TO_EXPORT+=("QTTRANSLATIONSDIR")

# List of libs to build. Possible values: qt, dcmtk, vtk, gdcm, itk, ecm, threadweaver.
LIBS=${LIBS:-"qt dcmtk vtk gdcm itk ecm threadweaver"}

# CMake and make
if [[ $(uname) == 'MSYS_NT'* ]]
then
    # jom path in PATH needed by CMake
    JOM_PATH=${JOM_PATH:-"$INSTALL_QTDIR/Tools/QtCreator/bin"}
    if [[ ! -d $JOM_PATH ]]
    then
        echo "[!] JOM path $JOM_PATH not found [!]"
    fi
    PATH=$JOM_PATH${PATH:+:${PATH}}
    MAKE=${MAKE:-jom}
    CMAKE=${CMAKE:-/mingw64/bin/cmake}
fi
CMAKE=${CMAKE:-cmake}
MAKE=${MAKE:-make}

# Set to appropriate value (example below) if you want to use distcc (distributed C compiler) on CMake.
CMAKE_DISTCC=${CMAKE_DISTCC:-}
#CMAKE_DISTCC='-DCMAKE_CXX_COMPILER_LAUNCHER:STRING=distcc -DCMAKE_C_COMPILER_LAUNCHER:STRING=distcc'

# Set to appropriate value (example below) to compile with non-default compiler (default is gcc on Linux and clang in Mac).
CMAKE_COMPILER=${CMAKE_COMPILER:-}
#CMAKE_COMPILER='-DCMAKE_CXX_COMPILER=clang++ -DCMAKE_C_COMPILER=clang'

# C++11 support
CMAKE_CPP11='-DCMAKE_CXX_STANDARD:STRING=11 -DCMAKE_CXX_STANDARD_REQUIRED:BOOL=ON -DCMAKE_C_STANDARD:STRING=11 -DCMAKE_C_STANDARD_REQUIRED:BOOL=ON'

# Number of simultaneous make jobs.
if [[ $(uname) == 'Linux' ]]
then
    MAKE_CONCURRENCY=${MAKE_CONCURRENCY:-`nproc`}
elif [[ $(uname) == 'Darwin' ]]
then
    MAKE_CONCURRENCY=${MAKE_CONCURRENCY:-`sysctl -n hw.ncpu`}
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    MAKE_CONCURRENCY=${MAKE_CONCURRENCY:-$NUMBER_OF_PROCESSORS}
fi
MAKE_CONCURRENCY=${MAKE_CONCURRENCY:-1}

# Verbose compilation: set to 'yes' to output the compiler calls.
MAKE_VERBOSE=${MAKE_VERBOSE:-yes}

# Where to write the SDK environment configuration script.
SDK_ENVIRONMENT_FILE=$SCRIPTS_ROOT/../../../prefix.sh
SDK_WINDOWS_ENVIRONMENT_FILE=$SCRIPTS_ROOT/../../../prefix.bat

# Currently only used to know the location of the ThreadWeaver lib dir.
if [[ $(uname) == 'Linux' ]]
then
    if [[ -e /etc/debian_version ]]
    then
        LIB64DIR=lib/`dpkg-architecture -q DEB_BUILD_GNU_TYPE`
    else
        LIB64DIR=lib64
    fi
elif [[ $(uname) == 'Darwin' ]]
then
    LIB64DIR=lib
elif [[ $(uname) == 'MSYS_NT'* ]]
then
    LIB64DIR=lib
fi


# Build type specified when compiling starviewer. Possible values: Debug, Release, RelWithDebInfo, MinSizeRel.
STARVIEWER_BUILD_TYPE=${STARVIEWER_BUILD_TYPE:-Release}

# Starviewer source code location
STARVIEWER_SOURCE_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer

# Starviwer shadow build directory
STARVIEWER_BUILD_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer-build

# Starviwer install prefix directory
STARVIEWER_INSTALL_DIR_BASE=$SCRIPTS_ROOT/../../../starviewer-install

# Starviewer CMake arguments, intended to generate an installer
STARVIEWER_CMAKE_ARGUMENTS="-DCMAKE_BUILD_TYPE:STRING=${STARVIEWER_BUILD_TYPE} -DCMAKE_INSTALL_PREFIX:PATH=${STARVIEWER_INSTALL_DIR_BASE} -DUSE_QTCONF:BOOL=ON"

# PATH-like and exportable Variables
# ==================================
PREPEND_TO_LD_LIBRARY_PATH="$STARVIEWER_BUILD_DIR_BASE/bin"
PREPEND_TO_PATH="$STARVIEWER_BUILD_DIR_BASE/bin"

PREPEND_TO_LD_LIBRARY_PATH=$QTLIBDIR${PREPEND_TO_LD_LIBRARY_PATH:+:${PREPEND_TO_LD_LIBRARY_PATH}}
PREPEND_TO_PATH=$QTBINDIR${PREPEND_TO_PATH:+:${PREPEND_TO_PATH}}

# Get the variables of the libraries in order to compose the PATH and LD_LIBRARY_PATH
for ALIB in $LIBS
do
    if [[ -f "$SCRIPTS_ROOT/libs/$ALIB.sh" ]]
    then
        . "$SCRIPTS_ROOT/config/$ALIB.sh"
    fi
done

PREPEND_TO_LD_LIBRARY_PATH=$DCMTKLIBDIR${PREPEND_TO_LD_LIBRARY_PATH:+:${PREPEND_TO_LD_LIBRARY_PATH}}
PREPEND_TO_PATH=$DCMTKBINDIR${PREPEND_TO_PATH:+:${PREPEND_TO_PATH}}
VARS_TO_EXPORT+=("DCMTKLIBDIR")
VARS_TO_EXPORT+=("DCMTKBINDIR")
VARS_TO_EXPORT+=("DCMTKINCLUDEDIR")

VARS_TO_EXPORT+=("ECMCMAKEDIR")

PREPEND_TO_LD_LIBRARY_PATH=$GDCMLIBDIR${PREPEND_TO_LD_LIBRARY_PATH:+:${PREPEND_TO_LD_LIBRARY_PATH}}
PREPEND_TO_PATH=$GDCMBINDIR${PREPEND_TO_PATH:+:${PREPEND_TO_PATH}}
VARS_TO_EXPORT+=("GDCMCMAKEDIR")
VARS_TO_EXPORT+=("GDCMLIBDIR")
VARS_TO_EXPORT+=("GDCMBINDIR")
VARS_TO_EXPORT+=("GDCMINCLUDEDIR")

PREPEND_TO_LD_LIBRARY_PATH=$ITKLIBDIR${PREPEND_TO_LD_LIBRARY_PATH:+:${PREPEND_TO_LD_LIBRARY_PATH}}
PREPEND_TO_PATH=$ITKBINDIR${PREPEND_TO_PATH:+:${PREPEND_TO_PATH}}
VARS_TO_EXPORT+=("ITKCMAKEDIR")
VARS_TO_EXPORT+=("ITKLIBDIR")
VARS_TO_EXPORT+=("ITKBINDIR")
VARS_TO_EXPORT+=("ITKINCLUDEDIR")

PREPEND_TO_LD_LIBRARY_PATH=$THREADWEAVERLIBDIR${PREPEND_TO_LD_LIBRARY_PATH:+:${PREPEND_TO_LD_LIBRARY_PATH}}
PREPEND_TO_PATH=$THREADWEAVERBINDIR${PREPEND_TO_PATH:+:${PREPEND_TO_PATH}}
VARS_TO_EXPORT+=("THREADWEAVERCMAKEDIR")
VARS_TO_EXPORT+=("THREADWEAVERLIBDIR")
VARS_TO_EXPORT+=("THREADWEAVERBINDIR")
VARS_TO_EXPORT+=("THREADWEAVERINCLUDEDIR")

PREPEND_TO_LD_LIBRARY_PATH=$VTKLIBDIR${PREPEND_TO_LD_LIBRARY_PATH:+:${PREPEND_TO_LD_LIBRARY_PATH}}
PREPEND_TO_PATH=$VTKBINDIR${PREPEND_TO_PATH:+:${PREPEND_TO_PATH}}
VARS_TO_EXPORT+=("VTKCMAKEDIR")
VARS_TO_EXPORT+=("VTKLIBDIR")
VARS_TO_EXPORT+=("VTKBINDIR")
VARS_TO_EXPORT+=("VTKINCLUDEDIR")

# Because SDK libraries binares are not on a standard location.
# This environment variable has to be set when starting starviewer binary or
# when you compile it (if not the linker will fail).
# It will be written to the SDK environment configuration script.
declare -x LD_LIBRARY_PATH=$PREPEND_TO_LD_LIBRARY_PATH${LD_LIBRARY_PATH:+:${LD_LIBRARY_PATH}}


# What you neet to put at the beginning of your current path. On Windows the PATH also works a little bit like LD_LIBRARY_PATH in order search .dll files.
declare -x PATH=$PREPEND_TO_PATH${PATH:+:${PATH}}


# Export exportable variables
# ===========================

for VARIABLE in ${VARS_TO_EXPORT[@]}
do
    export $VARIABLE
done