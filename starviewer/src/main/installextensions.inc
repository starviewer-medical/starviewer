# Afegim dependències de les extensions
# Include que ens serveix per tal de no haver d'afegir manualment els include's de les extensions perquè
# compilin correctament.
# Genera el fitxer extensions.h
#

include(../extensions.inc)

extensionsFileName = extensions.h

# Funció per afegir una llibreria estàtica com a dependència
defineReplace(addExtensionInclude) {
    directoryName = $$1
    extensionName = $$2
    exists($$directoryName/$$extensionName) {
      win32:system(echo $${LITERAL_HASH}include \"$$directoryName/$$extensionName/$${extensionName}extensionmediator.h\">>$$extensionsFileName)
      unix:system(echo \'$${LITERAL_HASH}include \"$$directoryName/$$extensionName/$${extensionName}extensionmediator.h\"\'>>$$extensionsFileName)
    }

    return(0)
}

defineReplace(addExtensionResourcesInit) {
    directoryName = $$1
    extensionName = $$2
    exists($$directoryName/$$extensionName) {
      win32:system(echo "Q_INIT_RESOURCE($$extensionName);">>$$extensionsFileName)
      unix:system(echo '"Q_INIT_RESOURCE($$extensionName);"'>>$$extensionsFileName)
    }

    return(0)
}

win32:system(echo "$${LITERAL_HASH}ifndef EXTENSIONS_H"> $$extensionsFileName)
win32:system(echo "$${LITERAL_HASH}define EXTENSIONS_H">> $$extensionsFileName)
unix:system(echo '"$${LITERAL_HASH}ifndef EXTENSIONS_H"'> $$extensionsFileName)
unix:system(echo '"$${LITERAL_HASH}define EXTENSIONS_H"'>> $$extensionsFileName)


# Afegim els include's
for(extensionName, MAIN_EXTENSIONS) {
    DUMMY = $$addExtensionInclude(../extensions/main, $$extensionName)
}
for(extensionName, CONTRIB_EXTENSIONS) {
    DUMMY = $$addExtensionInclude(../extensions/contrib, $$extensionName)
}
for(extensionName, PLAYGROUND_EXTENSIONS) {
    DUMMY = $$addExtensionInclude(../extensions/playground, $$extensionName)
}

# Afegim les inicialitzacions de resources

win32:system(echo "void initExtensionsResources()">> $$extensionsFileName)
unix:system(echo '"void initExtensionsResources()"'>> $$extensionsFileName)
system(echo "{">> $$extensionsFileName)

for(extensionName, MAIN_EXTENSIONS) {
    DUMMY = $$addExtensionResourcesInit(../extensions/main, $$extensionName)
}
for(extensionName, CONTRIB_EXTENSIONS) {
    DUMMY = $$addExtensionResourcesInit(../extensions/contrib, $$extensionName)
}
for(extensionName, PLAYGROUND_EXTENSIONS) {
    DUMMY = $$addExtensionResourcesInit(../extensions/playground, $$extensionName)
}

system(echo "}">> $$extensionsFileName)

win32:system(echo "$${LITERAL_HASH}endif" >> $$extensionsFileName)
unix:system(echo '"$${LITERAL_HASH}endif"' >> $$extensionsFileName)
