#include <string>

#include "dicommask.h"
#include "status.h"

#define HAVE_CONFIG_H 1
#include "dcsequen.h"

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
//     elemImplementationVersionName->putString( implementationVersionName.c_str() );
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

Status DicomMask::setPatientId( std::string patientID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_PatientID) ;

    retrieveLevel( StudyMask );

    elem->putString( patientID.c_str() );
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

Status DicomMask::setPatientName( std::string patientName )
{
    Status state;
    DcmElement *elem = newDicomElement(DCM_PatientsName);

    retrieveLevel( StudyMask );

    elem->putString( patientName.c_str() );
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

Status DicomMask::setPatientBirth( std::string date )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_PatientsBirthDate );

    retrieveLevel( StudyMask );

    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() != 0 ) return state.setStatus( error_MaskLengthDate );

    elem->putString( date.c_str() );
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

Status DicomMask::setPatientSex( std::string patientSex )
{
    Status state;
    DcmElement *elem = newDicomElement(DCM_PatientsSex );

    retrieveLevel( StudyMask );

    elem->putString( patientSex.c_str() );
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

Status DicomMask::setPatientAge( std::string patientAge )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_PatientsAge );

    retrieveLevel( StudyMask );

    elem->putString( patientAge.c_str() );

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


std::string DicomMask::getPatientId()
{
    const char *value = NULL;
    std::string patientID;

    DcmTagKey patientIDTagKey ( DCM_PatientID );
    OFCondition ec;

    ec = m_mask->findAndGetString( patientIDTagKey , value , OFFalse );

    if ( value != NULL ) patientID.insert( 0 , value );

    return patientID;
}

std::string DicomMask::getPatientName()
{
    const char *value = NULL;
    std::string patientName;

    DcmTagKey patientIDTagKey ( DCM_PatientsName );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientIDTagKey , value , OFFalse );

    if ( value != NULL ) patientName.insert( 0 , value );

    return patientName;
}

std::string DicomMask::getPatientBirth()
{
    const char *value = NULL;
    std::string patientBirth;

    DcmTagKey patientBirthTagKey ( DCM_PatientsBirthDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientBirthTagKey , value , OFFalse );


    if ( value != NULL ) patientBirth.insert( 0 , value );

    return patientBirth;
}

std::string DicomMask::getPatientSex()
{
    const char *value = NULL;
    std::string patientSex;

    DcmTagKey patientSexTagKey ( DCM_PatientsSex );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientSexTagKey , value , OFFalse );

    if ( value != NULL ) patientSex.insert( 0 , value );

    return patientSex;
}

std::string DicomMask::getPatientAge()
{
    const char *value = NULL;
    std::string patientAge;

    DcmTagKey patientAgeTagKey ( DCM_PatientsAge );
    OFCondition ec;
    ec = m_mask->findAndGetString( patientAgeTagKey , value , OFFalse );

    if ( value != NULL ) patientAge.insert( 0 , value );

    return patientAge;
}

/****************************************** STUDY *****************************************************/

Status DicomMask:: setStudyId( std::string studyID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyID );

    retrieveLevel( StudyMask );

    elem->putString( studyID.c_str() );

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

Status DicomMask:: setStudyDate( std::string date )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyDate );

    retrieveLevel( StudyMask );

    //pot venir la data amb format de 8 caracters, despres amb guio (9 càractes), o cerca entra dates (17 caràcters)
    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() != 0 ) return state.setStatus( error_MaskLengthDate );

    elem->putString( date.c_str() );

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

Status DicomMask:: setStudyDescription( std::string desc )
{
    Status state;
    DcmElement *elem = newDicomElement(DCM_StudyDescription);

    retrieveLevel( StudyMask );

    elem->putString( desc.c_str() );
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

Status DicomMask:: setStudyModality( std::string modality )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_ModalitiesInStudy );

    retrieveLevel( StudyMask );

    elem->putString( modality.c_str() );
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

Status DicomMask:: setStudyTime( std::string time )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyTime );

    retrieveLevel( StudyMask );

    elem->putString( time.c_str() );
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

