#include "dicommask.h"

#include <dcsequen.h>
#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <ofcond.h> //provide the OFcondition structure and his members
#include <QString>

#include "errordcmtk.h"
#include "status.h"

namespace udg{

DicomMask::DicomMask()
{
    char val[15];
    m_mask = new DcmDataset;

    m_objectMask = StudyMask; // per defecte nivell de la màscara per estudi

    DcmElement *elem = newDicomElement( DCM_QueryRetrieveLevel );

    strcpy( val , "STUDY");
    elem->putString( val );
    m_mask->insert( elem , OFTrue );

    //\TODO Revisar: Establim quin és el nostre implementationVersionName ( nom de l'aplicació que rebrà el pacs que es vol connectar contra ell)
//     DcmElement *elemImplementationVersionName = newDicomElement( DCM_ImplementationVersionName );
//     elemImplementationVersionName->putString( implementationVersionName) );
//     m_mask->insert( elemImplementationVersionName , OFTrue );
}

void DicomMask::retrieveLevel( ObjectMask object )
{
    char val[15];

    switch ( object )
    {
        case StudyMask :
            //per defecte tenim nivell d'estudi
            break;
        case SeriesMask :
            if ( m_objectMask == StudyMask ) // si fem un set a un tag d'una serie i tenim nivell d'estudi, especifiquem que el nivell de la màscara sera per sèrie
            {
                DcmElement *elem = newDicomElement( DCM_QueryRetrieveLevel );

                strcpy( val , "SERIES") ;
                elem->putString( val );
                m_mask->insert( elem , OFTrue );
            }
            break;
        case ImageMask :
            if ( m_objectMask == StudyMask || m_objectMask == SeriesMask  ) // si fem un set a un tag d'una imatge especifiquem que el nivell de la màscara serà per imatge
            {
                DcmElement *elem = newDicomElement( DCM_QueryRetrieveLevel );

                strcpy( val , "IMAGE") ;
                elem->putString( val );
                m_mask->insert( elem , OFTrue );
            }
            break;
    }

}

/************************* PATIENT  *********************************************************************/

Status DicomMask::setPatientId( QString patientID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_PatientID) ;

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(patientID) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientId );
    }

    //insert the tag PATIENT ID in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientId );
    }

    return state.setStatus( correct );
}

Status DicomMask::setPatientName( QString patientName )
{
    Status state;
    DcmElement *elem = newDicomElement(DCM_PatientsName);

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(patientName) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientName );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientName );
    }
    return state.setStatus( correct );
}

Status DicomMask::setPatientBirth( QString date )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_PatientsBirthDate );

    retrieveLevel( StudyMask );

    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() != 0 ) return state.setStatus( error_MaskLengthDate );

    elem->putString( qPrintable(date) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientBirth );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientBirth );
    }

    return state.setStatus( correct );
}

Status DicomMask::setPatientSex( QString patientSex )
{
    Status state;
    DcmElement *elem = newDicomElement(DCM_PatientsSex );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(patientSex) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientSex );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientSex );
    }

    return state.setStatus( correct );
}

Status DicomMask::setPatientAge( QString patientAge )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_PatientsAge );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(patientAge) );

    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientAge );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskPatientAge );
    }

    return state.setStatus( correct );
}


QString DicomMask::getPatientId() const
{
    const char *value = NULL;
    QString patientID;

    DcmTagKey patientIDTagKey ( DCM_PatientID );
    OFCondition ec;

    ec = m_mask->findAndGetString( patientIDTagKey , value , OFFalse );

    if ( value != NULL ) patientID.insert( 0 , value );

    return patientID;
}

QString DicomMask::getPatientName() const
{
    const char *value = NULL;
    QString patientName;

    DcmTagKey patientIDTagKey ( DCM_PatientsName );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientIDTagKey , value , OFFalse );

    if ( value != NULL ) patientName.insert( 0 , value );

    return patientName;
}

