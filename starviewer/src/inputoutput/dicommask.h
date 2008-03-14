/***************************************************************************
 *   Copyright (C) 2005 by marc                                            *
 *   marc@localhost.com                                                    *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/

#ifndef DICOMMASK
#define DICOMMASK

class QString;
class DcmDataset;

/** Aquesta classe construeix una màscara per la cerca d'estudis.
   *Cada camp que volem que la cerca ens retorni li hem de fer el set, sinó retornarà valor null per aquell camp. Per tots els camps podem passar, cadena buida que significa que buscarà tots els valors d'aquell camp, o passar-li un valor en concret, on només retornarà els compleixin aquell criteri.
 */
namespace udg{

class Status;

class DicomMask
{

public:

    DicomMask();

    /** This action especified that in the search we want the Patient's Name
     * @param Name of the patient to search.
     * @return state of the method
     */
    Status setPatientName( QString );

    /** Aquest mètode especifica la data de naixement del pacient amb la que s'han de cercar els estudis. El format és DDMMYYYY
     * Si el paràmetre no té valor, busca per totes les dates
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param  Study's Data de naixement del pacient
     * @return state of the method
     */
    Status setPatientBirth(QString date);

    /** This action especified that in the search we want the Patient's sex
     * @param Patient's sex of the patient to search.
     * @return state of the method
     */
    Status setPatientSex(QString);

    /** This action especified that in the search we want the Patient's ID
     * @param   patientID Patient's ID of the patient to search.
     * @return state of the method
     */
    Status setPatientId(QString);

    /** This action especified that in the search we want the Patient's Age
     * @param  Patient's age of the patient to search.
     * @return state of the method
     */
    Status setPatientAge(QString);

    /** This action especified that in the search we want the Study's Id
     * @param  Study's Id of the study to search.
     * @return state of the method
     */
    Status setStudyId(QString);

    /** Aquest mètode especifica la data amb la que s'han de cercar els estudis. El format és DDMMYYYY
     * Si el paràmetre no té valor, busca per totes les dates
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param  Study's date of the study to search.
     * @return state of the method
     */
    Status setStudyDate(QString date);

    /** This action especified that in the search we want the Study's description
     *  @param Study's description of the study to search.
     *  @return state of the method
     */
    Status setStudyDescription(QString);

    /** This action especified that in the search we want the Study's modality
     * @param Study's modality the study to search.
     * @return state of the method
     */
    Status setStudyModality(QString);

    /** Especifica l'hora de l'estudi a buscar, les hores es passen en format HHMM
     *         Si es buit busca per qualsevol hora.
     *         Si se li passa una hora com 1753, buscarà series d'aquella hora
     *         Si se li passa una hora amb guió a davant, com -1753 , buscarà sèries d'aquella hora o fetes abans
     *         Si se li passa una hora amb guió a darrera, com 1753- , buscarà series fetes a partir d'aquella hora
     *         Si se li passa dos hores separades per un guió, com 1223-1753 , buscarà series fetes entre aquelles hores
     * @param  Hora de l'estudi
     * @retun estat del mètode
     */
    Status setStudyTime(QString);

    /** This action especified that in the search we want the Study's instance UID
     * @param Study's instance UID the study to search.
     * @return state of the method
     */
    Status setStudyUID(QString);

    /** This action especified that in the search we want the Institution name
     * @param Institution name of the study to search.
     * @return state of the method
     */
    Status setInstitutionName(QString);

    /** This action especified that in the search we want the Accession Number
     * @param Accession Number of the study to search.
     * @return state of the method
     */
    Status setAccessionNumber(QString);

    /** Establei el ReferringPhysiciansName
     * @param physiciansName
     * @return estat del mètode
     */
    Status setReferringPhysiciansName( QString physiciansName );

    /** Retorna de la màscara l'estudi UID
     * @param mask [in] màscara de la cerca
     * @return   Estudi UID que cerquem
     */
    QString getStudyUID() const;

    /** Retorna el Id del pacient a buscar
     * @return   ID del pacient
     */
    QString getPatientId() const;

    /** Retorna el nom del pacient
     * @return Nom del pacient
     */
    QString getPatientName() const;

    /** Retorna la data naixement
     * @return Data de naixament del pacient
     */
    QString getPatientBirth() const;

    /** Retorna l'edat de pacient
     * @return edat del pacient
     */
    QString getPatientAge() const;

    /** Retorna el sexe del pacient
     * @return sexe del pacient
     */
    QString getPatientSex() const;

    /** Retorna Id de l'estudi
     * @return   ID de l'estudi
     */
    QString getStudyId() const;

    /** Retorna la data de l'estudi
     * @return   Data de l'estudi
     */
    QString getStudyDate() const;

    /** Retorna la descripció de l'estudi
     * @return   descripció de l'estudi
     */
    QString getStudyDescription() const;

    /** Retorna de la modalitat de l'estudi
     * @return   Modalitat de l'estudi
     */
    QString getStudyModality() const;

    /** Retorna l'hora de l'estudi
     * @return   hora de l'estudi
     */
    QString getStudyTime() const;

    /** Retorna el accession number de l'estudi
     * @return   accession number de l'estudi
     */
    QString getAccessionNumber() const;

    /** Retorna el nom de l'institució on s'ha realitzat l'estudi
     * @return   Nom de l'institucio
     */
    QString getInstitutionName() const;

    /** retorna el ReferringPhysiciansName
      * @return ReferringPhysiciansName
      */
    QString getReferringPhysiciansName() const;

