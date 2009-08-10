/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGISOIMAGEFILECREATOR_H
#define UDGISOIMAGEFILECREATOR_H

class QString;

namespace udg {

/** Crea un arxiu d'imatge ISO a partir d'un directori o fitxer.
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class IsoImageFileCreator
{
public:

    IsoImageFileCreator();

    /** Crea el fitxer d'imatge ISO output a partir del directori o fitxer output
     * @param input ruta completa del fitxer o directori a partir del qual es crearà la el fitxer d'imatge ISO.
     * @param output ruta completa indicant on s'ha de desar el fitxer d'imatge ISO creat, s'hi ha d'incloure el nom del fitxer.
     * @return true si la imatge s'ha creat correctament i false en cas contrari.
     */
    bool createIsoImageFile( QString input , QString output );

    ~IsoImageFileCreator();
};

}

#endif