QString DicomMask::getPatientBirth() const
{
    const char *value = NULL;
    QString patientBirth;

    DcmTagKey patientBirthTagKey ( DCM_PatientsBirthDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientBirthTagKey , value , OFFalse );


    if ( value != NULL ) patientBirth.insert( 0 , value );

    return patientBirth;
}

QString DicomMask::getPatientSex() const
{
    const char *value = NULL;
    QString patientSex;

    DcmTagKey patientSexTagKey ( DCM_PatientsSex );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientSexTagKey , value , OFFalse );

    if ( value != NULL ) patientSex.insert( 0 , value );

    return patientSex;
}

QString DicomMask::getPatientAge() const
{
    const char *value = NULL;
    QString patientAge;

    DcmTagKey patientAgeTagKey ( DCM_PatientsAge );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientAgeTagKey , value , OFFalse );

    if ( value != NULL ) patientAge.insert( 0 , value );

    return patientAge;
}

/****************************************** STUDY *****************************************************/

Status DicomMask:: setStudyId( QString studyID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyID );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(studyID) );

    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskStudyId );
    }

    //insert the tag StudyMask DATE in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal ) {
        return state.setStatus( errorMaskStudyId );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setStudyDate( QString date )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyDate );

    retrieveLevel( StudyMask );

    //pot venir la data amb format de 8 caracters, despres amb guio (9 càractes), o cerca entra dates (17 caràcters)
    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() != 0 ) return state.setStatus( error_MaskLengthDate );

    elem->putString( qPrintable(date) );

    if ( elem->error() != EC_Normal )
    {
       return state.setStatus( errorMaskStudyDate);
    }

    //insert the tag StudyMask DATE in the search mask
    if ( m_mask == NULL ) m_mask = new DcmDataset;
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskStudyDate);
    }

    return state.setStatus( correct );
}

Status DicomMask:: setStudyDescription( QString desc )
{
    Status state;
    DcmElement *elem = newDicomElement(DCM_StudyDescription);

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(desc) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskStudyDescription);
    }

    //insert the tag StudyMask DESCRIPTION in the search mask
    m_mask->insert( elem , OFTrue );

    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskStudyDescription);
    }

    return state.setStatus( correct );
}

Status DicomMask:: setStudyModality( QString modality )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_ModalitiesInStudy );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(modality) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyModality );
    }

    //insert the tag STUDY Modality in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyModality );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setStudyTime( QString time )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyTime );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(time) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyTime );
    }

    //insert the tag STUDY TIME in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyTime );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setStudyUID( QString studyUID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyInstanceUID );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(studyUID) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyUID );
    }

    //insert the tag STUDY UID in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyUID );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setInstitutionName( QString institution )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_InstitutionName );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(institution) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInstitutionName );
    }

    //insert the tag Institution name in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal ) {
        return state.setStatus( error_MaskInstitutionName );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setAccessionNumber( QString accession )
{
    char val[100];
    val[0] = '\0';
    QString value;
    Status state;
    DcmElement *elem = newDicomElement( DCM_AccessionNumber );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(accession) );

    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskAccessionNumber );
    }

    //insert the tag Accession Number in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal ) {
        return state.setStatus( error_MaskAccessionNumber );
    }

    return state.setStatus( correct );
}

Status DicomMask::setReferringPhysiciansName( QString physiciansName )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_ReferringPhysiciansName );

    retrieveLevel( StudyMask );

    elem->putString( qPrintable(physiciansName) );

    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal ) {
        return state.setStatus( error_MaskInsertTag );
    }

    return state.setStatus( correct );
}

/***************************************************************   GET **********************************************/

QString DicomMask::getStudyUID() const
{
    const char * value =NULL;
    QString studyUID;

    DcmTagKey studyUIDTagKey ( DCM_StudyInstanceUID );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyUIDTagKey , value , OFFalse );;

    if ( value != NULL ) studyUID.insert( 0 , value );

    return studyUID;
}

