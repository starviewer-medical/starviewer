
# Cal afegir el directori de l'extensió a la variable SUBDIRS tant si es compila com si no.

SUBDIRS = diffusionperfusionsegmentation \
          edemasegmentation \
          landmarkregistration \
          strokesegmentation \
          optimalviewpoint \
          rectumsegmentation \
          volume3dviewtesting \
          mprcardiac3d \
          cardiac2dviewer \
          multiq2dviewer \
          segmentationframework \
          registrationframework 

include(../../extensions.inc)

TEMPLATE = subdirs

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
    !contains(PLAYGROUND_EXTENSIONS, $$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
