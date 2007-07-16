
TEMPLATE = subdirs 

SUBDIRS = diffusionperfusionsegmentation \
          edemasegmentation \
          landmarkregistration \
          strokesegmentation \
          rectumsegmentation \
          optimalviewpoint \
          volume3dviewtesting \
          mprcardiac3d \
          cardiac2dviewer \
          multiq2dviewer \
          segmentationframework

for(dir, SUBDIRS) {
    !exists($$dir) {
        SUBDIRS -= $$dir
    }
}

include(../../compilationtype.inc)
