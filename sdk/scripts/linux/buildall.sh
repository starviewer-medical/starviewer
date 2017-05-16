#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

### BEGIN: AUXILIARY FUNCTIONS ###

download_and_verify()
{
    EXPECTED_HASH=$1
    FILENAME=$2
    URL=$3

    wget -nc --directory-prefix="${DOWNLOAD_PREFIX}" "${URL}"
    HASH=`sha256sum "${DOWNLOAD_PREFIX}/${FILENAME}" | cut -f 1 -d " "`

    if [ "${EXPECTED_HASH}" != "${HASH}" ]
    then
        echo "ERROR: Checksum verification failed for ${FILENAME}."
        exit
    fi
}

### END: AUXILIARY FUNCTIONS ###


mkdir -p "${DOWNLOAD_PREFIX}"

download_and_verify 1fa2df848a25b900c87c7f7b98a34e3869b6ce9699fae0afd73a2da0f572bb89 \
                    'qt-opensource-linux-x64-5.6.0.run' \
                    'https://download.qt.io/official_releases/qt/5.6/5.6.0/qt-opensource-linux-x64-5.6.0.run'

download_and_verify 09a97e82cf43672a4b16b162651f5bc0d86b9d7b8cfff58c345e5c97d6200937 \
                    'dcmtk-3.6.1_20120515.tar.gz' \
                    'http://support.dcmtk.org/redmine/attachments/download/8/dcmtk-3.6.1_20120515.tar.gz'

download_and_verify 78a990a15ead79cdc752e86b83cfab7dbf5b7ef51ba409db02570dbdd9ec32c3 \
                    'VTK-7.0.0.tar.gz' \
                    'http://www.vtk.org/files/release/7.0/VTK-7.0.0.tar.gz'

download_and_verify b116dc0b0e9f65dcd2196fa55d8815be215065ec537b15665afde0469bd47e91 \
                    'gdcm-2.6.4.tar.bz2' \
                    'http://downloads.sourceforge.net/project/gdcm/gdcm%202.x/GDCM%202.6.4/gdcm-2.6.4.tar.bz2'

download_and_verify ea0011fc99a6303427b236c3ceaf54688224350ce1a216b64c9771ab3194b15f \
                    'InsightToolkit-4.10.0.tar.xz' \
                    'http://downloads.sourceforge.net/project/itk/itk/4.10/InsightToolkit-4.10.0.tar.xz'

download_and_verify ea8b6a648b7b2dc313df709940813c22ed5b68d2f7bef30acbaf2a83a6e4c723 \
                    'extra-cmake-modules-1.3.0.tar.xz' \
                    'http://download.kde.org/Attic/frameworks/5.3.0/extra-cmake-modules-1.3.0.tar.xz'

download_and_verify c15818ec6e6320bea79a0d44e29cb4fc19a0e0d428cccb979d6c3104ec65a838 \
                    'threadweaver-5.3.0.tar.xz' \
                    'http://download.kde.org/Attic/frameworks/5.3.0/threadweaver-5.3.0.tar.xz'


chmod u+x "${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.6.0.run"
echo "Please install Qt on ${INSTALL_QTDIR}"
"${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.6.0.run"

# Building SDK

mkdir -p "${SOURCE_DIR_BASE}"

tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/dcmtk-3.6.1_20120515.tar.gz"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/VTK-7.0.0.tar.gz"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/gdcm-2.6.4.tar.bz2"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/InsightToolkit-4.10.0.tar.xz"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/threadweaver-5.3.0.tar.xz"
tar -C "${SOURCE_DIR_BASE}/threadweaver-5.3.0" -xvf "${DOWNLOAD_PREFIX}/extra-cmake-modules-1.3.0.tar.xz"

. $SCRIPTS_ROOT/build.sh

read -p "Copy the environment variables to the clipboard for a future use and press [Enter] to start building starviewer..."

# Building starviewer

export SDK_INSTALL_PREFIX
export LD_LIBRARY_PATH

cd $STARVIEWER_SOURCE_DIR_BASE
$QTDIR/bin/qmake starviewer.pro -r -spec linux-g++
make -j4

# Starviewer needs to be loaded with LD_LIBRARY_PATH environment variable if the SDK is in a non-system path
START_SCRIPT="${STARVIEWER_SOURCE_DIR_BASE}/bin/starviewer.sh"
echo "#!/bin/bash" > $START_SCRIPT
echo 'DIR=$(readlink -f $(dirname $BASH_SOURCE))' >> $START_SCRIPT
echo -n "LD_LIBRARY_PATH=$LD_LIBRARY_PATH" >> $START_SCRIPT
echo ' $DIR/starviewer "$@"' >> $START_SCRIPT

# Running starviewer

chmod u+x $STARVIEWER_SOURCE_DIR_BASE/bin/starviewer.sh
$STARVIEWER_SOURCE_DIR_BASE/bin/starviewer.sh &

echo "Have a nice day."
