#ifndef DICOMMASK
#define DICOMMASK

#include <QString>
#include <QDate>
#include <QTime>

/**
    Aquesta classe construeix una màscara per la cerca d'estudis/series/imatges al PACS/BD/DICOMDIR. 
    S'ha de consulta a cada classe on s'utilitza si tots els camps d'aquesta classe DICOMMask estan disponibles en la cercar.

    Quan es fan cerques el PACS, aquest només retorna els camps que se li han demanat, si utilitzem la DICOMMask per fer una cerca al PACS i volem indicar que ens retorni
    uns determinats camps hem de set amb string.empty amb els camps que volem que ens retorni per l'estudi/serie/imatge i pels camps que són Data/Hora una Data/Hora nul·la QDate().
  */
namespace udg {

class DicomMask {
public:
    DicomMask();

    /// Indiquem el Patient name pel qual filtrar. Si els dos paràmetres són Nulls es busquen tots els pacients, si per exemple minimumDate és null
    ///i a maximumDate se li passa valor buscarà els pacients nascuts abans o el mateix dia de la data indicada a maximumDate
    void setPatientName(const QString &patientName);

    /// Aquest mètode especifica la data de naixement del pacient amb la que s'han de cercar els estudis. El format és DDMMYYYY
    void setPatientBirth(const QDate &minimumDate, const QDate &maximumDate);

    /// This action especified that in the search we want the Patient's sex
    /// @param Patient's sex of the patient to search.
    /// @return state of the method
    void setPatientSex(const QString &patientSex);

    /// Indiquem el patient Id pel qual filtrar
    void setPatientId(const QString &patientId);

    /// This action especified that in the search we want the Patient's Age
    /// @param  Patient's age of the patient to search.
    /// @return state of the method
    void setPatientAge(const QString &patientAge);

    /// This action especified that in the search we want the Study's Id
    /// @param  Study's Id of the study to search.
    /// @return state of the method
    void setStudyId(const QString &studyId);

    ///Indiquem entre quines dates volem filtrar els estudis. Si els dos paràmetres són Nulls es busquen tots els estudis, si per exemple minimumDate és null
    ///i a maximumDate se li passa valor buscarà els estudis fets abans o el mateix dia de la data indicada a maximumDate
    void setStudyDate(const QDate &minimumDate, const QDate &maximumDate);

    /// This action especified that in the search we want the Study's description
    ///  @param Study's description of the study to search.
    ///  @return state of the method
    void setStudyDescription(const QString &studyDescription);

    /// This action especified that in the search we want the Study's modality
    /// @param Study's modality the study to search.
    /// @return state of the method
    void setStudyModality(const QString &studyModality);

    ///Indiquem entre quines hores volem filtrar els estudis. Si els dos paràmetres són Nulls es busquen tots els estudis, si per exemple minimumTime és null
    ///i a maximumTime se li passa valor buscarà els estudis fets abans o de l'hora indicada a maximumTime
    void setStudyTime(const QTime &studyTimeMinimum, const QTime &studyTimeMaximum);

    /// This action especified that in the search we want the Study's instance UID
    /// @param Study's instance UID the study to search.
    /// @return state of the method
    void setStudyInstanceUID(const QString &studyInstanceUID);

    /// This action especified that in the search we want the Accession Number
    /// @param Accession Number of the study to search.
    /// @return state of the method
    void setAccessionNumber(const QString &accessionNumber);

    /// Establei el ReferringPhysiciansName
    /// @param physiciansName
    /// @return estat del mètode
    void setReferringPhysiciansName(const QString &physiciansName);

    /// Retorna de la màscara l'estudi UID
    /// @param mask [in] màscara de la cerca
    /// @return   Estudi UID que cerquem
    QString getStudyInstanceUID() const;

    /// Retorna el Id del pacient a filtrar
    QString getPatientId() const;

    /// Retorna el nom del pacient a filtrar
    QString getPatientName() const;

    /// Retorna en quina data de neixement hem de buscar els pacients
    QDate getPatientBirthMinimum() const;
    QDate getPatientBirthMaximum() const;
    QString getPatientBirthRangeAsDICOMFormat() const;

    /// Retorna l'edat de pacient
    /// @return edat del pacient
    QString getPatientAge() const;

    /// Retorna el sexe del pacient
    /// @return sexe del pacient
    QString getPatientSex() const;

    /// Retorna Id de l'estudi
    /// @return   ID de l'estudi
    QString getStudyId() const;

    /// Retorna rang de dates en que s'han de buscar els estudis. Es retorna en format String per quan es fan cerques al PACS si és null significa que no hem 
    /// demanar al PACS que retorni aquell camp, si és buit significa que l'ha de retorna 
    QDate getStudyDateMinimum() const;
    QDate getStudyDateMaximum() const;

