# Fitxer generat pel gestor de qmake de kdevelop.
# -------------------------------------------
# Subdirectori relatiu al directori principal del projecte: ./src/extensions/playground
# L'objectiu �s un subdirectori del projecte

SUBDIRS += diffusionperfusionsegmentation \
           mpr2d \
           edemasegmentation \
           landmarkregistration \
           strokesegmentation \
           vsireconstruction \
           perfusionmapreconstruction \
           glialestimation \
           optimalviewpoint \
           rectumsegmentation \
           volume3dviewtesting \
           mprcardiac3d \
           cardiac2dviewer \
           volumecontourdelimiter \
           lesionvisualization \
           experimental3d \
           gputesting
include(../../extensions.inc)
TEMPLATE = subdirs
include(../../compilationtype.inc)
for(dir, SUBDIRS){
  !exists($$dir){
    SUBDIRS -= $$dir
  }
  !contains(PLAYGROUND_EXTENSIONS, $$dir){
    SUBDIRS -= $$dir
  }
}
