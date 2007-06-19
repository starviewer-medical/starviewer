
TEMPLATE = subdirs 

SUBDIRS = diffusionperfusionsegmentation \
          edemasegmentation \
          landmarkregistration \
          strokesegmentation \
          rectumsegmentation \
          optimalviewpoint \
          volume3dviewtesting

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