    ///Retorna en format strign el Rang de dates en que hem de cercar l'estudi. Si retorna null indica que no ens han demanat cercar per data. Retorna el rang de data separat per "-"
    QString getStudyDateRangeAsDICOMFormat() const;

    /// Retorna la descripció de l'estudi
    /// @return   descripció de l'estudi
    QString getStudyDescription() const;

    /// Retorna de la modalitat de l'estudi
    /// @return   Modalitat de l'estudi
    QString getStudyModality() const;

    /// Retorna l'hora de l'estudi
    /// @return   hora de l'estudi
    QTime getStudyTimeMinimum() const;
    QTime getStudyTimeMaximum() const;
    QString getStudyTimeRangeAsDICOMFormat() const;

    /// Retorna el accession number de l'estudi
    /// @return   accession number de l'estudi
    QString getAccessionNumber() const;

    /// Retorna el ReferringPhysiciansName
    /// @return ReferringPhysiciansName
    QString getReferringPhysiciansName() const;

    /// This action especified in the search which series number we want to match
    /// @param seriesNumber' Number of the series to search. If this parameter is null it's supose that any mask is applied at this field
    /// @return estat del mètode
    void setSeriesNumber(const QString &seriesNumber);

    ///Indiquem entre quines dates volem filtrar les sèries. Si els dos paràmetres són Nulls es busquen tots els estudis, si per exemple minimumDate és null
    ///i a maximumDate se li passa valor buscarà les sèries fets abans o el mateix dia de la data indicada a maximumDate
    void setSeriesDate(const QDate &minimumDate, const QDate &maximumDate);

    ///Indiquem entre quines hores volem filtrar les sèries. Si els dos paràmetres són Nulls es busquen totess les serèries, si per exemple minimumTime és null
    ///i a maximumTime se li passa valor buscarà les sèries fetes abans o de l'hora indicada a maximumTime
    void setSeriesTime(const QTime &minimumTime, const QTime &maximumTime);

    /// This action especified that in the search we want the seriess description
    /// @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
    /// @return estat del mètode
    void setSeriesDescription(const QString &seriesDescription);

    /// This action especified that in the search we want the series modality
    /// @param series modality the study to search. If this parameter is null it's supose that any mask is applied at this field
    /// @return estat del mètode
    void setSeriesModality(const QString &seriesModality);

    /// This action especified that in the search we want to query the Protocol Name
    /// @param Protocol Name. If this parameter is null it's supose that any mask is applied at this field
    /// @return estat del mètode
    void setSeriesProtocolName(const QString &protocolName);

    /// This action especified that in the search we want the seriess description
    /// @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field.
    /// @return estat del mètode
    void setSeriesInstanceUID(const QString &seriesInstanceUID);

    /// Estableix la seqüència per cercar per la requested atribute sequence.
    /// RequestAtrributeSequence està format pel RequestedProcedureID i el procedureStepID
    /// @param RequestedProcedureID Requested Procedure ID pel qual s'ha de cercar
    /// @param ScheduledProcedureStepID Scheduled Procedure Step ID pel qual s'ha de cercar
    /// @return estat del mètode
    void setRequestAttributeSequence(const QString &procedureID, const QString &procedureStepID);

    /// Estableix la màscara de cerca del Perfomed Procedure Step Start date
    /// @param startDate Perfomed Procedure Step Start date pel qual cercar
    /// @return estat del mètode
    void setPPSStartDate(const QDate &minimumDate, const QDate &maximumDate);

    /// Estableix la màscara de cerca del Perfomed Procedure Step Start Time
    /// @param startTime Perfomed Procedure Step Start Time pel qual cercar
    /// @return estat del mètode
    void setPPStartTime(const QTime &minimumTime, const QTime &maximumTime);

    /// Retorna el series Number
    /// @return   series Number
    QString getSeriesNumber() const;

    /// Retorna la data de la sèrie
    QDate getSeriesDateMinimum() const;
    QDate getSeriesDateMaximum() const;
    QString getSeriesDateRangeAsDICOMFormat() const;

    /// Retorna l'hora de la sèrie
    /// @return   hora de la sèrie
    QTime getSeriesTimeMinimum() const;
    QTime getSeriesTimeMaximum() const;
    QString getSeriesTimeRangeAsDICOMFormat() const;

    /// Retorna la descripcio de la sèrie
    /// @return descripcio de la serie
    QString getSeriesDescription() const;

    /// Retorna la modalitat de la sèrie
    /// @return modalitat de la sèrie
    QString getSeriesModality() const;

