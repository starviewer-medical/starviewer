
TEMPLATE = subdirs 

SUBDIRS = diffusionperfusionsegmentation \
          edemasegmentation \
          landmarkregistration \
          strokesegmentation \
          optimalviewpoint

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
