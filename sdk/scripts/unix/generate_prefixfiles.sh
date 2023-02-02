#!/bin/bash
SCRIPTS_ROOT=$(cd $(dirname $BASH_SOURCE) && pwd)
. "$SCRIPTS_ROOT/config/setup.sh"

# Prefix files define the essential variables to compile; they are useful to:
# - be copied and pasted to Qt Creator environment variables section
# - be used with the command "source" in a shell in order to get the environment nedded to compile. When doing so, it is assumed that the compiler toolchain (for example vcvarsall) is already set.

# Things to at the beginning of the prefix files
# -----------------------------------------------------------------------------
echo "Generating $SDK_ENVIRONMENT_FILE"
echo "#!/bin/bash
# Environment variables to compile with the SDK
# To enter inside the environment run this file using 'source' command.
# Better avoid manually editing this file. Rerun the sdk_build.sh script instead.
echo 'Remember, invoke CMake with the following extra arguments when compiling Starviewer:'
echo '$STARVIEWER_CMAKE_ARGUMENTS'
" > $SDK_ENVIRONMENT_FILE

if [[ $(uname) == 'MSYS_NT'* ]]
then
    echo "Generating $SDK_WINDOWS_ENVIRONMENT_FILE"
    echo "
:: Environment variables to compile with the SDK
:: Better avoid manually editing this file. Rerun the sdk_build.sh script instead.
echo Remember, invoke CMake with the following extra arguments when compiling Starviewer:
echo $STARVIEWER_CMAKE_ARGUMENTS
    " > $SDK_WINDOWS_ENVIRONMENT_FILE
fi

# Things to add in the middle of the prefix files
# -----------------------------------------------------------------------------

function addToPrefixFiles() {
    VARIABLE=$1
    VALUE=${!VARIABLE}
    ISPATH=$2
    
    echo "declare -x $VARIABLE='$VALUE'" >> $SDK_ENVIRONMENT_FILE

    # We also generate a windows bath prefix file (.bat)
    if [[ $(uname) == 'MSYS_NT'* ]]
    then
        if [[ -z $ISPATH ]]
        then
            echo "set $VARIABLE=`cygpath -w \"$VALUE\"`" >> $SDK_WINDOWS_ENVIRONMENT_FILE
        else
            echo "set $VARIABLE=`cygpath -pw \"$VALUE\"`" >> $SDK_WINDOWS_ENVIRONMENT_FILE
        fi
        
    fi
}

addToPrefixFiles PREPEND_TO_LD_LIBRARY_PATH 1
addToPrefixFiles PREPEND_TO_PATH 1

for VARIABLE in ${VARS_TO_EXPORT[@]}
do
    addToPrefixFiles $VARIABLE
done

# Generate a list of plain variable values to be copied and pasted to QtCreator
# -----------------------------------------------------------------------------

grep '^declare -x ' $SDK_ENVIRONMENT_FILE | sed 's/^declare -x //' > $SDK_ENVIRONMENT_FILE.txt
if [[ $(uname) == 'MSYS_NT'* ]]
then
    grep '^set ' $SDK_WINDOWS_ENVIRONMENT_FILE | sed 's/^set //' > $SDK_WINDOWS_ENVIRONMENT_FILE.txt
fi


# Things to add at the end of the prefix files
# -----------------------------------------------------------------------------

echo "
PATH=\$PREPEND_TO_PATH\${PATH:+:\${PATH}}
export PATH
LD_LIBRARY_PATH=\$PREPEND_TO_LD_LIBRARY_PATH\${LD_LIBRARY_PATH:+:\${LD_LIBRARY_PATH}}
export LD_LIBRARY_PATH
" >> $SDK_ENVIRONMENT_FILE
chmod ugo+x $SDK_ENVIRONMENT_FILE

if [[ $(uname) == 'MSYS_NT'* ]]
then
    echo "
set PATH=%PREPEND_TO_PATH%;%PATH%
set LD_LIBRARY_PATH=%PREPEND_TO_LD_LIBRARY_PATH%;%LD_LIBRARY_PATH%
    " >> $SDK_WINDOWS_ENVIRONMENT_FILE
fi