QString DicomMask::getStudyId() const
{
    const char *value = NULL;
    QString studyID;

    DcmTagKey studyIDTagKey ( DCM_StudyID );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyIDTagKey , value , OFFalse );

    if ( value != NULL ) studyID.insert( 0 , value );

    return studyID;
}

QString DicomMask::getStudyDate() const
{
    const char * value = NULL;
    QString studyDate;

    DcmTagKey studyDateTagKey ( DCM_StudyDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyDateTagKey , value , OFFalse );

    if ( value != NULL ) studyDate.insert( 0 , value );

    return studyDate;
}

QString DicomMask::getStudyDescription() const
{
    const char *value = NULL;
    QString studyDescription;

    DcmTagKey studyDescriptionTagKey ( DCM_StudyDescription );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyDescriptionTagKey , value , OFFalse );

    if ( value != NULL ) studyDescription.insert (0 , value );

    return studyDescription;
}

QString DicomMask::getStudyTime() const
{
    const char *value = NULL;
    QString studyTime;

    DcmTagKey studyTimeTagKey ( DCM_StudyTime );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyTimeTagKey , value , OFFalse );

    if ( value != NULL ) studyTime.insert( 0 , value );

    return studyTime;
}

QString DicomMask::getStudyModality() const
{
    const char *value = NULL;
    QString studyModality;

    DcmTagKey studyModalityTagKey ( DCM_ModalitiesInStudy );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyModalityTagKey , value , OFFalse );

    if ( value != NULL ) studyModality.insert( 0 , value );

    return studyModality;
}

QString DicomMask::getAccessionNumber() const
{
    const char *value = NULL;
    QString accessionNumber;

    DcmTagKey accessionNumberTagKey ( DCM_AccessionNumber );
    OFCondition ec;
    ec = m_mask->findAndGetString( accessionNumberTagKey , value , OFFalse );

    if ( value != NULL ) accessionNumber.insert( 0 , value );

    return accessionNumber;
}

QString DicomMask::getInstitutionName() const
{
    const char *value = NULL;
    QString institutionName;

    DcmTagKey institutionNameTagKey (DCM_InstitutionName);
    OFCondition ec;
    ec = m_mask->findAndGetString( institutionNameTagKey , value , OFFalse );

    if ( value != NULL ) institutionName.insert( 0 , value );

    return institutionName;
}

QString DicomMask::getReferringPhysiciansName() const
{
    const char * value = NULL;
    QString referringPhysiciansName;

    DcmTagKey referringPhysiciansNameTagKey ( DCM_ReferringPhysiciansName );
    OFCondition ec;
    ec = m_mask->findAndGetString( referringPhysiciansNameTagKey , value , OFFalse );

    if ( value != NULL ) referringPhysiciansName.insert( 0 , value );

    return referringPhysiciansName;
}

/************************************** SERIES *************************************************/

Status DicomMask:: setSeriesNumber( QString seriesNumber )
{
    DcmElement *elem = newDicomElement( DCM_SeriesNumber );
    Status state;

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(seriesNumber) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesNumber );
    }

    //insert the tag series Number in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesNumber );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesDate( QString date )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesDate );

    retrieveLevel( SeriesMask );

    //pot venir la data amb format de 8 caracters, despres amb guio (9 càractes), o cerca entra dates (17 caràcters)
    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() !=  0 )return state.setStatus( error_MaskLengthDate );

    elem->putString( qPrintable(date) );
    if ( elem->error() != EC_Normal )
    {
       return state.setStatus( errorMaskSeriesDate);
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesDate );
    }
    return state.setStatus( correct );
}