    /// Retorna el nom del protocol utiltizat la serie
    /// @return nom del protocol utilitzat a la seire
    QString getSeriesProtocolName() const;

    /// Retorna l'uid de la serie
    /// @return SeriesUID
    QString getSeriesInstanceUID() const;

    /// Retorna el Requested procedures ID
    /// @return requesta procedure ID
    QString getRequestedProcedureID() const;

    /// Retorna el scheduled procedure step ID
    /// @return scheduled procedure step ID
    QString getScheduledProcedureStepID() const;

    /// Retorna el Perfomed Procedure Step Start date
    /// @return Perfomed Procedure Step Start date
    QDate getPPSStartDateMinimum() const;
    QDate getPPSStartDateMaximum() const;
    QString getPPSStartDateAsRangeDICOMFormat() const;

    /// Retorna el Perfomed Procedure Step Start Time
    /// @return Perfomed Procedure Step Start Time
    QTime getPPSStartTimeMinimum() const;
    QTime getPPSStartTimeMaximum() const;
    QString getPPSStartTimeAsRangeDICOMFormat() const;

    /// Set the StudyId of the images
    /// @param   Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
    /// @return The state of the action
    void setImageNumber(const QString &imageNumber);

    /// Estableix el instance UID de l'image
    /// @param SOPIntanceUID Instance UID de l'imatge
    /// @return estat del mètode
    void setSOPInstanceUID(const QString &SOPInstanceUID);

    /// Retorna el número d'imatge
    /// @return número d'imatge
    QString getImageNumber() const;

    /// Retorna el SOPInstance UID de l'imatge
    /// @return SOPInstance UID de l'imatge
    QString getSOPInstanceUID() const;

    bool operator ==(const DicomMask &mask);

    /// Defineix l'operador suma dos màscares, en el cas que el this i la màscara passada per paràmetre tinguin un valor de filtratge per una determinada
    /// propietat, no es cercarà pels dos valors,sinó només pel de la màscara this
    DicomMask operator +(const DicomMask &mask);

    /// Mètode de conveniència que ens retorna un string indicant els camps
    /// que s'han omplert per fer la màscara de forma que es pugui loggejar per estadístiques.
    /// Els camps amb informació personal s'indicaran com a omplerts amb un asterisc, però no
    /// es posarà la informació. Per exemple, si es fa servir el camp "nom de pacient" s'indicarà
    /// el valor "#*#" però no la informació que ha posat l'usuari. Tampoc es tindran en compte
    /// les wildcards '*' que puguin haver, per tant, si un camp està omplert amb "*", considerarem
    /// que no s'ha fet servir
    QString getFilledMaskFields() const;

    /// Ens diu si la màscara està buida o no
    bool isEmpty();

private:

    ///Retorna el rang d'un data o hora en format DICOM
    QString getDateOrTimeRangeAsDICOMFormat(const QString &minimumDateTime, const QString &maximumDateTime) const;

private:
    static const QString DateFormatAsString;
    static const QString TimeFormatAsString;

    //Els camps que són Data/Hora els guardem con a QString perquè així alhora de cercar al PACS sabem si ens han demanat que es retorni aquell camp, 
    //ja que QString té 3 estats null (camp no sol·licitat), empty (s'ha de tornar aquell camp per tots els estudis que compleixin la cerca), 
    //valor s'han de tornar els estudis que tinguin aquell valor.

    QString m_patientId;
    QString m_patientName;
    QString m_patientBirthMinimum;
    QString m_patientBirthMaximum;
    QString m_patientSex;
    QString m_patientAge;
    QString m_studyId;
    QString m_studyDateMinimum;
    QString m_studyDateMaximum;
    QString m_studyTimeMinimum;
    QString m_studyTimeMaximum;
    QString m_studyDescription;
    QString m_studyModality;
    QString m_studyInstanceUID;
    QString m_accessionNumber;
    QString m_referringPhysiciansName;
    QString m_seriesNumber;
    QString m_seriesDateMinimum;
    QString m_seriesDateMaximum;
    QString m_seriesTimeMinimum;
    QString m_seriesTimeMaximum;
    QString m_seriesModality;
    QString m_seriesDescription;
    QString m_seriesProtocolName;
    QString m_seriesInstanceUID;
    QString m_requestedProcedureID;
    QString m_scheduledProcedureStepID;
    QString m_PPSStartDateMinimum;
    QString m_PPSStartDateMaximum;
    QString m_PPSStartTimeMinimum;
    QString m_PPSStartTimeMaximum;
    QString m_SOPInstanceUID;
    QString m_imageNumber;
};

} // End namespace
#endif
