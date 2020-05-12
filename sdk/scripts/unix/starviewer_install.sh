#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

mkdir -p $STARVIEWER_INSTALL_DIR_BASE
pushd $STARVIEWER_INSTALL_DIR_BASE

# Path like variable of all the binaries where dependencies shall be inferred from
DO_LDD="$STARVIEWER_BUILD_DIR_BASE/bin/starviewer:$STARVIEWER_BUILD_DIR_BASE/bin/starviewer_crashreporter:$STARVIEWER_BUILD_DIR_BASE/bin/starviewer_sapwrapper"

# Variable mainly used to copy Qt plugins.
LIB_EXTENSION=".so"
LIB_PREFIX="lib"
if [[ $(uname) == 'MSYS_NT'* ]]
then
    LIB_PREFIX=""
    if [[ ${SDK_BUILD_TYPE,,} == "debug" ]]
    then
        LIB_EXTENSION="d.dll"
    else
        LIB_EXTENSION=".dll"
    fi
fi


# Copy files
# ==========
pushd $STARVIEWER_INSTALL_DIR_BASE

mkdir dicomdumpdefaulttags
cp -r $STARVIEWER_SOURCE_DIR_BASE/dicomdumpdefaulttags .

mkdir hangingprotocols
cp -r $STARVIEWER_SOURCE_DIR_BASE/hangingprotocols/Default/* hangingprotocols

mkdir releasenotes
cp $STARVIEWER_SOURCE_DIR_BASE/releasenotes/*.html releasenotes
cp $STARVIEWER_SOURCE_DIR_BASE/releasenotes/*.css releasenotes
cp $STARVIEWER_SOURCE_DIR_BASE/releasenotes/*.svg releasenotes

cp $STARVIEWER_BUILD_DIR_BASE/bin/starviewer .
DO_LDD="$DO_LDD:`realpath starviewer`"

cp $STARVIEWER_BUILD_DIR_BASE/bin/starviewer_crashreporter .
DO_LDD="$DO_LDD:`realpath starviewer_crashreporter`"

cp $STARVIEWER_BUILD_DIR_BASE/bin/starviewer_sapwrapper .
DO_LDD="$DO_LDD:`realpath starviewer_sapwrapper`"

if [[  $(uname) == 'MSYS_NT'* ]]
then
    cp $STARVIEWER_SOURCE_DIR_BASE/bin/mkisofs.exe .
fi

if [[ $(uname) == 'Linux' ]]
then
    cp $STARVIEWER_SOURCE_DIR_BASE/bin/starviewer.sh .
fi

cp $STARVIEWER_SOURCE_DIR_BASE/bin/log.conf .

# File used for the anonymization feature
cp $STARVIEWER_SOURCE_DIR_BASE/bin/Part3.xml .

# Installed Qt configuration
echo "
[paths]
prefix=.
" > qt.conf

popd

# Qt QML things
# --------------
mkdir $STARVIEWER_INSTALL_DIR_BASE/qml
mkdir $STARVIEWER_INSTALL_DIR_BASE/qml/QtQuick.2
pushd $STARVIEWER_INSTALL_DIR_BASE/qml/QtQuick.2

cp $QTQMLDIR/QtQuick.2/plugins.qmltypes .
cp $QTQMLDIR/QtQuick.2/qmldir .
cp $QTQMLDIR/QtQuick.2/${LIB_PREFIX}qtquick2plugin$LIB_EXTENSION .

DO_LDD="$DO_LDD:`realpath ${LIB_PREFIX}qtquick2plugin`$LIB_EXTENSION"

popd

# Qt plugins
# ----------
mkdir $STARVIEWER_INSTALL_DIR_BASE/plugins
pushd $STARVIEWER_INSTALL_DIR_BASE/plugins

mkdir iconengines
cp $QTPLUGINSDIR/iconengines/${LIB_PREFIX}qsvgicon$LIB_EXTENSION iconengines

mkdir imageformats
cp $QTPLUGINSDIR/imageformats/${LIB_PREFIX}qgif$LIB_EXTENSION imageformats
cp $QTPLUGINSDIR/imageformats/${LIB_PREFIX}qsvg$LIB_EXTENSION imageformats
cp $QTPLUGINSDIR/imageformats/${LIB_PREFIX}qico$LIB_EXTENSION imageformats


if [[ $(uname) == 'Linux' ]]
then
    cp -r $QTPLUGINSDIR/platform* .
    cp -r $QTPLUGINSDIR/wayland* .
    cp -r $QTPLUGINSDIR/xcb* .
fi
if [[ $(uname) == 'Darwin' ]]
then
    echo "TODO Specify Darwin based system plugins to copy"
    exit 1
fi
if [[ $(uname) == 'MSYS_NT'* ]]
then
    mkdir platforms
    mkdir styles
    cp $QTPLUGINSDIR/platforms/${LIB_PREFIX}qwindows$LIB_EXTENSION platforms
    cp $QTPLUGINSDIR/styles/${LIB_PREFIX}qwindowsvistastyle$LIB_EXTENSION styles
fi

mkdir sqldrivers
cp $QTPLUGINSDIR/sqldrivers/${LIB_PREFIX}qsqlite$LIB_EXTENSION sqldrivers

# Add all copied libraries to LDD todo list
# realpath outputs several lines, with xargs and echo we produce a : delimited list.
DO_LDD="$DO_LDD:`realpath ./*/*.* | xargs -I {} echo -n :{}`"

popd




# Automatic dependency detection 
# ==============================

LIBRARY_DIRECTORIES=$PREPEND_TO_PATH:$PREPEND_TO_LD_LIBRARY_PATH
COPIABLE_LIBRARY_SEARCH_PATH=""
UNTOUCHED_IFS=$IFS

# The following loop is for performance reasons, the readlink command takes a significative amount of time to process.
IFS=':'
for LIBRARY_DIRECTORY in $LIBRARY_DIRECTORIES
do
    # Unexistant directories are skipped
    if [[ -d $LIBRARY_DIRECTORY ]]
    then
        REAL_PATH=`realpath $LIBRARY_DIRECTORY`
        COPIABLE_LIBRARY_SEARCH_PATH="$REAL_PATH${COPIABLE_LIBRARY_SEARCH_PATH:+:${COPIABLE_LIBRARY_SEARCH_PATH}}"
    fi
done
IFS=$UNTOUCHED_IFS

# Warning: this is just one line, but made in to multiple using the backslash at the beginning and the end.
echo -n $DO_LDD | \
xargs -d ':' -I {} ldd {} | \
grep '=>' | \
sed -e '/^[^\t]/ d; s/^\t\(.* => \)\?\([^ ]*\) (.*/\2/g' | \
while read DEPENDENCY
do
    # The IFS is an internal bash variable that establishes the bash interpreter separator.
    #echo "DEPENDENCY -> $DEPENDENCY"
    DEPENDENCY=`realpath --no-symlinks $DEPENDENCY`
    UNTOUCHED_IFS=$IFS
    IFS=':'
    for LIBRARY_DIRECTORY in $COPIABLE_LIBRARY_SEARCH_PATH
    do
        #echo "DEP -> $DEPENDENCY"
        #echo "   DIR -> $LIBRARY_DIRECTORY"
        if [[ $DEPENDENCY == $LIBRARY_DIRECTORY* ]]
        then
            echo $DEPENDENCY
        fi
    done
    IFS=$UNTOUCHED_IFS
done | \
uniq | \
xargs -I {} cp {} $STARVIEWER_INSTALL_DIR_BASE

# Stripping debugging information
# ===============================

if [[ $(uname) == 'Linux' || $(uname) == 'Darwin' ]]
then
    # Strips debug information for files immediately in the same directory
    find -P -type f -exec strip {} \;
    find -P -type f -name '*.so*' -exec chmod ugo-x {} \;
fi

popd
