# Aquí cal afegir les extensions que volem que s'incloguin a l'executable
#

# Exclude playground extensions from official releases
!official_release {
    PLAYGROUND_EXTENSIONS = diffusionperfusionsegmentation \
                            edemasegmentation \
                            angiosubstraction \
                            perfusionmapreconstruction \
                            rectumsegmentation \
                            experimental3d \
                            example
}

#CONTRIB_EXTENSIONS =

MAIN_EXTENSIONS = mpr \
                  dicomprint \
                  q2dviewer \
                  q3dviewer \
                  pdf

#
# En cas que fem un build de la versió "lite" només tindrem l'estensió 2D i prou
#
lite_version {
	PLAYGROUND_EXTENSIONS = ''
	MAIN_EXTENSIONS = q2dviewer	
}

#No s'inclou l'extensió perquè a l'hora de fer el lincatge dóna error per duplicació de simbols
macx {
        PLAYGROUND_EXTENSIONS -= perfusionmapreconstruction \
}