Status DicomMask:: setStudyUID( std::string studyUID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_StudyInstanceUID );

    retrieveLevel( StudyMask );

    elem->putString( studyUID.c_str() );
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

Status DicomMask:: setInstitutionName( std::string institution )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_InstitutionName );

    retrieveLevel( StudyMask );

    elem->putString( institution.c_str() );
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

Status DicomMask:: setAccessionNumber( std::string accession )
{
    char val[100];
    val[0] = '\0';
    std::string value;
    Status state;
    DcmElement *elem = newDicomElement( DCM_AccessionNumber );

    retrieveLevel( StudyMask );

    elem->putString( accession.c_str() );

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

Status DicomMask::setReferringPhysiciansName( std::string physiciansName )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_ReferringPhysiciansName );

    retrieveLevel( StudyMask );

    elem->putString( physiciansName.c_str() );

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

std::string DicomMask::getStudyUID()
{
    const char * value =NULL;
    std::string studyUID;

    DcmTagKey studyUIDTagKey ( DCM_StudyInstanceUID );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyUIDTagKey , value , OFFalse );;

    if ( value != NULL ) studyUID.insert( 0 , value );

    return studyUID;
}

std::string DicomMask::getStudyId()
{
    const char *value = NULL;
    std::string studyID;

    DcmTagKey studyIDTagKey ( DCM_StudyID );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyIDTagKey , value , OFFalse );

    if ( value != NULL ) studyID.insert( 0 , value );

    return studyID;
}

std::string DicomMask::getStudyDate()
{
    const char * value = NULL;
    std::string studyDate;

    DcmTagKey studyDateTagKey ( DCM_StudyDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyDateTagKey , value , OFFalse );

    if ( value != NULL ) studyDate.insert( 0 , value );

    return studyDate;
}

std::string DicomMask::getStudyDescription()
{
    const char *value = NULL;
    std::string studyDescription;

    DcmTagKey studyDescriptionTagKey ( DCM_StudyDescription );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyDescriptionTagKey , value , OFFalse );

    if ( value != NULL ) studyDescription.insert (0 , value );

    return studyDescription;
}

std::string DicomMask::getStudyTime()
{
    const char *value = NULL;
    std::string studyTime;

    DcmTagKey studyTimeTagKey ( DCM_StudyTime );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyTimeTagKey , value , OFFalse );

    if ( value != NULL ) studyTime.insert( 0 , value );

    return studyTime;
}

std::string DicomMask::getStudyModality()
{
    const char *value = NULL;
    std::string studyModality;

    DcmTagKey studyModalityTagKey ( DCM_ModalitiesInStudy );
    OFCondition ec;
    ec = m_mask->findAndGetString( studyModalityTagKey , value , OFFalse );

    if ( value != NULL ) studyModality.insert( 0 , value );

    return studyModality;
}

std::string DicomMask::getAccessionNumber()
{
    const char *value = NULL;
    std::string accessionNumber;

    DcmTagKey accessionNumberTagKey ( DCM_AccessionNumber );
    OFCondition ec;
    ec = m_mask->findAndGetString( accessionNumberTagKey , value , OFFalse );

    if ( value != NULL ) accessionNumber.insert( 0 , value );

    return accessionNumber;
}

std::string DicomMask::getInstitutionName()
{
    const char *value = NULL;
    std::string institutionName;

    DcmTagKey institutionNameTagKey (DCM_InstitutionName);
    OFCondition ec;
    ec = m_mask->findAndGetString( institutionNameTagKey , value , OFFalse );

    if ( value != NULL ) institutionName.insert( 0 , value );

    return institutionName;
}

std::string DicomMask::getReferringPhysiciansName()
{
    const char * value = NULL;
    std::string referringPhysiciansName;

    DcmTagKey referringPhysiciansNameTagKey ( DCM_ReferringPhysiciansName );
    OFCondition ec;
    ec = m_mask->findAndGetString( referringPhysiciansNameTagKey , value , OFFalse );

    if ( value != NULL ) referringPhysiciansName.insert( 0 , value );

    return referringPhysiciansName;
}

/************************************** SERIES *************************************************/

