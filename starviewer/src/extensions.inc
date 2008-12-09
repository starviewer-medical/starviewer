# Aquí cal afegir les extensions que volem que s'incloguin a l'executable
#

PLAYGROUND_EXTENSIONS = diffusionperfusionsegmentation \
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
                        experimental3d

# Només per sistemes no windows. Temporal mentre no s'empaqueta GLEW per windows
!win32{
    PLAYGROUND_EXTENSIONS += gputesting
}

#CONTRIB_EXTENSIONS =

MAIN_EXTENSIONS = mpr \
                  mpr3d \
                  mpr3d2d \
                  q2dviewer
