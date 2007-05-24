#ifndef SERIES
#define SERIES

#define HAVE_CONFIG_H 1

#include <list>
#include <string>
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags

/** This class sets and gets series fields
 */

namespace udg{

class Series
{

 public :

    /// Initialize the fields of the series
    Series();

    /** Construeix un objecte Series a partir un dmcdataset de dicom
     * @param  dcmdataset a partir del qual construir l'objecte
     */
    Series(DcmDataset *);

    /** Operador que ens ajudar a ordenar les series pel camp seriesNumber
     * @param Serie amb la que hem de comparar
     */
    bool operator<( Series );

    /** This action set the series Number
     * @param Series Number
     */
    void setSeriesNumber( std::string );

    /** This action set the series description
     * @param Series description
     */
    void setSeriesDescription( std::string );

    /** This action set the series modality
     * @param series modality
     */
    void setSeriesModality( std::string );

    /** This action set the series date.
     * @param Series date
     */
    void setSeriesDate( std::string );

    /** This action set the series time.
     * @param Series time
     */
    void setSeriesTime( std::string );

    /** This action set the study UID
     * @param  study UID
     */
    void setStudyUID( std::string );

    /** This action set the series UID
     * @param   Series UID
     */
    void setSeriesUID( std::string );

    /** This action set the operator's name
     * @param  Series name
     */
    void setOperatorName( std::string );

    /** This action set the series Protocol name
     * @param   Series Protocol
     */
    void setProtocolName( std::string );

    /** This action set the body part examinated
     * @param Body part examinated in the series
     */
    void setBodyPartExaminated( std::string );

    /** Estableix el path relatiu de la sèrie
     * @param  Series path
     */
    void setSeriesPath( std::string );

    /** Estableix a quin PACS pertany la sèrie
     * @param AEtitle AeTitle del pacs la que pertany la sèrie
     */
    void setPacsAETitle( std::string AETitle );

    /** Estableix el número d'imatges que conté la serie, respecte el directori on es troben els estudis
     * @param Número d'imatges
     */
    void setImageNumber( int );

    /** Estableix el requeste procedure ID
     * @param requestedProcedureID
     */
    void setRequestedProcedureID( std::string requestedProcedureID );

    /** Estableix el scheduled procedure step ID
     * @param scheduledProcedureStepID
     */
    void setScheduledProcedureStepID( std::string scheduledProcedureStepID );

    /** Estableix el Performed Procedure Step Start Date
     * @param startDate
     */
    void setPPSStartDate( std:: string startDate );

    /** Estableix el Performed Procedure Step Start Time
     * @param startTime
     */
    void setPPSStartTime( std:: string startTime );

    /** This function get the Series Number
     * @return  Series Number
     */
    std::string getSeriesNumber();

    /** This function get the Series Description
     * @return  Series Description
     */
    std::string getSeriesDescription();

    /** This function get the Series Modality
     * @return  Series Modality
     */
    std::string getSeriesModality();

    /** This function get the Series Time
     * @return  Series Time
     */
    std::string getSeriesTime();

    /** This function get the Series Date
     * @return  Series Date
     */
    std::string getSeriesDate();

    /** This function get the Study UID
     * @return  Study UID
     */
    std::string getStudyUID();

    /** This function get the Series UID
     * @return  Series's UID
     */
    std::string getSeriesUID();

    /** This function get the operator name
     * @return  operator name
     */
    std::string getOperatorName();

    /** This function get the Protocol Name
     * @return  Protocol Name
     */
    std::string getProtocolName();

    /** This function get the Body Part Examinated
     * @return  SBody Part Examinated
     */
    std::string getBodyPartExaminated();

    /** Retorna el path relatiu de la sèrie, respecte el directori on es troben els estudis
     * @return  path
     */
    std::string getSeriesPath();

    /** retorna el número d'imatges que conté la serie
     * @return Número d'imatges
     */
    int         getImageNumber();

    /** retorna el AETitle del PACS al que pertany la serie
     * @return AETitle del Pacs
     */
    std::string getPacsAETitle();

    /** retorna el Requested ProcedureID
     * @return Requested ProcedureID
     */
    std::string getRequestedProcedureID();

    /** retorna el Scheduled Procedure Step ID
     * @return Scheduled Procedure Step ID
     */
    std::string getScheduledProcedureStepID();

    /** retorna el Perfomed Procedure Step Start date
     * @return Perfomed Procedure Step Start date
     */
    std::string getPPSStartDate();

    /** retorna el Perfomed Procedure Step Start Time
     * @return Perfomed Procedure Step Start Time
     */
    std::string getPPStartTime();

 private :

    std::string m_seriesUID;
    std::string m_seriesDescription;
    std::string m_seriesDate;
    std::string m_seriesTime;
    std::string m_seriesModality;
    std::string m_seriesNumber;
    std::string m_studyUID;
    std::string m_protocolName;
    std::string m_operatorName;
    std::string m_bodyPartExaminated;
    std::string m_seriesPath; //path relatiu, dins el directori on es guarden els estudis, no ha de ser path abosut, per exemple /STU00000/SER00001
    int         m_imageNumber;
    std::string m_seriesAETitlePACS;
    std::string m_requestedProcedureID;
    std::string m_scheduledProcedureStepID;
    std::string m_ppsStartDate;
    std::string m_ppsStartTime;
};
};
#endif

