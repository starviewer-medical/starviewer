#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

echo '
This script will print all the system dependencies (package names) for the current operating system variant and version.
- The script assumes that you somehow installed everything nou needed to successfully run what is inside the starviewer-install directory.
- This script shall work in any Red Hat based distribution.
- The script assumes your system libraries have paths with no spaces.
- The script will probably output -devel dependencies too, which are unecessary for binary distribution.
- This script has no brain; use yours. Review the result before using it to fill starviewer_packaging.sh
'

pushd $STARVIEWER_INSTALL_DIR_BASE

# Warning: this is just one line, but made in to multiple using the backslashes.
# This is a little bit like functional programming paradigm but on bash.
# The regex removes the library version number after the .so
# The sh -c is used to run the given input in "parallel" (i.e each input line will become two, one for each command)
find -P -type f -name 'starviewer' -or -name 'starviewer_sapwrapper' -or -name 'starviewer_crashreporter' -or -name '*.so*' -exec ldd {} \; | \
grep -F -e '=> /lib' -e '=> /usr/lib' - | \
cut -f2 | \
cut -d ' ' -f3 | \
sort | \
uniq | \
xargs -I {} sh -c 'realpath "{}" ; echo "{}"' | \
xargs -I {} sh -c 'echo "{}" | sed '"'"'s~\.so\(\.[0-9]*\)*$~.so~'"'"' ; echo "{}"' | \
sort | \
uniq | \
xargs -I {} rpm -q --whatprovides {} --qf "%{NAME}\n" 2> /dev/null | \
sort | \
uniq

popd