Status DicomMask:: setSeriesNumber( std::string seriesNumber )
{
    DcmElement *elem = newDicomElement( DCM_SeriesNumber );
    Status state;

    retrieveLevel( SeriesMask );

    elem->putString( seriesNumber.c_str() );
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

Status DicomMask:: setSeriesDate( std::string date )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesDate );

    retrieveLevel( SeriesMask );

    //pot venir la data amb format de 8 caracters, despres amb guio (9 càractes), o cerca entra dates (17 caràcters)
    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() !=  0 )return state.setStatus( error_MaskLengthDate );

    elem->putString( date.c_str() );
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

Status DicomMask:: setSeriesDescription( std::string desc )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesDescription );

    retrieveLevel( SeriesMask );

    elem->putString( desc.c_str() );
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

Status DicomMask:: setSeriesModality( std::string modality )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_Modality );

    retrieveLevel( SeriesMask );

    elem->putString( modality.c_str() );
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

Status DicomMask:: setSeriesTime( std::string time )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesTime );

    retrieveLevel( SeriesMask );

    //la hora ha de ser de longitud 4 HHMM, o 5 HHMM- o -HHMM, o 9 HHMM-HHMM
    if ( time.length() != 4 && time.length() != 5 && time.length() != 9 && time.length() !=0 ) return state.setStatus( error_MaskLengthTime );

    elem->putString( time.c_str() );
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

Status DicomMask:: setSeriesUID( std::string seriesUID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SeriesInstanceUID );

    retrieveLevel( SeriesMask );

    elem->putString( seriesUID.c_str() );
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

Status DicomMask:: setSeriesOperator( std::string name )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_OperatorsName );

    retrieveLevel( SeriesMask );

    elem->putString( name.c_str() );
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

Status DicomMask:: setSeriesBodyPartExaminated( std::string part )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_BodyPartExamined );

    retrieveLevel( SeriesMask );

    elem->putString( part.c_str() );
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

Status DicomMask:: setSeriesProtocolName( std::string name )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_ProtocolName );

    retrieveLevel( SeriesMask );

    elem->putString( name.c_str() );
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

Status DicomMask::setPPSStartDate( std::string startDate )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_PerformedProcedureStepStartDate );

    retrieveLevel( SeriesMask );

    elem->putString( startDate.c_str() );
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

Status DicomMask::setPPStartTime( std::string startTime )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_PerformedProcedureStepStartTime );

    retrieveLevel( SeriesMask );

    elem->putString( startTime.c_str() );
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

std::string DicomMask::getSeriesNumber()
{
    const char *value = NULL;
    std::string seriesNumber;

    DcmTagKey seriesNumberTagKey (DCM_SeriesNumber);
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesNumberTagKey , value , OFFalse );

    if ( value != NULL ) seriesNumber.insert( 0 , value );

    return seriesNumber;
}

std::string DicomMask::getSeriesDate()
{
    const char *value = NULL;
    std::string seriesDate;

    DcmTagKey seriesDateTagKey ( DCM_SeriesDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesDateTagKey , value , OFFalse );

    if ( value != NULL ) seriesDate.insert( 0 , value );

    return seriesDate;
}

std::string DicomMask::getSeriesTime()
{
    const char *value = NULL;
    std::string seriesTime;

    DcmTagKey seriesTimeTagKey ( DCM_SeriesTime );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesTimeTagKey , value , OFFalse );

    if ( value != NULL ) seriesTime.insert( 0 , value );

    return seriesTime;
}

std::string DicomMask::getSeriesDescription()
{
    const char *value = NULL;
    std::string seriesDescription;

    DcmTagKey seriesDescriptionTagKey ( DCM_SeriesDescription );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesDescriptionTagKey , value , OFFalse );

    if ( value != NULL ) seriesDescription.insert( 0 , value );

    return seriesDescription;
}

std::string DicomMask::getSeriesModality()
{
    const char *value = NULL;
    std::string seriesModality;

    DcmTagKey seriesModalityTagKey ( DCM_Modality );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesModalityTagKey , value , OFFalse );

    if ( value != NULL ) seriesModality.insert( 0 , value );

    return seriesModality;
}