Status DicomMask:: setSeriesDescription( QString desc )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesDescription );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(desc) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesDescription );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesDescription );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesModality( QString modality )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_Modality );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(modality) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesModality );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesModality );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesTime( QString time )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesTime );

    retrieveLevel( SeriesMask );

    //la hora ha de ser de longitud 4 HHMM, o 5 HHMM- o -HHMM, o 9 HHMM-HHMM
    if ( time.length() != 4 && time.length() != 5 && time.length() != 9 && time.length() !=0 ) return state.setStatus( error_MaskLengthTime );

    elem->putString( qPrintable(time) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesTime );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesTime );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesUID( QString seriesUID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesInstanceUID );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(seriesUID) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesUID );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesUID );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesOperator( QString name )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_OperatorsName );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(name) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskOperatorName );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskOperatorName );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesBodyPartExaminated( QString part )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_BodyPartExamined );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(part) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskBodyPartExaminated );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskBodyPartExaminated );
    }

    return state.setStatus( correct );
}

Status DicomMask:: setSeriesProtocolName( QString name )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_ProtocolName );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(name) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskProtocolName );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskProtocolName );
    }

    return state.setStatus( correct );
}

Status DicomMask::setRequestAttributeSequence( QString requestedProcedureID, QString scheduledProcedureStepID )
{
     Status state;
    // creem la sequencia

    retrieveLevel( SeriesMask );

    DcmSequenceOfItems *requestedAttributeSequence = new DcmSequenceOfItems( DCM_RequestAttributesSequence );

    DcmItem *requestedAttributeSequenceItem = new DcmItem( DCM_Item );
    requestedAttributeSequenceItem->putAndInsertString( DCM_RequestedProcedureID, qPrintable( requestedProcedureID ) );

    requestedAttributeSequenceItem->putAndInsertString( DCM_ScheduledProcedureStepID, qPrintable( scheduledProcedureStepID ) );

    requestedAttributeSequence->insert( requestedAttributeSequenceItem );
    m_mask->insert( requestedAttributeSequence, OFTrue );

    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag);
    }

    return state.setStatus( correct );
}

Status DicomMask::setPPSStartDate( QString startDate )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_PerformedProcedureStepStartDate );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(startDate) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag);
    }

    return state.setStatus( correct );
}

Status DicomMask::setPPStartTime( QString startTime )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_PerformedProcedureStepStartTime );

    retrieveLevel( SeriesMask );

    elem->putString( qPrintable(startTime) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag);
    }

    return state.setStatus( correct );
}

/************************************************ GET **************************************************************/

QString DicomMask::getSeriesNumber() const
{
    const char *value = NULL;
    QString seriesNumber;

    DcmTagKey seriesNumberTagKey (DCM_SeriesNumber);
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesNumberTagKey , value , OFFalse );

    if ( value != NULL ) seriesNumber.insert( 0 , value );

    return seriesNumber;
}

QString DicomMask::getSeriesDate() const
{
    const char *value = NULL;
    QString seriesDate;

    DcmTagKey seriesDateTagKey ( DCM_SeriesDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesDateTagKey , value , OFFalse );

    if ( value != NULL ) seriesDate.insert( 0 , value );

    return seriesDate;
}

QString DicomMask::getSeriesTime() const
{
    const char *value = NULL;
    QString seriesTime;

    DcmTagKey seriesTimeTagKey ( DCM_SeriesTime );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesTimeTagKey , value , OFFalse );

    if ( value != NULL ) seriesTime.insert( 0 , value );

    return seriesTime;
}

QString DicomMask::getSeriesDescription() const
{
    const char *value = NULL;
    QString seriesDescription;

    DcmTagKey seriesDescriptionTagKey ( DCM_SeriesDescription );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesDescriptionTagKey , value , OFFalse );

    if ( value != NULL ) seriesDescription.insert( 0 , value );

    return seriesDescription;
}

QString DicomMask::getSeriesModality() const
{
    const char *value = NULL;
    QString seriesModality;

    DcmTagKey seriesModalityTagKey ( DCM_Modality );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesModalityTagKey , value , OFFalse );

    if ( value != NULL ) seriesModality.insert( 0 , value );

    return seriesModality;
}

