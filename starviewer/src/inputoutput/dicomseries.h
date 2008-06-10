#ifndef DICOMSERIES
#define DICOMSERIES

#include <QString>

class DcmDataset;

namespace udg{

/**
* This class sets and gets series fields
*/
class DICOMSeries
{

 public :

    /// Initialize the fields of the series
    DICOMSeries();

    /** Construeix un objecte Series a partir un dmcdataset de dicom
     * @param  dcmdataset a partir del qual construir l'objecte
     */
    DICOMSeries(DcmDataset *);

    /** Operador que ens ajudar a ordenar les series pel camp seriesNumber
     * @param Serie amb la que hem de comparar
     */
    bool operator<( DICOMSeries series );

    /** This action set the series Number
     * @param Series Number
     */
    void setSeriesNumber( QString );

    /** This action set the series description
     * @param Series description
     */
    void setSeriesDescription( QString );

    /** This action set the series modality
     * @param series modality
     */
    void setSeriesModality( QString );

    /** This action set the series date.
     * @param Series date
     */
    void setSeriesDate( QString );

    /** This action set the series time.
     * @param Series time
     */
    void setSeriesTime( QString );

    /** This action set the study UID
     * @param  study UID
     */
    void setStudyUID( QString );

    /** This action set the series UID
     * @param   Series UID
     */
    void setSeriesUID( QString );

    /** This action set the operator's name
     * @param  Series name
     */
    void setOperatorName( QString );

    /** This action set the series Protocol name
     * @param   Series Protocol
     */
    void setProtocolName( QString );

    /** This action set the body part examinated
     * @param Body part examinated in the series
     */
    void setBodyPartExaminated( QString );

    /** Estableix el path relatiu de la sèrie
     * @param  Series path
     */
    void setSeriesPath( QString );

    /** Estableix a quin PACS pertany la sèrie
     * @param AEtitle AeTitle del pacs la que pertany la sèrie
     */
    void setPacsAETitle( QString AETitle );

    /** Estableix el número d'imatges que conté la serie, respecte el directori on es troben els estudis
     * @param Número d'imatges
     */
    void setImageNumber( int );

    /** Estableix el requeste procedure ID
     * @param requestedProcedureID
     */
    void setRequestedProcedureID( QString requestedProcedureID );

    /** Estableix el scheduled procedure step ID
     * @param scheduledProcedureStepID
     */
    void setScheduledProcedureStepID( QString scheduledProcedureStepID );

    /** Estableix el Performed Procedure Step Start Date
     * @param startDate
     */
    void setPPSStartDate( QString startDate );

    /** Estableix el Performed Procedure Step Start Time
     * @param startTime
     */
    void setPPSStartTime( QString startTime );

    /** This function get the Series Number
     * @return  Series Number
     */
    QString getSeriesNumber();

    /** This function get the Series Description
     * @return  Series Description
     */
    QString getSeriesDescription();

    /** This function get the Series Modality
     * @return  Series Modality
     */
    QString getSeriesModality();

    /** This function get the Series Time
     * @return  Series Time
     */
    QString getSeriesTime();

    /** This function get the Series Date
     * @return  Series Date
     */
    QString getSeriesDate();

    /** This function get the Study UID
     * @return  Study UID
     */
    QString getStudyUID();

    /** This function get the Series UID
     * @return  Series's UID
     */
    QString getSeriesUID();

    /** This function get the operator name
     * @return  operator name
     */
    QString getOperatorName();

    /** This function get the Protocol Name
     * @return  Protocol Name
     */
    QString getProtocolName();

    /** This function get the Body Part Examinated
     * @return  SBody Part Examinated
     */
    QString getBodyPartExaminated();

    /** Retorna el path relatiu de la sèrie, respecte el directori on es troben els estudis
     * @return  path
     */
    QString getSeriesPath();

    /** retorna el número d'imatges que conté la serie
     * @return Número d'imatges
     */
    int         getImageNumber();

    /** retorna el AETitle del PACS al que pertany la serie
     * @return AETitle del Pacs
     */
    QString getPacsAETitle();

    /** retorna el Requested ProcedureID
     * @return Requested ProcedureID
     */
    QString getRequestedProcedureID();

    /** retorna el Scheduled Procedure Step ID
     * @return Scheduled Procedure Step ID
     */
    QString getScheduledProcedureStepID();

    /** retorna el Perfomed Procedure Step Start date
     * @return Perfomed Procedure Step Start date
     */
    QString getPPSStartDate();

    /** retorna el Perfomed Procedure Step Start Time
     * @return Perfomed Procedure Step Start Time
     */
    QString getPPStartTime();

 private :

    QString m_seriesUID;
    QString m_seriesDescription;
    QString m_seriesDate;
    QString m_seriesTime;
    QString m_seriesModality;
    QString m_seriesNumber;
    QString m_studyUID;
    QString m_protocolName;
    QString m_operatorName;
    QString m_bodyPartExaminated;
    QString m_seriesPath; //path relatiu, dins el directori on es guarden els estudis, no ha de ser path abosut, per exemple /STU00000/SER00001
    int         m_imageNumber;
    QString m_seriesAETitlePACS;
    QString m_requestedProcedureID;
    QString m_scheduledProcedureStepID;
    QString m_ppsStartDate;
    QString m_ppsStartTime;
};
};
#endif

