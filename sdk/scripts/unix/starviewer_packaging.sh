#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. "$SCRIPTS_ROOT/config/setup.sh"

# Notes:
# - Due to Debian restrictions, tha version should always start with a number.
# - Architecture must match a Debian one.

PACKAGER_PROD_NAME=Starviewer
PACKAGER_PROD_VERSION=1.1.0-beta1
PACKAGER_DIST_ARCH=amd64


if [[ $(uname) == 'Linux' ]]
then
    # Fedora 32
#    PACKAGER_DIST_NAME='Fedora~32'
#    PACKAGER_DIST_ARCH=x86_64
#    PACKAGER_DEPENDS=''
#    . "$SCRIPTS_ROOT/packagers/rpm.sh"
#    exit
    
    
    # Debian 10 (Buster)
#    PACKAGER_DIST_NAME='Debian~buster'
#    PACKAGER_DIST_ARCH=amd64
#    PACKAGER_DEPENDS='libatk1.0-0:amd64, libatk-bridge2.0-0:amd64, libatspi2.0-0:amd64, libblkid1:amd64, libbsd0:amd64, libc6:amd64, libcairo2:amd64, libcairo-gobject2:amd64, libdatrie1:amd64, libdbus-1-3:amd64, libdrm2:amd64, libegl1:amd64, libepoxy0:amd64, libexpat1:amd64, libffi6:amd64, libfontconfig1:amd64, libfreetype6:amd64, libfribidi0:amd64, libgcc1:amd64, libgcrypt20:amd64, libgdk-pixbuf2.0-0:amd64, libgl1:amd64, libglib2.0-0:amd64, libglu1-mesa:amd64, libglvnd0:amd64, libglx0:amd64, libgpg-error0:amd64, libgraphite2-3:amd64, libgtk-3-0:amd64, libgtk3-nocsd0:amd64, libharfbuzz0b:amd64, libice6:amd64, liblz4-1:amd64, liblzma5:amd64, libmount1:amd64, libopengl0:amd64, libpango-1.0-0:amd64, libpangocairo-1.0-0:amd64, libpangoft2-1.0-0:amd64, libpcre3:amd64, libpixman-1-0:amd64, libpng16-16:amd64, libselinux1:amd64, libsm6:amd64, libstdc++5:amd64, libstdc++6:amd64, libsystemd0:amd64, libthai0:amd64, libuuid1:amd64, libwayland-client0:amd64, libwayland-cursor0:amd64, libwayland-egl1:amd64, libwayland-server0:amd64, libx11-6:amd64, libx11-xcb1:amd64, libxau6:amd64, libxcb1:amd64, libxcb-glx0:amd64, libxcb-render0:amd64, libxcb-shm0:amd64, libxcb-xkb1:amd64, libxcomposite1:amd64, libxcursor1:amd64, libxdamage1:amd64, libxdmcp6:amd64, libxext6:amd64, libxfixes3:amd64, libxi6:amd64, libxinerama1:amd64, libxkbcommon0:amd64, libxkbcommon-x11-0:amd64, libxrandr2:amd64, libxrender1:amd64, libxt6:amd64, zlib1g:amd64'
#    . "$SCRIPTS_ROOT/packagers/dpkg.sh"
    # Debian 11 (Bullseye)

    # Ubuntu 20.04 LTS (Focal Fossa)
    PACKAGER_DIST_NAME='Ubuntu~20.04'
    PACKAGER_DIST_ARCH=amd64
    PACKAGER_DEPENDS='libatk1.0-0:amd64, libatk-bridge2.0-0:amd64, libatspi2.0-0:amd64, libblkid1:amd64, libbsd0:amd64, libc6:amd64, libcairo2:amd64, libcairo-gobject2:amd64, libdatrie1:amd64, libdbus-1-3:amd64, libdrm2:amd64, libegl1:amd64, libepoxy0:amd64, libexpat1:amd64, libffi7:amd64, libfontconfig1:amd64, libfreetype6:amd64, libfribidi0:amd64, libgcc-s1:amd64, libgcrypt20:amd64, libgdk-pixbuf2.0-0:amd64, libgl1:amd64, libglib2.0-0:amd64, libglu1-mesa:amd64, libglvnd0:amd64, libglx0:amd64, libgpg-error0:amd64, libgraphite2-3:amd64, libgtk-3-0:amd64, libharfbuzz0b:amd64, libice6:amd64, liblz4-1:amd64, liblzma5:amd64, libmount1:amd64, libopengl0:amd64, libpango-1.0-0:amd64, libpangocairo-1.0-0:amd64, libpangoft2-1.0-0:amd64, libpcre2-8-0:amd64, libpcre3:amd64, libpixman-1-0:amd64, libpng16-16:amd64, libselinux1:amd64, libsm6:amd64, libstdc++5:amd64, libstdc++6:amd64, libsystemd0:amd64, libthai0:amd64, libuuid1:amd64, libwayland-client0:amd64, libwayland-cursor0:amd64, libwayland-egl1:amd64, libwayland-server0:amd64, libx11-6:amd64, libx11-xcb1:amd64, libxau6:amd64, libxcb1:amd64, libxcb-glx0:amd64, libxcb-render0:amd64, libxcb-shm0:amd64, libxcb-xkb1:amd64, libxcomposite1:amd64, libxcursor1:amd64, libxdamage1:amd64, libxdmcp6:amd64, libxext6:amd64, libxfixes3:amd64, libxi6:amd64, libxinerama1:amd64, libxkbcommon0:amd64, libxkbcommon-x11-0:amd64, libxrandr2:amd64, libxrender1:amd64, libxt6:amd64, zlib1g:amd64'
    . "$SCRIPTS_ROOT/packagers/dpkg.sh"

    # Universal compressed file
#    PACKAGER_DIST_NAME='GNULinux~portable'
#    . "$SCRIPTS_ROOT/packagers/archive.sh"
    
    
fi

