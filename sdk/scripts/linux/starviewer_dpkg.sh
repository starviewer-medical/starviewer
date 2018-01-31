#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

rm -rf $DPKG_TMP
mkdir -p $DPKG_TMP
cp -R $SCRIPTS_ROOT/dpkg/* $DPKG_TMP

cp -R $STARVIEWER_SOURCE_DIR_BASE/hangingprotocols $DPKG_TMP/data/opt/starviewer/
cp -R $STARVIEWER_SOURCE_DIR_BASE/dicomdumpdefaulttags $DPKG_TMP/data/opt/starviewer/bin/
cp -R $STARVIEWER_SOURCE_DIR_BASE/releasenotes $DPKG_TMP/data/opt/starviewer/
cp $STARVIEWER_SOURCE_DIR_BASE/bin/log.conf $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_SOURCE_DIR_BASE/bin/Part3.xml $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_SOURCE_DIR_BASE/bin/Starviewer_User_guide.pdf $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_SOURCE_DIR_BASE/bin/Starviewer_Shortcuts_guide.pdf $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_SOURCE_DIR_BASE/bin/Starviewer_Quick_start_guide.pdf $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_SOURCE_DIR_BASE/src/main/images/logo/ico-64.png $DPKG_TMP/data/opt/starviewer/starviewer.png
cp $STARVIEWER_BUILD_DIR_BASE/bin/starviewer $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_BUILD_DIR_BASE/bin/starviewer_crashreporter $DPKG_TMP/data/opt/starviewer/bin/
cp $STARVIEWER_BUILD_DIR_BASE/bin/starviewer_sapwrapper $DPKG_TMP/data/opt/starviewer/bin/

# Dependencies tracking
LD_LIBRARY_PATH=$LD_LIBRARY_PATH:$STARVIEWER_BUILD_DIR_BASE/bin/
export LD_LIBRARY_PATH

#cp -Rd $QTDIR/libexec $DPKG_TMP/data/opt/starviewer/
#cp -Rd $QTDIR/plugins $DPKG_TMP/data/opt/starviewer/
#cp -Rd $QTDIR/qml $DPKG_TMP/data/opt/starviewer/

ldd \
$STARVIEWER_BUILD_DIR_BASE/bin/starviewer \
$STARVIEWER_BUILD_DIR_BASE/bin/starviewer_sapwrapper \
$STARVIEWER_BUILD_DIR_BASE/bin/starviewer_crashreporter \
| grep -e $STARVIEWER_BUILD_DIR_BASE/bin/ -e $SDK_INSTALL_PREFIX/lib \
| awk '{print $3}' \
> $DPKG_TMP/deps

#ldd \
#$QTDIR/libexec/* \
#$QTDIR/plugins/*/*.so* \
#$QTDIR/qml/*/*.so* \
#| grep -e $QTDIR/lib \
#| awk '{print $3}' \
#>> $DPKG_TMP/deps

# Delete empty lines
grep -v '^$' $DPKG_TMP/deps > $DPKG_TMP/deps_noempty

# Delete duplicate entries
sort -u $DPKG_TMP/deps_noempty > $DPKG_TMP/deps
rm $DPKG_TMP/deps_noempty

while read DEP
do 
	cp $DEP $DPKG_TMP/data/opt/starviewer/lib
done < $DPKG_TMP/deps

# Package creation
tar czf $DPKG_TMP/data.tar.gz -C $DPKG_TMP/data .
tar czf $DPKG_TMP/control.tar.gz -C $DPKG_TMP/control .

ar -r $DPKG_DESTINATION/starviewer.deb $DPKG_TMP/debian-binary $DPKG_TMP/control.tar.gz $DPKG_TMP/data.tar.gz

rm -rf $DPKG_TMP

