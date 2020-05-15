#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

echo '
This script will print all the system dependencies (package names) for the current operating system variant and version.
- You will be asked root permissions in order to install apt-file command and update its cache.
- The script assumes that you somehow installed everything nou needed to successfully run what is inside the starviewer-install directory.
- This script shall work in any Debian-based distribution; even Ubuntu whose package structure differs.
- The script assumes your system libraries have paths with no spaces.
- This script has no brain; use yours. Review the result before using it to fill starviewer_packaging.sh
'

sudo apt-get install apt-file
sudo apt-file update

pushd $STARVIEWER_INSTALL_DIR_BASE

# Warning: this is just one line, but made in to multiple using the backslashes.
find -P -type f -name 'starviewer' -or -name 'starviewer_sapwrapper' -or -name 'starviewer_crashreporter' -or -name '*.so*' -exec ldd {} \; | \
grep -F -e '=> /lib' -e '=> /usr/lib' - | \
cut -f2 | \
cut -d ' ' -f3 | \
sort | \
uniq | \
xargs -I {} apt-file search --fixed-string {} | \
cut -d : -f 1 | \
sort | \
uniq

popd
