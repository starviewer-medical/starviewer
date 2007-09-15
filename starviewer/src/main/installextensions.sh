#!/bin/sh
# Script que ens serveix per tal de no haver d'afegir manualment els include's de les extensions perquè
# compilin correctament.
# Genera el fitxer extensions.h
#

writeIncludes()
{
    for include in `grep -Ir InstallExtension ../extensions/$1/*|grep -v .svn|grep -v \~|cut -d: -f1|grep .h$`
    do
        echo "#include \"$include\"">> extensions.h
    done
}

writeInitResources()
{
    for resource in `grep -Ir RESOURCES ../extensions/$1/*|grep -v .svn|grep -v \~|grep .pro|cut -d= -f2`
    do
        resourceName=${resource%.*}
        echo "Q_INIT_RESOURCE($resourceName);">> extensions.h
    done
}

writeInitResourcesFunction()
{
    echo "void initExtensionsResources()">> extensions.h
    echo "{">> extensions.h
    writeInitResources main
    writeInitResources contrib
    writeInitResources playground
    echo "}">> extensions.h
}

#Netejem el fitxer per si existeix
echo ' ' > extensions.h

echo '#ifndef EXTENSIONS_H' >> extensions.h
echo '#define EXTENSIONS_H' >> extensions.h

writeIncludes main
writeIncludes contrib
writeIncludes playground

writeInitResourcesFunction

echo '#endif' >> extensions.h
