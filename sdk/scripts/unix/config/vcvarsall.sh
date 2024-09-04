#!/bin/bash
# Like msvc's vcvarsall.bat but made bash compatilbe (MSYS, Cygwin, MinGW, etc...) hence the name vcvarsall.sh

# SECURITY WARNING: This script evaluates environment variables (does bash eval/source), this may be dangerous if those are compromised maliciously.

# Summary of the process:
# 1. Run VCVARSALL.BAT in a CMD
# 2. Run the env command to extract the environment
# 3. Process the output text in order to set the variables as if they were declared in the current script

# Well... the following command is complicated, but basically does what is explained above.
COMMAND="\"C:\Program Files (x86)\Microsoft Visual Studio\2017\BuildTools\VC\Auxiliary\Build\vcvarsall.bat\" $@"
source <(( echo "$COMMAND" ; echo "bash -c 'echo \"echo VCVARSALLDOTSHBEGIN\" ; export -p ; echo \"echo VCVARSALLDOTSHEND\"'" ) | $COMSPEC | sed '/^echo VCVARSALLDOTSHBEGIN/,/^echo VCVARSALLDOTSHEND/!d')
