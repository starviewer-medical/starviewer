#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

download_and_verify()
{
    EXPECTED_HASH=$1
    FILENAME=$2
    URL=$3

    wget -nc -O "${DOWNLOAD_PREFIX}/${FILENAME}" "${URL}"
    HASH=`shasum -a 256 "${DOWNLOAD_PREFIX}/${FILENAME}" | cut -f 1 -d " "`

    if [[ "${EXPECTED_HASH}" != "${HASH}" ]]
    then
        echo "ERROR: Checksum verification failed for ${FILENAME}."
        exit
    fi
}

mkdir -p "${DOWNLOAD_PREFIX}"

for LIB in $LIBS
do
    case $LIB in
        qt)
            if [[ $(uname) == 'Linux' ]]
            then
                download_and_verify f1fbd6b7ee067f6af6412d16c33c5478aed230c655c888d9cc7b1037f8048ec5 \
                                    'qt-opensource-linux-x64-5.6.3.run' \
                                    'https://download.qt.io/official_releases/qt/5.6/5.6.3/qt-opensource-linux-x64-5.6.3.run'
            elif [[ $(uname) == 'Darwin' ]]
            then
                download_and_verify e12174b4a128ef34900527487d7ad147fad56e081fe4e0ff667737decd27b6b5 \
                                    'qt-opensource-mac-x64-clang-5.6.3.dmg' \
                                    'https://download.qt.io/official_releases/qt/5.6/5.6.3/qt-opensource-mac-x64-clang-5.6.3.dmg'
            fi
            ;;
        dcmtk)
            download_and_verify 09a97e82cf43672a4b16b162651f5bc0d86b9d7b8cfff58c345e5c97d6200937 \
                                'dcmtk-3.6.1_20120515.tar.gz' \
                                'https://support.dcmtk.org/redmine/attachments/download/8/dcmtk-3.6.1_20120515.tar.gz'
            ;;
        vtk)
            download_and_verify 78a990a15ead79cdc752e86b83cfab7dbf5b7ef51ba409db02570dbdd9ec32c3 \
                                'VTK-7.0.0.tar.gz' \
                                'https://www.vtk.org/files/release/7.0/VTK-7.0.0.tar.gz'
            ;;
        gdcm)
            download_and_verify f4e611c74ea2b16c9aa28d7fdf3a8678ce926909606705b7cd08c9a5b05d34fc \
                                'GDCM-2.8.6.tar.gz' \
                                'https://github.com/malaterre/GDCM/archive/v2.8.6.tar.gz'
            ;;
        itk)
            download_and_verify ea0011fc99a6303427b236c3ceaf54688224350ce1a216b64c9771ab3194b15f \
                                'InsightToolkit-4.10.0.tar.xz' \
                                'http://downloads.sourceforge.net/project/itk/itk/4.10/InsightToolkit-4.10.0.tar.xz'    # https fails in mac
            ;;
        ecm)
            download_and_verify ea8b6a648b7b2dc313df709940813c22ed5b68d2f7bef30acbaf2a83a6e4c723 \
                                'extra-cmake-modules-1.3.0.tar.xz' \
                                'https://download.kde.org/Attic/frameworks/5.3.0/extra-cmake-modules-1.3.0.tar.xz'
            ;;
        threadweaver)
            download_and_verify c15818ec6e6320bea79a0d44e29cb4fc19a0e0d428cccb979d6c3104ec65a838 \
                                'threadweaver-5.3.0.tar.xz' \
                                'https://download.kde.org/Attic/frameworks/5.3.0/threadweaver-5.3.0.tar.xz'
            ;;
    esac
done
