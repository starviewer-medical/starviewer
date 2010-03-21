# Aquí cal afegir les extensions que volem que s'incloguin a l'executable
#

PLAYGROUND_EXTENSIONS = volume3dviewtesting

#CONTRIB_EXTENSIONS =

MAIN_EXTENSIONS = mpr \
                  q2dviewer

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
