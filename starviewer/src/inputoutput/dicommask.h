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

#include <QString>

#define HAVE_CONFIG_H 1

#include "const.h"
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members

/** Aquesta classe construeix una màscara per la cerca d'estudis.
   *Cada camp que volem que la cerca ens retorni li hem de fer el set, sinó retornarà valor null per aquell camp. Per tots els camps podem passar, cadena buida que significa que buscarà tots els valors d'aquell camp, o passar-li un valor en concret, on només retornarà els compleixin aquell criteri.
 */
namespace udg{

class Status;
class string;

class DicomMask
{

public:

    DicomMask();

    /** This action especified that in the search we want the Patient's Name
     * @param Name of the patient to search.
     * @return state of the method
     */
    Status setPatientName(std::string);

    /** Aquest mètode especifica la data de naixement del pacient amb la que s'han de cercar els estudis. El format és DDMMYYYY
     * Si el paràmetre no té valor, busca per totes les dates
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param  Study's Data de naixement del pacient
     * @return state of the method
     */
    Status setPatientBirth(std::string date);

    /** This action especified that in the search we want the Patient's sex
     * @param Patient's sex of the patient to search.
     * @return state of the method
     */
    Status setPatientSex(std::string);

    /** This action especified that in the search we want the Patient's ID
     * @param   patientID Patient's ID of the patient to search.
     * @return state of the method
     */
    Status setPatientId(std::string);

    /** This action especified that in the search we want the Patient's Age
     * @param  Patient's age of the patient to search.
     * @return state of the method
     */
    Status setPatientAge(std::string);

    /** This action especified that in the search we want the Study's Id
     * @param  Study's Id of the study to search.
     * @return state of the method
     */
    Status setStudyId(std::string);

    /** Aquest mètode especifica la data amb la que s'han de cercar els estudis. El format és DDMMYYYY
     * Si el paràmetre no té valor, busca per totes les dates
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param  Study's date of the study to search.
     * @return state of the method
     */
    Status setStudyDate(std::string date);

    /** This action especified that in the search we want the Study's description
     *  @param Study's description of the study to search.
     *  @return state of the method
     */
    Status setStudyDescription(std::string);

    /** This action especified that in the search we want the Study's modality
     * @param Study's modality the study to search.
     * @return state of the method
     */
    Status setStudyModality(std::string);

    /** Especifica l'hora de l'estudi a buscar, les hores es passen en format HHMM
     *         Si es buit busca per qualsevol hora.
     *         Si se li passa una hora com 1753, buscarà series d'aquella hora
     *         Si se li passa una hora amb guió a davant, com -1753 , buscarà sèries d'aquella hora o fetes abans
     *         Si se li passa una hora amb guió a darrera, com 1753- , buscarà series fetes a partir d'aquella hora
     *         Si se li passa dos hores separades per un guió, com 1223-1753 , buscarà series fetes entre aquelles hores
     * @param  Hora de l'estudi
     * @retun estat del mètode
     */
    Status setStudyTime(std::string);

    /** This action especified that in the search we want the Study's instance UID
     * @param Study's instance UID the study to search.
     * @return state of the method
     */
    Status setStudyUID(std::string);

    /** This action especified that in the search we want the Institution name
     * @param Institution name of the study to search.
     * @return state of the method
     */
    Status setInstitutionName(std::string);

    /** This action especified that in the search we want the Accession Number
     * @param Accession Number of the study to search.
     * @return state of the method
     */
    Status setAccessionNumber(std::string);

    /** Establei el ReferringPhysiciansName
     * @param physiciansName
     * @return estat del mètode
     */
    Status setReferringPhysiciansName( std::string physiciansName );

    /** Retorna de la màscara l'estudi UID
     * @param mask [in] màscara de la cerca
     * @return   Estudi UID que cerquem
     */
    std::string getStudyUID();

    /** Retorna el Id del pacient a buscar
     * @return   ID del pacient
     */
    std::string getPatientId();

    /** Retorna el nom del pacient
     * @return Nom del pacient
     */
    std::string getPatientName();

    /** Retorna la data naixement
     * @return Data de naixament del pacient
     */
    std::string getPatientBirth();

    /** Retorna l'edat de pacient
     * @return edat del pacient
     */
    std::string getPatientAge();

    /** Retorna el sexe del pacient
     * @return sexe del pacient
     */
    std::string getPatientSex();

    /** Retorna Id de l'estudi
     * @return   ID de l'estudi
     */
    std::string getStudyId();

    /** Retorna la data de l'estudi
     * @return   Data de l'estudi
     */
    std::string getStudyDate();

    /** Retorna la descripció de l'estudi
     * @return   descripció de l'estudi
     */
    std::string getStudyDescription();

    /** Retorna de la modalitat de l'estudi
     * @return   Modalitat de l'estudi
     */
    std::string getStudyModality();

    /** Retorna l'hora de l'estudi
     * @return   hora de l'estudi
     */
    std::string getStudyTime();

    /** Retorna el accession number de l'estudi
     * @return   accession number de l'estudi
     */
    std::string getAccessionNumber();

    /** Retorna el nom de l'institució on s'ha realitzat l'estudi
     * @return   Nom de l'institucio
     */
    std::string getInstitutionName();

