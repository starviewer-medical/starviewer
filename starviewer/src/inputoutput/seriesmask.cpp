#include "seriesmask.h"
#include "status.h"

namespace udg{

SeriesMask::SeriesMask()
{
    m_seriesMask = new DcmDataset;
    retrieveLevel();
    }

void SeriesMask:: retrieveLevel()
{
    char val[15];
   
    DcmElement *elem = newDicomElement( DCM_QueryRetrieveLevel );

    strcpy( val , "SERIES" );
    elem->putString( val );

    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        printf("cannot insert tag: ");
    }
}

/************************* SEIRES  *********************************************************************/

Status SeriesMask:: setSeriesNumber( std::string seriesNumber )
{
    DcmElement *elem = newDicomElement( DCM_SeriesNumber );
    Status state;
    
    elem->putString( seriesNumber.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesNumber );
    }

    //insert the tag series Number in the search mask    
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( errorMaskSeriesNumber );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesDate( std::string date )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_SeriesDate );
    
    //pot venir la data amb format de 8 caracters, despres amb guio (9 càractes), o cerca entra dates (17 caràcters) 
    if ( date.length() != 8 && date.length() != 9 && date.length() != 17 && date.length() !=  0 )return state.setStatus( error_MaskLengthDate );
    
    elem->putString( date.c_str() );
    if ( elem->error() != EC_Normal )
    {
       return state.setStatus( errorMaskSeriesDate);
    }
    
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( errorMaskSeriesDate );
    }
    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesDescription( std::string desc )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_SeriesDescription );

    elem->putString( desc.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( errorMaskSeriesDescription );
    }
    
    //insert the tag SERIES DESCRIPTION in the search mask    
    m_seriesMask->insert( elem , OFTrue );
        
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( errorMaskSeriesDescription );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesModality( std::string modality )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_Modality );

    elem->putString( modality.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesModality );
    }
    
    //insert the tag series Modality in the search mask    
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskSeriesModality );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesTime( std::string time )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_SeriesTime );
    
    //la hora ha de ser de longitud 4 HHMM, o 5 HHMM- o -HHMM, o 9 HHMM-HHMM
    if ( time.length() != 4 && time.length() != 5 && time.length() != 9 && time.length() !=0 ) return state.setStatus( error_MaskLengthTime );
    
    elem->putString( time.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesTime );
    }
    
    //insert the tag SERIES TIME in the search mask    
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskSeriesTime );
    }
       
    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesUID( std::string seriesUID )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_SeriesInstanceUID );

    elem->putString( seriesUID.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskSeriesUID );
    }
    
    //insert the tag SERIES UID in the search mask    
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskSeriesUID );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setStudyUID( std::string studyUID )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_StudyInstanceUID );

    elem->putString( studyUID.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskStudyUID );
    }
    
    //insert the tag STUDY UID in the search mask    
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskStudyUID );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesOperator( std::string name )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_OperatorsName );

    elem->putString( name.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskOperatorName );
    }
     
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskOperatorName );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesBodyPartExaminated( std::string part )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_BodyPartExamined );

    elem->putString( part.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskBodyPartExaminated );
    }
     
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskBodyPartExaminated );
    }

    return state.setStatus( correct );
}

Status SeriesMask:: setSeriesProtocolName( std::string name )
{
    Status state;

    DcmElement *elem = newDicomElement( DCM_ProtocolName );

    elem->putString( name.c_str() );
    if ( elem->error() != EC_Normal )
    {
        return state.setStatus( error_MaskProtocolName );
    }
     
    m_seriesMask->insert( elem , OFTrue );
    if ( m_seriesMask->error() != EC_Normal ) 
    {
        return state.setStatus( error_MaskProtocolName );
    }

    return state.setStatus( correct );
}

/************************************************ GET **************************************************************/

std::string SeriesMask::getSeriesNumber()
{
    const char * seriesNum = NULL;
    std::string seriesNumber;
    
    DcmTagKey seriesNumberTagKey (DCM_SeriesNumber);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesNumberTagKey , seriesNum , OFFalse );
    
    if (seriesNum != NULL ) seriesNumber.insert( 0 , seriesNum );
        
    return seriesNum;
}

std::string SeriesMask::getSeriesDate()
{
    const char * date = NULL;
    std::string seriesDate;
    
    DcmTagKey seriesDateTagKey ( DCM_SeriesDate );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesDateTagKey , date , OFFalse );
    
    if ( date != NULL ) seriesDate.insert( 0 , date );
        
    return seriesDate;
}

std::string SeriesMask::getSeriesTime()
{
    const char * time = NULL;
    std::string seriesTime;
    
    DcmTagKey seriesTimeTagKey ( DCM_SeriesTime );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesTimeTagKey , time , OFFalse );
    
    if ( time != NULL ) seriesTime.insert( 0 , time );
        
    return seriesTime;
}

std::string SeriesMask::getSeriesDescription()
{
    const char * description = NULL;
    std::string seriesDescription;
    
    DcmTagKey seriesDescriptionTagKey ( DCM_SeriesDescription );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesDescriptionTagKey , description , OFFalse );
    
    if ( description != NULL ) seriesDescription.insert( 0 , description );
        
    return seriesDescription;
}

std::string SeriesMask::getSeriesModality()
{
    const char * modality = NULL;
    std::string seriesModality;
    
    DcmTagKey seriesModalityTagKey ( DCM_Modality );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesModalityTagKey , modality , OFFalse );
    
    if ( modality != NULL ) seriesModality.insert( 0 , modality );
        
    return seriesModality;
}

std::string SeriesMask::getSeriesOperator()
{
    const char * oper = NULL;
    std::string operatorsName;
    
    DcmTagKey operatorsNameTagKey ( DCM_OperatorsName );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( operatorsNameTagKey , oper , OFFalse );
    
    if ( oper != NULL ) operatorsName.insert( 0 ,  oper );
        
    return operatorsName;
}

std::string SeriesMask::getSeriesBodyPartExaminated()
{
    const char * bodyPart = NULL;
    std::string bodyPartExaminated;
    
    DcmTagKey bodyPartExaminatedTagKey ( DCM_BodyPartExamined );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( bodyPartExaminatedTagKey , bodyPart , OFFalse );
    
    if ( bodyPart != NULL ) bodyPartExaminated.insert( 0 ,  bodyPart );
        
    return bodyPartExaminated;
}

std::string SeriesMask::getSeriesProtocolName()
{
    const char * protocol = NULL;
    std::string ProtocolName;
    
    DcmTagKey ProtocolNameTagKey ( DCM_ProtocolName );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( ProtocolNameTagKey , protocol , OFFalse );
    
    if ( protocol != NULL ) ProtocolName.insert( 0 ,  protocol );
        
    return ProtocolName;
}

std::string SeriesMask::getSeriesUID()
{
    const char * UID = NULL;
    std::string seriesUID;
    
    DcmTagKey seriesUIDTagKey ( DCM_SeriesInstanceUID );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesUIDTagKey , UID , OFFalse );
    
    if (UID != NULL ) seriesUID.insert( 0 , UID);
        
    return seriesUID;
}

std::string SeriesMask::getStudyUID()
{
    const char * UID=NULL;
    std::string studyUID;
    
    DcmTagKey studyUIDTagKey ( DCM_StudyInstanceUID );
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( studyUIDTagKey , UID , OFFalse );
    
    if (UID != NULL ) studyUID.insert( 0 , UID);
        
    return studyUID;
}

DcmDataset* SeriesMask::getSeriesMask()
{
    return m_seriesMask;
}

};

