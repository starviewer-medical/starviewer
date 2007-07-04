/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTUDYVOLUM_H
#define UDGSTUDYVOLUM_H

#include "seriesvolum.h"

namespace udg {

/** Aquesta classe és permet organitzar els estudis per a que l'starviewer els pugui visualitzar
@author marc
*/
class StudyVolum {
public:

    StudyVolum();
    ~StudyVolum();

    /** Permet afegir una sèrie a aquest estudi. S'utilitza per especificar quines sèries te un estudi
     * @param SeriesVolum SeriesVolum a afegir
     */
    void addSeriesVolum( SeriesVolum seriesVolum );

    /** Retorna el número de series que conte l'estudi
     * @return retorna el numero de series de l'estudi
     */
    int getNumberOfSeries();

    /** Estableix el UID de l'estudi
     * @param UID de l'estudi
     */
    void setStudyUID( QString studyUID );

    /** Estableix el Id de l'estudi
     * @param Id de l'estudi
     */
    void setStudyId( QString studyId );

    /** Estableix el temps en que es va realitzar l'estudi
     * @param Hora en que es va realitzar l'estudi
     */
    void setStudyTime( QString time );

    /** Estableix la data en que es va realitzar l'estudi
     * @param data de l'estudi
     */
    void setStudyDate( QString date );

    /** Estableix el nom del pacient de l'estudi
     * @param nom del pacient
     */
    void setPatientName( QString name );

    /** Estableix el Id del pacient
     * @param Id del pacient
     */
    void setPatientId( QString patienId );

    /** Estableix l'edat del pacient al realitzar-li l'estudi
     * @param Edat del pacient al realtizar-se de l'estudi
     */
    void setPatientAge( QString age );

    /** Estableix quina sera la serie per defecte s'obrira a l'starviewer al visualitzar l'estudi
     * @param UID de la serie a visualitzar
     */
    void setDefaultSeriesUID( QString seriesUID );


    /** retorna el UID de l'estudi
     * @return UID de l'estudi
     */
    QString getStudyUID();

    /** retorna el Id de l'estudi
     * @return Id de l'estudi
     */
    QString getStudyId();

    /** retorna el hora en que s'ha realitzat l'estudi
     * @return hora de l'estudi
     */
    QString getStudyTime();

    /** retorna la data en que s'ha realitzat l'estudi
     * @return data de l'estudi
     */
    QString getStudyDate();

    /** retorna el nom del pacient a qui s'ha realitzat l'estudi
     * @return nom del pacient
     */
    QString getPatientName();

    /** retorna el Id del pacient a qui s'ha realitzat l'estudi
     * @return Id del pacient
     */
    QString getPatientId();

    /** retorna el l'edat del pacient al realitzar-li l'estudi
     * @return edat del pacient
     */
    QString getPatientAge();

    /** retorna la serie que s'ha d'obrir de l'estudi en el visualitzador per defecte
     * @return UID de la sèrie a visualitzar
     */
    QString getDefaultSeriesUID();

    /** retorna el seriesVolum que indiquem
     * @return retorna el seriesVolum que s'indica
     */
    SeriesVolum getSeriesVolum( int index );

private:

    QString m_studyUID;
    QString m_studyId;
    QString m_studyTime;
    QString m_studyDate;
    QString m_patientName;
    QString m_patientId;
    QString m_patientAge;
    QString m_defaultSeriesUID;//conté el UID de la serie que s'ha visualitzar per defecte, ja que quant visualitzem només s'obrira una sèrie de l'estudi

    /// Conté la llista de sèries del volum. Substitueix el membre obsolet std::vector <SeriesVolum> m_vectorStudyVolum;
    QList< SeriesVolum > m_studyVolumList;

};

};

#endif
