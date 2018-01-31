#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

#chmod u+x "${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.6.3.run"
#echo "Please install Qt on ${INSTALL_QTDIR}"
#"${DOWNLOAD_PREFIX}/qt-opensource-linux-x64-5.6.3.run"

# Building SDK

mkdir -p "${SOURCE_DIR_BASE}"

tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/dcmtk-3.6.1_20120515.tar.gz"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/VTK-7.0.0.tar.gz"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/gdcm-2.6.4.tar.bz2"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/InsightToolkit-4.10.0.tar.xz"
tar -C "${SOURCE_DIR_BASE}" -xvf "${DOWNLOAD_PREFIX}/threadweaver-5.3.0.tar.xz"
tar -C "${SOURCE_DIR_BASE}/threadweaver-5.3.0" -xvf "${DOWNLOAD_PREFIX}/extra-cmake-modules-1.3.0.tar.xz"


