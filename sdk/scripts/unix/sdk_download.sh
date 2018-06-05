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
                download_and_verify 4f4ebd19051913e90a72b87a4397b0791ba50d81d26596b149867a11ffae0b16 \
                                    'qt-opensource-linux-x64-5.9.5.run' \
                                    'https://download.qt.io/official_releases/qt/5.9/5.9.5/qt-opensource-linux-x64-5.9.5.run'
            elif [[ $(uname) == 'Darwin' ]]
            then
                download_and_verify eda679eeb801a03f3b52c4870214f73fae13a851ec230cf356420789837d5a80 \
                                    'qt-opensource-mac-x64-5.9.5.dmg' \
                                    'https://download.qt.io/official_releases/qt/5.9/5.9.5/qt-opensource-mac-x64-5.9.5.dmg'
            fi
            ;;
        dcmtk)
            download_and_verify 09a97e82cf43672a4b16b162651f5bc0d86b9d7b8cfff58c345e5c97d6200937 \
                                'dcmtk-3.6.1_20120515.tar.gz' \
                                'https://support.dcmtk.org/redmine/attachments/download/8/dcmtk-3.6.1_20120515.tar.gz'
            ;;
        vtk)
            download_and_verify 71a09b4340f0a9c58559fe946dc745ab68a866cf20636a41d97b6046cb736324 \
                                'VTK-8.1.1.tar.gz' \
                                'https://www.vtk.org/files/release/8.1/VTK-8.1.1.tar.gz'
            ;;
        gdcm)
            download_and_verify f4e611c74ea2b16c9aa28d7fdf3a8678ce926909606705b7cd08c9a5b05d34fc \
                                'GDCM-2.8.6.tar.gz' \
                                'https://github.com/malaterre/GDCM/archive/v2.8.6.tar.gz'
            ;;
        itk)
            download_and_verify feb3fce3cd3bf08405e49da30876dc766e5145c821e5e3f8736df1d1717da125 \
                                'InsightToolkit-4.13.0.tar.xz' \
                                'https://freefr.dl.sourceforge.net/project/itk/itk/4.13/InsightToolkit-4.13.0.tar.xz'
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
