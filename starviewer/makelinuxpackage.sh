#!/bin/bash


# configuration

test $1 && VERSION=$1 && VERSION_SUFFIX=-$1
STARVIEWER_DIR=$(readlink -f $(dirname $BASH_SOURCE))

test $TARGET_DIR || TARGET_DIR=$STARVIEWER_DIR/linuxpackage
test $PACKAGE_NAME || PACKAGE_NAME=starviewer$VERSION_SUFFIX

echo Package $TARGET_DIR/$PACKAGE_NAME.tar.xz will be created
echo Detected configuration:

if [ ! $QT_LIB_DIR ]
then
    QT_LIB_DIR=$(dirname $(ldd $STARVIEWER_DIR/bin/starviewer | grep libQt5Core | awk '{print $3}'))
    test $QT_LIB_DIR != . || { echo Please provide the QT_LIB_DIR variable; exit; }
    echo "QT_LIB_DIR=$QT_LIB_DIR"
fi

if [ ! $VTK_LIB_DIR ]
then
    VTK_LIB_DIR=$(dirname $(ldd $STARVIEWER_DIR/bin/starviewer | grep libvtkCommonCore | awk '{print $3}'))
    test $VTK_LIB_DIR != . || { echo Please provide the VTK_LIB_DIR variable; exit; }
    echo "VTK_LIB_DIR=$VTK_LIB_DIR"
fi

if [ ! $ITK_LIB_DIR ]
then
    ITK_LIB_DIR=$(dirname $(ldd $STARVIEWER_DIR/bin/starviewer | grep libITKCommon | awk '{print $3}'))
    test $ITK_LIB_DIR != . || { echo Please provide the ITK_LIB_DIR variable; exit; }
    echo "ITK_LIB_DIR=$ITK_LIB_DIR"
fi

if [ ! $GDCM_LIB_DIR ]
then
    GDCM_LIB_DIR=$(dirname $(ldd $STARVIEWER_DIR/bin/starviewer | grep libgdcmCommon | awk '{print $3}'))
    test $GDCM_LIB_DIR != . || { echo Please provide the GDCM_LIB_DIR variable; exit; }
    echo "GDCM_LIB_DIR=$GDCM_LIB_DIR"
fi

if [ ! $THREADWEAVER_LIB_DIR ]
then
    THREADWEAVER_LIB_DIR=$(dirname $(ldd $STARVIEWER_DIR/bin/starviewer | grep libKF5ThreadWeaver | awk '{print $3}'))
    test $THREADWEAVER_LIB_DIR != . || { echo Please provide the THREADWEAVER_LIB_DIR variable; exit; }
    echo "THREADWEAVER_LIB_DIR=$THREADWEAVER_LIB_DIR"
fi


# Create directory

rm -rf $TARGET_DIR/$PACKAGE_NAME
echo Creating directory $TARGET_DIR/$PACKAGE_NAME
mkdir -p $TARGET_DIR/$PACKAGE_NAME
cd $TARGET_DIR/$PACKAGE_NAME


# Main files

echo Copying main files...
cp $STARVIEWER_DIR/bin/starviewer .
cp $STARVIEWER_DIR/bin/starviewer.sh .
cp $STARVIEWER_DIR/bin/log.conf .
cp $STARVIEWER_DIR/bin/Part3.xml .


# Libraries

echo Copying libraries:
mkdir -p lib
cd lib

echo "  - SingleApplication..."
cp $STARVIEWER_DIR/bin/libQt*Solutions_SingleApplication-head.so.1 .

echo "  - Qt..."
cp $QT_LIB_DIR/libQt5*.so.5 .
cp $QT_LIB_DIR/libicu*.so.53 .

echo "  - VTK..."
cp $VTK_LIB_DIR/libvtk*.so.1 .

echo "  - ITK..."
cp $ITK_LIB_DIR/libITK*.so.1 .
cp $ITK_LIB_DIR/libitk*.so.1 .

echo "  - GDCM..."
cp $GDCM_LIB_DIR/libgdcm*.so.2.4 .
cp $GDCM_LIB_DIR/libvtkgdcm.so.2.4 .
cp $GDCM_LIB_DIR/libgdcmexpat.so.2.0 .
cp $GDCM_LIB_DIR/libgdcmopenjpeg.so.1.4 .

echo "  - ThreadWeaver..."
cp $THREADWEAVER_LIB_DIR/libKF5ThreadWeaver.so.5 .

cd ..


# Others

echo Copying Qt plugins...
mkdir -p platforms
cp $QT_LIB_DIR/../plugins/platforms/libqxcb.so platforms/
mkdir -p imageformats
cp $QT_LIB_DIR/../plugins/imageformats/libqgif.so imageformats/
mkdir -p sqldrivers
cp $QT_LIB_DIR/../plugins/sqldrivers/libqsqlite.so sqldrivers/

echo Copying DICOM dump default tags...
cp -r $STARVIEWER_DIR/dicomdumpdefaulttags .

echo Copying hanging protocols...
mkdir -p hangingprotocols
cp -r $STARVIEWER_DIR/hangingprotocols/Default/* hangingprotocols/

echo Copying release notes...
cp -r $STARVIEWER_DIR/releasenotes .
FINAL_VERSION=$(echo $VERSION | sed "s/\([0-9][0-9]*\.[0-9][0-9]*\.[0-9][0-9]*\).*/\1/")
mv releasenotes/changelog.html releasenotes/releasenotes$FINAL_VERSION.html

echo Copying icon...
cp $STARVIEWER_DIR/src/main/images/starviewer.png .

echo Copying documentation...
mkdir -p doc
cp $STARVIEWER_DIR/bin/*.pdf doc/


# Create package

cd ..
echo Creating archive $TARGET_DIR/$PACKAGE_NAME.tar.xz
rm -f $PACKAGE_NAME.tar.xz
XZ_OPT=-9 tar cJf $PACKAGE_NAME.tar.xz $PACKAGE_NAME

echo Finished.
