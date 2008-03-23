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
    echo "Tipus de builds possibles:" >&2
    echo "  style           Fa la validació d'estil de tot el codi" >&2
    echo "  style <fitxers> Fa la validació d'estil dels fitxers indicats" >&2
    echo "  KWStyle         Test d'estil per al Dart" >&2
    echo "  Experimental    Test de compilació al track Experimental" >&2
    echo "  Nightly         Test de compilació diaria" >&2
    echo "  Continuous      Test de compilació continua (recompilació a cada canvi al SVN" >&2
    echo "  config          Generar fitxer de configuració" >&2
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

#    $QMAKE --version 2>&1 | grep "4.2" || Error "Qt version 4.2" 3
}

# Configura el Buildname amb la release de la distribució i compilador
SetBuildFromOS() {
    #DISTRO=`cat /etc/issue | grep -v "^$" | sed 's%\\\\.%%g'`
    DISTRO=`cat /etc/*release* | head -1`
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
        shift
        if [ -z "$1" ]
        then
            # Fem check complet
            $KWSTYLE -xml kws.xml -html KWStyle -lesshtml -D kwsFiles.txt
        else
            # Fem check dels fitxers indicats com a paràmetres
            $KWSTYLE -xml kws.xml -html KWStyle -lesshtml $*
        fi
        ;;
    Continuous)
        # Continuous Test
        SetBuildFromOS
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Continuous
        ;;
    Nightly)
        # Nightly Test
        SetBuildFromOS
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Nightly
        ;;
    KWStyle)
        # Style Test
        SetBuildFromSVN
        BUILDNAME="$BUILDNAME Style"
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Style
        ;;
    config)
        # Generar configuració
        SetBuildFromSVN
        CreateCTestConfig
        ;;
    Experimental)
        # Experimental Test
        SetBuildFromOS
        CreateCTestConfig
        $CTEST -VV -S CTest.cmake,Experimental
        ;;
    *|help)
        Usage
        ;;
esac
