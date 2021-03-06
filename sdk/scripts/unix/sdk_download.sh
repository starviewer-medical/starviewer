#!/bin/bash

SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

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
            elif [[ $(uname) == 'MSYS_NT'* ]]
            then
                download_and_verify 7e7fd6bea5b1992771eb705188061acdf2121a59e9b2ccfc355016e22dded7a3 \
                                    'qt-opensource-windows-x86-5.12.6.exe' \
                                    'https://download.qt.io/official_releases/qt/5.12/5.12.6/qt-opensource-windows-x86-5.12.6.exe'
            fi
            ;;
        dcmtk)
            if [[ $(uname) == 'MSYS_NT'* ]]
            then
                download_and_verify 4fa47885833d3fbe51173f40397eb7c92d84580ddd39be4e316d653dfb1449d1 \
                                    'dcmtk-3.6.5-win64-support-MT-iconv-msvc-15.8.zip' \
                                    'https://dicom.offis.de/download/dcmtk/dcmtk365/support/dcmtk-3.6.5-win64-support-MT-iconv-msvc-15.8.zip'
            fi
            download_and_verify 37dad355d5513b4de4a86b5b7b0c3e9ec059860d88781b80916bba2a04e6d5b8 \
                                'dcmtk-DCMTK-3.6.5.tar.gz' \
                                'https://github.com/DCMTK/dcmtk/archive/DCMTK-3.6.5.tar.gz'
            ;;
        vtk)
            download_and_verify 96acfe0b4304218561bf2a88ca23aa35df51293d7ed9c3232c1d05a2012f405b \
                                'VTK-8.2.0.tar.gz' \
                                'https://github.com/Kitware/VTK/archive/v8.2.0.tar.gz'
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
            download_and_verify 6495fb18f8736cc3411fe3a6aab1f6f04c26e3a574ef1725378b7cb1c508a388 \
                                'extra-cmake-modules-5.64.0.tar.gz' \
                                'https://github.com/KDE/extra-cmake-modules/archive/v5.64.0.tar.gz'
            ;;
        threadweaver)
            download_and_verify 3c0051535d1d0755c64394d6a7bcd6f0dd4ccdfeaa292dfe80feef4916b2bac8 \
                                'threadweaver-5.64.0.tar.gz' \
                                'https://github.com/KDE/threadweaver/archive/v5.64.0.tar.gz'
            ;;
    esac
done
