#!/bin/bash
# Those variables are meant to be set externally by starviewer_packaging.sh
# If not those "generalistic" defaults are picked to create a .deb that can be put anywhere, although it will probably not run smoothly as no system dependencies are specified.
# RPM packaging inspired by https://stackoverflow.com/questions/880227/what-is-the-minimum-i-have-to-do-to-create-an-rpm-file
PACKAGER_DIST_NAME=${PACKAGER_DIST_NAME:-anydist}
PACKAGER_DIST_ARCH=${PACKAGER_DIST_ARCH:-'noarch'}
PACKAGER_DEPENDS=${PACKAGER_DEPENDS:-''}

PACKAGER_PROD_NAME=${PACKAGER_PROD_NAME:-product}
PACKAGER_PROD_VERSION=${PACKAGER_PROD_VERSION:-version}

# TODO: sed a la versió per evitar els guions
# TODO: potser canviar tot arreu on surt data per Name-Version-Release
# TODO: veure si es molt important el «release» potser millor fer que vagi amb timestamp
# 

RPM_NAME=$PACKAGER_PROD_NAME
RPM_VERSION='1.0.0_devel'
RPM_RELEASE=1
RPM_ARCH=$PACKAGER_DIST_ARCH

OUTPUT_DIR=$STARVIEWER_PACKAGING_DIR_BASE/${PACKAGER_PROD_NAME}-${PACKAGER_PROD_VERSION}~${PACKAGER_DIST_NAME}_${PACKAGER_DIST_ARCH}

mkdir -p $OUTPUT_DIR
pushd $OUTPUT_DIR
echo "Packaging $OUTPUT_DIR"

# Data directory
# ==============
mkdir -p {RPMS,SRPMS,BUILD,SOURCES,SPECS,tmp}
mkdir SOURCES/${RPM_NAME}-${RPM_VERSION}

mkdir -p SOURCES/${RPM_NAME}-${RPM_VERSION}/opt/starviewer
cp -r $STARVIEWER_INSTALL_DIR_BASE/* SOURCES/${RPM_NAME}-${RPM_VERSION}/opt/starviewer

mkdir -p SOURCES/${RPM_NAME}-${RPM_VERSION}/usr/bin
ln -s ../../opt/starviewer/starviewer.sh SOURCES/${RPM_NAME}-${RPM_VERSION}/usr/bin/starviewer

mkdir -p SOURCES/${RPM_NAME}-${RPM_VERSION}/usr/share/applications
echo "[Desktop Entry]
Version=$PACKAGER_PROD_VERSION
Type=Application
Name=$PACKAGER_PROD_NAME (${PACKAGER_PROD_VERSION})
Comment=Medical imaging DICOM image viewer.
Exec=../../../opt/starviewer/starviewer.sh
Icon=../../../opt/starviewer/logo/64.svg
Categories=Graphics;Science;Biology;Medical;
" > SOURCES/${RPM_NAME}-${RPM_VERSION}/usr/share/applications/starviewer.desktop

# SPECS directory
# ===============

echo "
# Don't try fancy stuff like debuginfo, which is useless on binary-only
# packages. Don't strip binary too
# Be sure buildpolicy set to do nothing
%define        __spec_install_post %{nil}
%define          debug_package %{nil}
%define        __os_install_post %{_dbpath}/brp-compress

Name: $RPM_NAME
Version: $RPM_VERSION
Release: $RPM_RELEASE
Group: Graphics
Summary: Medical imaging DICOM viewer.
License: GPLv3+
URL: http://starviewer.org/
BuildArch: $RPM_ARCH

SOURCE0 : %{name}-%{version}.tar.gz
BuildRoot: %{_tmppath}/%{name}-%{version}-%{release}-root

%description
%{summary}

%prep
%setup -q

%build
# Empty section.

%install
rm -rf %{buildroot}
mkdir -p %{buildroot}

# in builddir
cp -a * %{buildroot}


%clean
rm -rf %{buildroot}


%files
%defattr(-,root,root,-)
/usr/bin/starviewer
/opt/starviewer/*
/usr/share/applications/starviewer.desktop

" > SPECS/${RPM_NAME}-${RPM_VERSION}.spec


# Packaging
# =========

# This tar must contain the top directory specified by this command.
tar czf SOURCES/${RPM_NAME}-${RPM_VERSION}.tar.gz --no-acls --no-selinux --no-xattrs --no-same-owner --owner=0 --group=0 -C SOURCES ${RPM_NAME}-${RPM_VERSION}/
#TODO: mirar si es pot treure aquest rm
rm -r SOURCES/${RPM_NAME}-${RPM_VERSION}

rpmbuild -v -bb --define "_topdir $(pwd)" --define "_tmppath $(pwd)/tmp" SPECS/${RPM_NAME}-${RPM_VERSION}.spec

popd