std::string DicomMask::getSeriesOperator()
{
    const char *value = NULL;
    std::string operatorsName;

    DcmTagKey operatorsNameTagKey ( DCM_OperatorsName );
    OFCondition ec;
    ec = m_mask->findAndGetString( operatorsNameTagKey , value , OFFalse );

    if ( value != NULL ) operatorsName.insert( 0 ,  value );

    return operatorsName;
}

std::string DicomMask::getSeriesBodyPartExaminated()
{
    const char *value = NULL;
    std::string bodyPartExaminated;

    DcmTagKey bodyPartExaminatedTagKey ( DCM_BodyPartExamined );
    OFCondition ec;
    ec = m_mask->findAndGetString( bodyPartExaminatedTagKey , value , OFFalse );

    if ( value != NULL ) bodyPartExaminated.insert( 0 ,  value );

    return bodyPartExaminated;
}

std::string DicomMask::getSeriesProtocolName()
{
    const char *value = NULL;
    std::string ProtocolName;

    DcmTagKey ProtocolNameTagKey ( DCM_ProtocolName );
    OFCondition ec;
    ec = m_mask->findAndGetString( ProtocolNameTagKey , value , OFFalse );

    if ( value != NULL ) ProtocolName.insert( 0 , value );

    return ProtocolName;
}

std::string DicomMask::getSeriesUID()
{
    const char *value = NULL;
    std::string seriesUID;

    DcmTagKey seriesUIDTagKey ( DCM_SeriesInstanceUID );
    OFCondition ec;
    ec = m_mask->findAndGetString( seriesUIDTagKey , value , OFFalse );

    if ( value != NULL ) seriesUID.insert( 0 , value );

    return seriesUID;
}

std::string DicomMask::getRequestedProcedureID()
{
    const char * value = NULL;
    std::string requestedProcedureID;
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

std::string DicomMask::getScheduledProcedureStepID()
{
    const char * value = NULL;
    std::string procedureStepID;
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

std::string DicomMask::getPPSStartDate()
{
    const char * value =NULL;
    std::string startDate;

    DcmTagKey tagKey ( DCM_PerformedProcedureStepStartDate );
    OFCondition ec;
    ec = m_mask->findAndGetString( tagKey , value , OFFalse );

    if ( value != NULL ) startDate.insert( 0 , value );

    return startDate;
}

std::string DicomMask::getPPSStartTime()
{
    const char * value =NULL;
    std::string startTime;

    DcmTagKey tagKey ( DCM_PerformedProcedureStepStartTime );
    OFCondition ec;
    ec = m_mask->findAndGetString( tagKey , value , OFFalse );

    if ( value != NULL ) startTime.insert( 0 , value );

    return startTime;
}

/********************************************** IMAGE **************************************/

Status DicomMask:: setImageNumber( std::string imgNum )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_InstanceNumber );

    retrieveLevel( ImageMask );

    elem->putString( imgNum.c_str() );
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

Status DicomMask:: setSOPInstanceUID( std::string SOPInstanceUID )
{
    Status state;
    DcmElement *elem = newDicomElement( DCM_SOPInstanceUID );

    retrieveLevel( ImageMask );

    elem->putString( SOPInstanceUID.c_str() );
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

std::string DicomMask::getImageNumber()
{
    const char *value = NULL;
    std::string imageNumber;

    DcmTagKey instanceNumberTagKey ( DCM_InstanceNumber );
    OFCondition ec;
    ec = m_mask->findAndGetString( instanceNumberTagKey , value , OFFalse );

    if ( value != NULL ) imageNumber.insert( 0 , value );

    return imageNumber;
}

std::string DicomMask::getSOPInstanceUID()
{
    const char * value = NULL;
    std::string SOPInstanceUID;

    DcmTagKey SOPInstanceUIDTagKey ( DCM_SOPInstanceUID );
    OFCondition ec;
    ec = m_mask->findAndGetString( SOPInstanceUIDTagKey  , value , OFFalse );

    if ( value != NULL ) SOPInstanceUID.insert( 0 , value );

    return SOPInstanceUID;
}

std::string DicomMask::getRetrieveLevel()
{
    const char * value = NULL;
    std::string queryRetrieve;

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

};

