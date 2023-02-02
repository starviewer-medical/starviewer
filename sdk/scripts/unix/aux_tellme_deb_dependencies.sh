#!/bin/bash
SCRIPTS_ROOT=$(readlink -f $(dirname $BASH_SOURCE))
. $SCRIPTS_ROOT/config/setup.sh

# You may find useful the following command to process the output of this script:
# grep -vFe '-dev' -e '-dbg' dependencies_list.txt | xargs | sed 's~\ ~:amd64, ~g' | sed 's~$~:amd64~g'

echo '
This script will print all the system dependencies (package names) for the current operating system variant and version.
- You will be asked root permissions in order to install apt-file command and update its cache.
- The script assumes that you somehow installed everything nou needed to successfully run what is inside the starviewer-install directory.
- This script shall work in any Debian-based distribution; even Ubuntu whose package structure differs.
- The script assumes your system libraries have paths with no spaces.
- The script will probably output -dev dependencies too, which are unecessary for binary distribution.
- It will take a while to execute (several minutes).
- This script has no brain; use yours. Review the result.
'

sudo apt-get install apt-file
sudo apt-file update

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
xargs -I {} apt-file search "{}" | \
cut -d : -f 1 | \
sort | \
uniq

popd
