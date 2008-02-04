#!/bin/bash
#
# Utilitat per enviar testos i fer comprovacions d'estil

DARTURL=http://trueta.udg.edu/dart
DARTBOARD="Starviewer"
SITE=`hostname -f`

# Programes necessaris
CTEST=/usr/bin/ctest
QMAKE=/usr/bin/qmake
KWSTYLE=/usr/bin/KWStyle

# Important!!! Sinó ctest no parseja bé els warnings
export LC_ALL=en_US.UTF-8
export LANG=$LC_ALL
export LANGUAGE=$LC_ALL

ARG2=$2

# Funcions
Error() {
	echo "Error: $1" >&2
	echo >&2
	exit $2
}

Usage() {
    echo "$0: <tipus_build> [nom_build]" >&2
    echo >&2
    exit 1
}

# Verifica l'existència de programes necessaris i la seva versió
CheckRequired() {
    [ ! -e $CTEST ] && CTEST=`which ctest`
    [ ! -e $QMAKE ] && QMAKE=`which qmake`
    [ ! -e $KWSTYLE ] && KWSTYLE=`which KWStyle`

    [ -z "$CTEST" ] && Error "CTest not found" 2
    [ -z "$QMAKE" ] && Error "QMake not found" 2
    [ -z "$KWSTYLE" ] && Error "KWStyle not found" 2

    $QMAKE --version 2>&1 | grep "4.2" || Error "Qt version 4.2" 3
}

# Configura el Buildname amb la release de la distribució i compilador
SetBuildFromOS() {
    DISTRO=`cat /etc/issue | grep -v "^$" | sed 's%\\\\.%%g'`
    GCCRELEASE=`gcc -dumpversion`
    BUILDNAME="$DISTRO gcc $GCCRELEASE"
    SetBuildFromUserArg 
}

# Configura el Buildname amb la revisio SVN del codi
SetBuildFromSVN() {
    if svn info &>/dev/null
    then
        SVN=`svn info | grep URL`
        REVISION=`svn info|grep Revision|cut -f2 -d' '`
        SVN=${SVN%%/starviewer}
        SVN=${SVN##URL: https://trueta.udg.edu/repos/starviewer/}
	    
        # Check if tainted source
        if [ `svn status src/ | grep "^M" | wc -l` -eq 0 ]
        then
            TAINTED=""
        else
            TAINTED="-T"
        fi
        BUILDNAME=${SVN}-${REVISION}${TAINTED}
    fi
    SetBuildFromUserArg
}

# Configura el buildname amb l'argument passat per l'usuari si existex
SetBuildFromUserArg() {
    [ ! -z "$ARG2" ] && BUILDNAME=$ARG2
}

# Crea fitxer de configuració per fer submit al Dart
# (només necessari per a CTest)
CreateCTestConfig() {
    # Per evitar problemes hem de fer 'strip' d'algunes barres
    DARTURL=${DARTURL%%/}
    DARTBOARD=${DARTBOARD%%/}
    DARTBOARD=${DARTBOARD##/}
    cat > CTestConfiguration.ini << _EOF_DART_CONFIG
SourceDirectory: .
BuildDirectory: .
Site: $SITE
BuildName: $BUILDNAME
DropMethod: xmlrpc
DropSite: $DARTURL
DropLocation: $DARTBOARD
NightlyStartTime: 00:00:00 CET
ConfigureCommand: qmake
MakeCommand: make
UpdateCommand: /usr/bin/svn
UpdateOptions: 
UpdateType: svn
TimeOut: 1500
_EOF_DART_CONFIG
}


### MAIN SCRIPT

CheckRequired
case "$1" in
    style)
        $KWSTYLE -xml kws.xml -html KWStyle -lesshtml -D kwsFiles.txt
        ;;
    Continuous)
        SetBuildFromSVN
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Continuous
        ;;
    Nightly)
        SetBuildFromSVN
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Nightly
        BUILDNAME="${BUILDNAME%%gcc*} Style"
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Style
        ;;
    help)
        Usage
        ;;
    *|Experimental)
        SetBuildFromOS
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Experimental
        ;;
esac
