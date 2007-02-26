/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSCALEIMAGE_H
#define UDGSCALEIMAGE_H

/*  Aquesta classe és una modificació del programa dcmj2pnm. Per qualsevol dubte o per afegir funcions a aquesta classe consultar-la!!!
  */
///  Aquesta classe passant-li un fitxer dicom ens retorna una imatge de la mida de pixels especificada. Converteix la imatge a PGM i l'escala, reduint considerablement el tamany de la imatge
namespace udg {

class ScaleImage
{
public:

    ///Constructor de la classe
    ScaleImage();

    ///Destructor de la classe
    ~ScaleImage();

    /** Converteix un dicom a un pgm (Imatge que pot ser visualitzada amb qualsevol eina gràfica).
     * @param dicomFile path del dicom a convertir a png
     * @param lpgmFile path del fitxer pgm desti
     * @param pixelSize mida de pixels de l'imatge
     */
    int dicom2lpgm(const char* ,const char *,int);
};
};

#endif