QString DicomMask::getSeriesOperator() const
{
    const char *value = NULL;
    QString operatorsName;

    DcmTagKey operatorsNameTagKey ( DCM_OperatorsName );
    OFCondition ec;
    ec = m_mask->findAndGetString( operatorsNameTagKey , value , OFFalse );

    if ( value != NULL ) operatorsName.insert( 0 ,  value );

    return operatorsName;
}

QString DicomMask::getSeriesBodyPartExaminated() const
{
    const char *value = NULL;
    QString bodyPartExaminated;

    DcmTagKey bodyPartExaminatedTagKey ( DCM_BodyPartExamined );
    OFCondition ec;
    ec = m_mask->findAndGetString( bodyPartExaminatedTagKey , value , OFFalse );

    if ( value != NULL ) bodyPartExaminated.insert( 0 ,  value );

    return bodyPartExaminated;
}

QString DicomMask::getSeriesProtocolName() const
{
    const char *value = NULL;
    QString ProtocolName;

    DcmTagKey ProtocolNameTagKey ( DCM_ProtocolName );
    OFCondition ec;
    ec = m_mask->findAndGetString( ProtocolNameTagKey , value , OFFalse );

    if ( value != NULL ) ProtocolName.insert( 0 , value );

    return ProtocolName;
}

QString DicomMask::getSeriesUID() const
{
    const char *value = NULL;
    QString seriesUID;

    DcmTagKey seriesUIDTagKey ( DCM_SeriesInstanceUID );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesUIDTagKey , value , OFFalse );

    if ( value != NULL ) seriesUID.insert( 0 , value );

    return seriesUID;
}

QString DicomMask::getRequestedProcedureID() const
{
    const char * value = NULL;
    QString requestedProcedureID;
    OFCondition ec;

    DcmSequenceOfItems *requestAttributesSequence;
    ec = m_mask->findAndGetSequence( DCM_RequestAttributesSequence , requestAttributesSequence , false );
    if ( requestAttributesSequence != NULL )
    {
        DcmItem * itemsSequence = requestAttributesSequence->getItem( 0 );

        itemsSequence->findAndGetString( DCM_RequestedProcedureID  , value , false );
        if ( value != NULL ) requestedProcedureID.insert( 0 , value );
    }

    return requestedProcedureID;
}

QString DicomMask::getScheduledProcedureStepID() const
{
    const char * value = NULL;
    QString procedureStepID;
    OFCondition ec;

    DcmSequenceOfItems *requestAttributesSequence;
    ec = m_mask->findAndGetSequence( DCM_RequestAttributesSequence , requestAttributesSequence , false );
    if ( requestAttributesSequence != NULL )
    {
        DcmItem * itemsSequence = requestAttributesSequence->getItem( 0 );

        itemsSequence->findAndGetString( DCM_ScheduledProcedureStepID , value , false );
        if ( value != NULL ) procedureStepID.insert( 0 , value );
    }

    return procedureStepID;
}

QString DicomMask::getPPSStartDate() const
{
    const char * value =NULL;
    QString startDate;

    DcmTagKey tagKey ( DCM_PerformedProcedureStepStartDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( tagKey , value , OFFalse );

    if ( value != NULL ) startDate.insert( 0 , value );

    return startDate;
}

QString DicomMask::getPPSStartTime() const
{
    const char * value =NULL;
    QString startTime;

    DcmTagKey tagKey ( DCM_PerformedProcedureStepStartTime );
    OFCondition ec;
    ec = m_mask->findAndGetString( tagKey , value , OFFalse );

    if ( value != NULL ) startTime.insert( 0 , value );

    return startTime;
}

/********************************************** IMAGE **************************************/

Status DicomMask:: setImageNumber( QString imgNum )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_InstanceNumber );

    retrieveLevel( ImageMask );

    elem->putString( qPrintable(imgNum) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInstanceNumber );
    }

    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInstanceNumber );
    }
    return state.setStatus( correct );
}

