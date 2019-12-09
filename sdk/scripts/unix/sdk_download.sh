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
            download_and_verify 37dad355d5513b4de4a86b5b7b0c3e9ec059860d88781b80916bba2a04e6d5b8 \
                                'dcmtk-DCMTK-3.6.5.tar.gz' \
                                'https://github.com/DCMTK/dcmtk/archive/DCMTK-3.6.5.tar.gz'
            ;;
        vtk)
            download_and_verify 71a09b4340f0a9c58559fe946dc745ab68a866cf20636a41d97b6046cb736324 \
                                'VTK-8.1.1.tar.gz' \
                                'https://www.vtk.org/files/release/8.1/VTK-8.1.1.tar.gz'
            ;;
        gdcm)
            download_and_verify 9e460e5aae0082e5bb0cb4e6cb1293f4ea902394406e2260cc92674f7bd481b2 \
                                'GDCM-3.0.4.tar.gz' \
                                'https://github.com/malaterre/GDCM/archive/v3.0.4.tar.gz'
            ;;
        itk)
            download_and_verify 613b125cbf58481e8d1e36bdeacf7e21aba4b129b4e524b112f70c4d4e6d15a6 \
                                'InsightToolkit-5.0.1.tar.gz' \
                                'https://github.com/InsightSoftwareConsortium/ITK/releases/download/v5.0.1/InsightToolkit-5.0.1.tar.gz'
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
