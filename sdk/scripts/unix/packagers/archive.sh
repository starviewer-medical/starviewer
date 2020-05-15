#!/bin/bash
# Those variables are meant to be set externally by starviewer_packaging.sh

PACKAGER_DIST_NAME=${PACKAGER_DIST_NAME:-anydist}
PACKAGER_DIST_ARCH=${PACKAGER_DIST_ARCH:-'none'}

PACKAGER_PROD_NAME=${PACKAGER_PROD_NAME:-product}
PACKAGER_PROD_VERSION=${PACKAGER_PROD_VERSION:-version}

OUTPUT_DIR=$STARVIEWER_PACKAGING_DIR_BASE/${PACKAGER_PROD_NAME}-${PACKAGER_PROD_VERSION}~${PACKAGER_DIST_NAME}_${PACKAGER_DIST_ARCH}

mkdir -p $OUTPUT_DIR
pushd $OUTPUT_DIR
echo "Packaging $OUTPUT_DIR"

# Data directory
# ==============
cp -r $STARVIEWER_INSTALL_DIR_BASE/* .

# Packaging
# =========
tar czf $OUTPUT_DIR.tar.gz .
tar cJf $OUTPUT_DIR.tar.xz .
zip -r  $OUTPUT_DIR.zip .

popd
