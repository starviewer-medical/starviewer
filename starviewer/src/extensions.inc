# Aquí cal afegir les extensions que volem que s'incloguin a l'executable
#

PLAYGROUND_EXTENSIONS = diffusionperfusionsegmentation \
                        mpr2d \
                        edemasegmentation \
                        landmarkregistration \
                        vsireconstruction \
                        perfusionmapreconstruction \
                        glialestimation \
                        rectumsegmentation \
                        volume3dviewtesting \
                        experimental3d

# Només per sistemes no windows. Temporal mentre no s'empaqueta GLEW per windows
!win32{
    PLAYGROUND_EXTENSIONS += gputesting
}

#CONTRIB_EXTENSIONS =

MAIN_EXTENSIONS = mpr \
                  mpr3d \
                  q2dviewer

#
# En cas que fem un build de la versió "lite" només tindrem l'estensió 2D i prou
#
lite_version {
	PLAYGROUND_EXTENSIONS = ''
	MAIN_EXTENSIONS = q2dviewer	
}
