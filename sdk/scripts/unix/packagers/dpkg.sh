#!/bin/bash
# Those variables are meant to be set externally by starviewer_packaging.sh
# If not those "generalistic" defaults are picked to create a .deb that can be put anywhere, although it will probably not run smoothly as no system dependencies are specified.
PACKAGER_DIST_NAME=${PACKAGER_DIST_NAME:-anydist}
PACKAGER_DIST_ARCH=${PACKAGER_DIST_ARCH:-'none'}
PACKAGER_DEPENDS=${PACKAGER_DEPENDS:-''}

PACKAGER_PROD_NAME=${PACKAGER_PROD_NAME:-product}
PACKAGER_PROD_VERSION=${PACKAGER_PROD_VERSION:-version}

OUTPUT_DIR=$STARVIEWER_PACKAGING_DIR_BASE/${PACKAGER_PROD_NAME}-${PACKAGER_PROD_VERSION}~${PACKAGER_DIST_NAME}_${PACKAGER_DIST_ARCH}

mkdir -p $OUTPUT_DIR
pushd $OUTPUT_DIR
echo "Packaging $OUTPUT_DIR"

# Data directory
# ==============

mkdir -p data/opt/starviewer
cp -r $STARVIEWER_INSTALL_DIR_BASE/* data/opt/starviewer

mkdir -p data/usr/bin
ln -s ../../opt/starviewer/starviewer.sh data/usr/bin/starviewer

mkdir -p data/usr/share/applications
echo "[Desktop Entry]
Version=$PACKAGER_PROD_VERSION
Type=Application
Name=$PACKAGER_PROD_NAME (${PACKAGER_PROD_VERSION})
Comment=Medical DICOM image viewer.
Exec=../../../opt/starviewer/starviewer.sh
Icon=../../../opt/starviewer/logo/64.svg
Categories=Graphics;Science;Biology;Medical;
" > data/usr/share/applications/starviewer.desktop

# Control directory
# =================

mkdir -p control

echo "Package: $PACKAGER_PROD_NAME
Version: $PACKAGER_PROD_VERSION
Section: medical
Priority: optional
Architecture: $PACKAGER_DIST_ARCH
Depends: $PACKAGER_DEPENDS
Installed-Size: `du -s data | cut -f 1`
Maintainer: Starviewer Team <suport@starviewer.udg.edu>
Description: DICOM medical viewer with advanced features. 
" > control/control


# Packaging
# =========

echo '2.0' > debian-binary
tar czf data.tar.gz -C data .
tar czf control.tar.gz -C control .

ar -r $OUTPUT_DIR.deb debian-binary control.tar.gz data.tar.gz

popd