    /** This action especified in the search which series number we want to match
     * @param seriesNumber' Number of the series to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesNumber(QString );

    /** Aquest mètode especifica per quina data s'ha de buscar la serie. El format es YYYYMMDD
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param series data a cercar la sèrie
     * @return estat del mètode
     */
    Status setSeriesDate(QString date);

     /** Especifica l'hora de la serie a buscar, les hores es passen en format HHMM
      *         Si es buit busca per qualsevol hora.
      *         Si se li passa una hora com 1753, buscarà series d'aquella hora
      *         Si se li passa una hora amb guió a davant, com -1753 , buscarà sèries d'aquella hora o fetes abans
      *         Si se li passa una hora amb guió a darrera, com 1753- , buscarà series fetes a partir d'aquella hora
      *         Si se li passa dos hores separades per un guió, com 1223-1753 , buscarà series fetes entre aquelles hores
      * @param Series Hora de la serie
      * @retun estat del mètode
      */
    Status setSeriesTime(QString);

    /** This action especified that in the search we want the seriess description
     * @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
     * @return estat del mètode
     */
    Status setSeriesDescription(QString);

    /** This action especified that in the search we want the series modality
     * @param series modality the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesModality(QString);

    /** This action especified that in the search we want to query the operator's name
     * @param Operator's name. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesOperator(QString);

    /** This action especified that in the search we want to query the body part examinated
     * @param Body Part. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesBodyPartExaminated(QString);

    /** This action especified that in the search we want to query the Protocol Name
     * @param Protocol Name. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesProtocolName(QString);

     /** This action especified that in the search we want the seriess description
     * @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
     * @return estat del mètode
     */
    Status setSeriesUID(QString date);

    /** Estableix la seqüència per cercar per la requested atribute sequence.
     * RequestAtrributeSequence està format pel RequestedProcedureID i el procedureStepID
     * @param RequestedProcedureID Requested Procedure ID pel qual s'ha de cercar
     * @param ScheduledProcedureStepID Scheduled Procedure Step ID pel qual s'ha de cercar
     * @return estat del mètode
     */
    Status setRequestAttributeSequence( QString procedureID, QString procedureStepID );

    /** Estableix la màscara de cerca del Perfomed Procedure Step Start date
     * @param startDate Perfomed Procedure Step Start date pel qual cercar
     * @return estat del mètode
     */
    Status setPPSStartDate( QString startDate );

    /** Estableix la màscara de cerca del Perfomed Procedure Step Start Time
     * @param startTime Perfomed Procedure Step Start Time pel qual cercar
     * @return estat del mètode
     */
    Status setPPStartTime( QString startTime );

    /** Retorna el series Number
     * @return   series Number
     */
    QString getSeriesNumber() const;

    /** Retorna la data de la sèrie
     * @return   data de la sèrie
     */
    QString getSeriesDate() const;

    /** Retorna l'hora de la sèrie
     * @return   hora de la sèrie
     */
    QString getSeriesTime() const;

    /** Retorna la descripcio de la sèrie
     * @return descripcio de la serie
     */
    QString getSeriesDescription() const;

    /** Retorna la modalitat de la sèrie
     * @return modalitat de la sèrie
     */
    QString getSeriesModality() const;

    /** Retorna l'operador que captat la serie
     * @return operdor
     */
    QString getSeriesOperator() const;

    /** Retorna la part del cos examinada en la serie
     * @return part del cos examinada
     */
    QString getSeriesBodyPartExaminated() const;

    /** Retorna el nom del protocol utiltizat la serie
     * @return nom del protocol utilitzat a la seire
     */
    QString getSeriesProtocolName() const;

    /** Retorna l'uid de la serie
     * @return SeriesUID
     */
    QString getSeriesUID() const;

    /** retorna el Requested procedures ID
     * @return requesta procedure ID
     */
    QString getRequestedProcedureID() const;

    /** retorna el scheduled procedure step ID
     * @return scheduled procedure step ID
     */
    QString getScheduledProcedureStepID() const;

    /** retorna el Perfomed Procedure Step Start date
     * @return Perfomed Procedure Step Start date
     */
    QString getPPSStartDate() const;

    /** retorna el Perfomed Procedure Step Start Time
     * @return Perfomed Procedure Step Start Time
     */
    QString getPPSStartTime() const;

    /** set the StudyId of the images
     * @param   Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return The state of the action
     */
    Status setImageNumber( QString );

    /** Estableix el instance UID de l'image
     * @param SOPIntanceUID Instance UID de l'imatge
     * @return estat del mètode
     */
    Status setSOPInstanceUID( QString SOPInstanceUID);

    /** Retorna el número d'imatge
      * @return número d'imatge
      */
    QString getImageNumber() const;

    /** retorna el SOPInstance UID de l'imatge
     * @return SOPInstance UID de l'imatge
     */
    QString getSOPInstanceUID() const;

    /** retorna a quin nivell es fa la màscara
     */
    QString getRetrieveLevel() const;

    DcmDataset* getDicomMask();

    bool operator ==(const DicomMask &mask);

private:

   //enum objectMask { StudyMask , SeriesMask , ImageMask };
    enum ObjectMask { StudyMask , SeriesMask , ImageMask };
    DcmDataset *m_mask;
    ObjectMask m_objectMask;

    /** En funció de per quins elements establim que hem de crear la màscara el retrieve estableix un nivell o altre de cercar
    * @param retrieveLevel retrieve level amb que ataquem al PACS
    */
    void retrieveLevel( ObjectMask retrieveLevel);
};

} //end namespace
#endif