    /** retorna el ReferringPhysiciansName
      * @return ReferringPhysiciansName
      */
    std:: string getReferringPhysiciansName();

    /** This action especified in the search which series number we want to match
     * @param seriesNumber' Number of the series to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesNumber(std:: string );

    /** Aquest mètode especifica per quina data s'ha de buscar la serie. El format es YYYYMMDD
     * Si passem una data sola, per exemple 20040505 només buscara sèries d'aquell dia
     * Si passem una data amb un guio a davant, per exemple -20040505 , buscarà sèries fetes aquell dia o abans
     * Si passem una data amb un guio a darrera, per exemple 20040505- , buscarà series fetes aquell dia, o dies posteriors
     * Si passem dos dates separades per un guio, per exemple 20030505-20040505 , buscarà sèries fetes entre aquelles dos dates
     * @param series data a cercar la sèrie
     * @return estat del mètode
     */
    Status setSeriesDate(std::string date);

     /** Especifica l'hora de la serie a buscar, les hores es passen en format HHMM
      *         Si es buit busca per qualsevol hora.
      *         Si se li passa una hora com 1753, buscarà series d'aquella hora
      *         Si se li passa una hora amb guió a davant, com -1753 , buscarà sèries d'aquella hora o fetes abans
      *         Si se li passa una hora amb guió a darrera, com 1753- , buscarà series fetes a partir d'aquella hora
      *         Si se li passa dos hores separades per un guió, com 1223-1753 , buscarà series fetes entre aquelles hores
      * @param Series Hora de la serie
      * @retun estat del mètode
      */
    Status setSeriesTime(std::string);

    /** This action especified that in the search we want the seriess description
     * @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
     * @return estat del mètode
     */
    Status setSeriesDescription(std::string);

    /** This action especified that in the search we want the series modality
     * @param series modality the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesModality(std::string);

    /** This action especified that in the search we want to query the operator's name
     * @param Operator's name. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesOperator(std::string);

    /** This action especified that in the search we want to query the body part examinated
     * @param Body Part. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesBodyPartExaminated(std::string);

    /** This action especified that in the search we want to query the Protocol Name
     * @param Protocol Name. If this parameter is null it's supose that any mask is applied at this field
     * @return estat del mètode
     */
    Status setSeriesProtocolName(std::string);

     /** This action especified that in the search we want the seriess description
     * @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
     * @return estat del mètode
     */
    Status setSeriesUID(std::string date);

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
    Status setPPSStartDate( std::string startDate );

    /** Estableix la màscara de cerca del Perfomed Procedure Step Start Time
     * @param startTime Perfomed Procedure Step Start Time pel qual cercar
     * @return estat del mètode
     */
    Status setPPStartTime( std:: string startTime );

    /** Retorna el series Number
     * @return   series Number
     */
    std::string getSeriesNumber();

    /** Retorna la data de la sèrie
     * @return   data de la sèrie
     */
    std::string getSeriesDate();

    /** Retorna l'hora de la sèrie
     * @return   hora de la sèrie
     */
    std::string getSeriesTime();

    /** Retorna la descripcio de la sèrie
     * @return descripcio de la serie
     */
    std::string getSeriesDescription();

    /** Retorna la modalitat de la sèrie
     * @return modalitat de la sèrie
     */
    std::string getSeriesModality();

    /** Retorna l'operador que captat la serie
     * @return operdor
     */
    std::string getSeriesOperator();

    /** Retorna la part del cos examinada en la serie
     * @return part del cos examinada
     */
    std::string getSeriesBodyPartExaminated();

    /** Retorna el nom del protocol utiltizat la serie
     * @return nom del protocol utilitzat a la seire
     */
    std::string getSeriesProtocolName();

    /** Retorna l'uid de la serie
     * @return SeriesUID
     */
    std::string getSeriesUID();

    /** retorna el Requested procedures ID
     * @return requesta procedure ID
     */
    std::string getRequestedProcedureID();

    /** retorna el scheduled procedure step ID
     * @return scheduled procedure step ID
     */
    std::string getScheduledProcedureStepID();

    /** retorna el Perfomed Procedure Step Start date
     * @return Perfomed Procedure Step Start date
     */
    std::string getPPSStartDate();

    /** retorna el Perfomed Procedure Step Start Time
     * @return Perfomed Procedure Step Start Time
     */
    std::string getPPSStartTime();

    /** set the StudyId of the images
     * @param   Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
     * @return The state of the action
     */
    Status setImageNumber( std::string );

    /** Estableix el instance UID de l'image
     * @param SOPIntanceUID Instance UID de l'imatge
     * @return estat del mètode
     */
    Status setSOPInstanceUID( std::string SOPInstanceUID);

    /** Retorna el número d'imatge
      * @return número d'imatge
      */
    std::string getImageNumber();

    /** retorna el SOPInstance UID de l'imatge
     * @return SOPInstance UID de l'imatge
     */
    std::string getSOPInstanceUID();

    /** retorna a quin nivell es fa la màscara
     */
    std::string getRetrieveLevel();

    DcmDataset* getDicomMask();

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
