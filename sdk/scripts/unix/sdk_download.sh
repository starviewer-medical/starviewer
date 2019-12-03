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
                download_and_verify b5ae1ad8f0bd288b0d5b239669dc077725c5d919bc7f217c67b709a4e9702e6e \
                                    'qt-opensource-linux-x64-5.12.6.run' \
                                    'https://download.qt.io/official_releases/qt/5.12/5.12.6/qt-opensource-linux-x64-5.12.6.run'
            elif [[ $(uname) == 'Darwin' ]]
            then
                download_and_verify 7eb75902fac1ebea0a31e4a3b6988f454c410427dafb522797ba09d4a5688a22 \
                                    'qt-opensource-mac-x64-5.12.6.dmg' \
                                    'https://download.qt.io/official_releases/qt/5.12/5.12.6/qt-opensource-mac-x64-5.12.6.dmg'
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
            download_and_verify e5da3e1b67e19d7b8b5b1d867bb1e9a54d902d718e55148f3296da9f9a18c275 \
                                'extra-cmake-modules-5.46.0.tar.xz' \
                                'https://download.kde.org/stable/frameworks/5.46/extra-cmake-modules-5.46.0.tar.xz'
            ;;
        threadweaver)
            download_and_verify f858d1f4b7d567e0cfba00b56e15af75e0552a1d6e7afb6925bacf3964f7cd7e \
                                'threadweaver-5.46.0.tar.xz' \
                                'https://download.kde.org/stable/frameworks/5.46/threadweaver-5.46.0.tar.xz'
            ;;
    esac
done
