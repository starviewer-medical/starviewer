/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/ 
#ifndef UDGSTUDYVOLUM_H
#define UDGSTUDYVOLUM_H

#include <vector>
#include "seriesvolum.h"

class string;

namespace udg {

/** Aquesta classe és permet organitzar els estudis per a que l'starviewer els pugui visualitzar
@author marc
*/
class StudyVolum
{

public:
    StudyVolum();
    
    /** Permet afegir una sèrie a aquest estudi. S'utilitza per especificar quines sèries te un estudi
     * @param SeriesVolum SeriesVolum a afegir
     */
    void addSeriesVolum(SeriesVolum);

    /** Retorna el número de series que conte l'estudi
     * @return retorna el numero de series de l'estudi
     */
    int getNumberOfSeries();

    /** Estableix el UID de l'estudi
     * @param UID de l'estudi
     */
    void setStudyUID( std::string );

    /** Estableix el Id de l'estudi
     * @param Id de l'estudi
     */
    void setStudyId( std::string );

    /** Estableix el temps en que es va realitzar l'estudi
     * @param Hora en que es va realitzar l'estudi
     */
    void setStudyTime( std::string );

    /** Estableix la data en que es va realitzar l'estudi
     * @param data de l'estudi
     */
    void setStudyDate( std::string );

    /** Estableix el nom del pacient de l'estudi
     * @param nom del pacient
     */
    void setPatientName( std::string );

    /** Estableix el Id del pacient
     * @param Id del pacient
     */
    void setPatientId( std::string );

    /** Estableix l'edat del pacient al realitzar-li l'estudi
     * @param Edat del pacient al realtizar-se de l'estudi
     */
    void setPatientAge( std::string );

    /** Estableix quina sera la serie per defecte s'obrira a l'starviewer al visualitzar l'estudi
     * @param UID de la serie a visualitzar
     */
    void setDefaultSeriesUID( std::string );
    

    /** retorna el UID de l'estudi
     * @return UID de l'estudi
     */
    std::string getStudyUID();

    /** retorna el Id de l'estudi
     * @return Id de l'estudi
     */
    std::string getStudyId();

    /** retorna el hora en que s'ha realitzat l'estudi
     * @return hora de l'estudi
     */
    std::string getStudyTime();

    /** retorna la data en que s'ha realitzat l'estudi
     * @return data de l'estudi
     */
    std::string getStudyDate();

    /** retorna el nom del pacient a qui s'ha realitzat l'estudi
     * @return nom del pacient
     */
    std::string getPatientName();

    /** retorna el Id del pacient a qui s'ha realitzat l'estudi
     * @return Id del pacient
     */
    std::string getPatientId();

    /** retorna el l'edat del pacient al realitzar-li l'estudi
     * @return edat del pacient
     */
    std::string getPatientAge();

    /** retorna la serie que s'ha d'obrir de l'estudi en el visualitzador per defecte
     * @return UID de la sèrie a visualitzar
     */
    std::string getDefaultSeriesUID();
    
    /// Es situa a la primera serie de l'estudi
    void firstSerie();

    /// seguent serie
    void nextSerie();

    /** indica si s'ha arribat al final de la llista
     * @return boolea indicant si hem arribat al final de la llista de series
     */
    bool end();
    
    /** retorna l'actual seriesVolum al que apunta la llista
     * @return retorna el seriesVolum al que s'apunta
     */
    SeriesVolum getSeriesVolum();

    ///Destructor de la classe    
    ~StudyVolum();

private: 

    std::string m_studyUID;
    std::string m_studyId;
    std::string m_studyTime;
    std::string m_studyDate;
    std::string m_patientName;
    std::string m_patientId;
    std::string m_patientAge;
    std::string m_defaultSeriesUID;//conté el UID de la serie que s'ha visualitzar per defecte, ja que quant visualitzem només s'obrira una sèrie de l'estudi
    
    std::vector <SeriesVolum> m_vectorStudyVolum;
    std::vector <SeriesVolum>::iterator i;
};

};

#endif