Status DicomMask:: setSOPInstanceUID( QString SOPInstanceUID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SOPInstanceUID );

    retrieveLevel( ImageMask );

    elem->putString( qPrintable(SOPInstanceUID) );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag );
    }

    //insert the tag SOPInstanceUID in the search mask
    m_mask->insert( elem , OFTrue );
    if ( m_mask->error() != EC_Normal )
    {
        return state.setStatus( error_MaskInsertTag );
    }
    return state.setStatus( correct );
}

QString DicomMask::getImageNumber() const
{
    const char *value = NULL;
    QString imageNumber;

    DcmTagKey instanceNumberTagKey ( DCM_InstanceNumber );
    OFCondition ec;
    ec = m_mask->findAndGetString( instanceNumberTagKey , value , OFFalse );

    if ( value != NULL ) imageNumber.insert( 0 , value );

    return imageNumber;
}

QString DicomMask::getSOPInstanceUID() const
{
    const char * value = NULL;
    QString SOPInstanceUID;

    DcmTagKey SOPInstanceUIDTagKey ( DCM_SOPInstanceUID );
    OFCondition ec;
    ec = m_mask->findAndGetString( SOPInstanceUIDTagKey  , value , OFFalse );

    if ( value != NULL ) SOPInstanceUID.insert( 0 , value );

    return SOPInstanceUID;
}

QString DicomMask::getRetrieveLevel() const
{
    const char * value = NULL;
    QString queryRetrieve;

    DcmTagKey tagKey ( DCM_QueryRetrieveLevel );
    OFCondition ec;
    ec = m_mask->findAndGetString( tagKey  , value , OFFalse );

    if ( value != NULL ) queryRetrieve.insert( 0 , value );

    return queryRetrieve;

}

DcmDataset* DicomMask::getDicomMask()
{
    return m_mask;
}

bool DicomMask::operator ==(const DicomMask &mask)
{
    if(    getStudyUID() == mask.getStudyUID()
        && getPatientId() == mask.getPatientId()
        && getPatientName() == mask.getPatientName()
        && getPatientBirth() == mask.getPatientBirth()
        && getPatientAge() == mask.getPatientAge()
        && getPatientSex() == mask.getPatientSex()
        && getStudyId() == mask.getStudyId()
        && getStudyDate() == mask.getStudyDate()
        && getStudyDescription() == mask.getStudyDescription()
        && getStudyModality() == mask.getStudyModality()
        && getStudyTime() == mask.getStudyTime()
        && getAccessionNumber() == mask.getAccessionNumber()
        && getInstitutionName() == mask.getInstitutionName()
        && getReferringPhysiciansName() == mask.getReferringPhysiciansName()
        && getSeriesNumber() == mask.getSeriesNumber()
        && getSeriesDate() == mask.getSeriesDate()
        && getSeriesTime() == mask.getSeriesTime()
        && getSeriesDescription() == mask.getSeriesDescription()
        && getSeriesModality() == mask.getSeriesModality()
        && getSeriesOperator() == mask.getSeriesOperator()
        && getSeriesBodyPartExaminated() == mask.getSeriesBodyPartExaminated()
        && getSeriesProtocolName() == mask.getSeriesProtocolName()
        && getSeriesUID() == mask.getSeriesUID()
        && getRequestedProcedureID() == mask.getRequestedProcedureID()
        && getScheduledProcedureStepID() == mask.getScheduledProcedureStepID()
        && getPPSStartDate() == mask.getPPSStartDate()
        && getPPSStartTime() == mask.getPPSStartTime()
        && getImageNumber() == mask.getImageNumber()
        && getSOPInstanceUID() == mask.getSOPInstanceUID()
        && getRetrieveLevel() == mask.getRetrieveLevel()
    )
        return true;
    else
        return false;
}

};

