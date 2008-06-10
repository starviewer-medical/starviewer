/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGCACHESERIESDAL_H
#define UDGCACHESERIESDAL_H

namespace udg {

class DicomMask;
class DICOMSeries;
class SeriesList;
class Status;

/** Classe que conté els mètodes d'accés a la Taula series
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class CacheSeriesDAL
{

public:
    CacheSeriesDAL();

    /** Insereix una sèrie a la caché
     * @param series Dades de la sèrie
     * @return retorna l'estat de la inserció
     */
    Status insertSeries( DICOMSeries * serie );

    /** Cerca les sèries demanades a la màscara. Important! Aquesta acció només té en compte l'StudyUID de la màscara per fer la cerca, els altres camps de la màscara els ignorarà!
     * @param  mascarà de la serie amb l' sstudiUID a buscar
     * @param  retorna la llista amb la sèries trobades
     * @return retorna estat del mètode
     */
    Status querySeries( DicomMask mask , SeriesList &list );

    /** Esborra les series que tinguin el studyUID passat per paràmetre
      * @param studyUID StudyUID de les imatges a esborrar
      */
    Status deleteSeries ( QString studyUID );

    ~CacheSeriesDAL();

private:

    /** Construeix la sentència per buscar les sèries d'un estudi
     * @param mask màscara de cerca
     * @return sentència sql
     */
    QString buildSqlQuerySeries( DicomMask *dicomMask );

};

}

#endif
