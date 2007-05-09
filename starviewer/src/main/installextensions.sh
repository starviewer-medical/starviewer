#!/bin/sh
# Script que ens serveix per tal de no haver d'afegir manualment els include's de les extensions perquè 
# compilin correctament.
# Genera el fitxer extensions.h
#

#Netejem el fitxer per si existeix
echo ' ' > extensions.h

echo '#ifndef EXTENSIONS_H' >> extensions.h
echo '#define EXTENSIONS_H' >> extensions.h

for include in `grep -Ir InstallExtension ../extensions/*|grep -v .svn|cut -d: -f1|grep .h$`
do
	echo "#include \"$include\"">> extensions.h
done

echo '#endif' >> extensions.h
