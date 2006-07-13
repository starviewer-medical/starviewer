/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGREADDICOMDIR_H
#define UDGREADDICOMDIR_H

#define HAVE_CONFIG_H 1
#include "dcdicdir.h" //llegeix el dicom dir

class string;
class DcmDicomDir;

namespace udg {

class Status;
class StudyList;
class SeriesList;

/** Aquesta classe permet llegir un dicomdir i consultar-ne els seus elements
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ReadDicomdir{
public:

    ReadDicomdir();

    /** Obre un directori domcidr
     * @param dicomdirPath directori on es troba el dicomdir
     * @return estat del mètode
     */
    Status open( std::string dicomdirPath );
    
    /** Retorna la llista d'estudis que conté el dicomdir
      * @param studyList llista amb els estudis que conté el dicomdir
      * @return estat del mètode
      */
    Status readStudies( StudyList &studyList );
    
    /** Retorna la llista de sèries d'un estudi que estigui en el dicomdir
     * @param studyUID UID de l'estudi del qual es vol consultar les sèries
     * @param seriesList llista amb les sèries que conté l'estudi
     * @return estat del mètode
     */
    Status readSeries ( std::string studyUID , SeriesList  &serieList );

    ~ReadDicomdir();

private :

    DcmDicomDir *dicomdir;

};

}

#endif
