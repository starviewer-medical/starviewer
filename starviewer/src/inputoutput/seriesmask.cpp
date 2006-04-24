#include "seriesmask.h"
#include "status.h"



namespace udg{

/** Constructor de la màscara de series, per defecte se li ha de passar com a mínim l'UID de l'estudi pel qual volem buscar les sèries
 *
 */
 
SeriesMask::SeriesMask()
{
    m_seriesMask = new DcmDataset;
    retrieveLevel();
    
/*    char val[70];
    val[0] = '\0';

    DcmElement *elem = newDicomElement(DCM_StudyInstanceUID );

    
    //if the patientId is null we supose that the user don't apply a criterium in this field
    if (studyUID==NULL)
    {
        strcpy(val,"*");
    }
    else if (strlen(studyUID)>0)
    {
        strcpy(val,studyUID);
    }
    else strcpy(val,"*");
    
    elem->putString(val);

    //insert the tag PATIENT ID in the search mask    
    seriesMask->insert(elem, OFTrue);*/

}

/** This action especified that the query search, will use the retrieve level Study. For any doubts about this retrieve level and the query/retrieve fields,
 consult DICOMS's documentation in Chapter 4, C.6.2.1
*/

void SeriesMask:: retrieveLevel()
{

    char val[15];
   
    DcmElement *elem = newDicomElement(DCM_QueryRetrieveLevel);

    
    strcpy(val,"SERIES");
    elem->putString(val);

    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        printf("cannot insert tag: ");
    }

}



/************************* SEIRES  *********************************************************************/

/** This action especified in the search which series number we want to match
  *              @param seriesNumber' Number of the series to search. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setSeriesNumber(std::string seriesNumber )
{

    DcmElement *elem = newDicomElement(DCM_SeriesNumber);
    Status state;
    
    elem->putString(seriesNumber.c_str());
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskSeriesNumber);
        
        
    }

    //insert the tag series Number in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(errorMaskSeriesNumber);
    }

    return state.setStatus(correct);
}


/** This action especified that in the search we want the series date. Date's format is YYYYMMDD
  *              @param series date of the series to search. If this parameter is null it's supose that any mask is applied at this field. 
  */
Status SeriesMask:: setSeriesDate(std::string date )
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesDate);
    
    //if the Date is null we supose that the user don't apply a criterium in this field
    if (date.length() != 8)
    return state.setStatus(error_MaskLengthDate);
    
    elem->putString(date.c_str());
    if (elem->error() != EC_Normal)
    {
       return state.setStatus(errorMaskSeriesDate);
    }
    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(errorMaskSeriesDate);
    }
    return state.setStatus(correct);
}

/** This action especified that in the search we want the series date between the two parameter dates. Date's format is YYYYMMDD
  *              @param Min date Study of the study to search.  
  *              @param Max date Study of the study to search.
  */
Status SeriesMask:: setSeriesDate(std::string dateMin,std::string dateMax )
{
    std::string value;
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesDate);

    //the length of the date must be 8 numbers. The correct format is YYYYMMDD   
    if (dateMin.length() != 8 || dateMax.length() != 8) 
    {
        return state.setStatus(error_MaskLengthDate);
    }
     
    value.insert( 0 , dateMin );
    value.append( "-" );
    value.append( dateMax );
    elem->putString( value.c_str() );
    
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskSeriesDate);
    }
    

    //insert the tag SERIES DATE in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(errorMaskSeriesDate);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the seriess description
  *              @param Series description of the study to search. If this parameter is null it's supose that any mask is applied at this field. 
  */
Status SeriesMask:: setSeriesDescription(std::string desc)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesDescription);

    elem->putString( desc.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(errorMaskSeriesDescription);
    }
    

    //insert the tag SERIES DESCRIPTION in the search mask    
    m_seriesMask->insert(elem, OFTrue);
        
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(errorMaskSeriesDescription);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the series modality
  *              @param series modality the study to search. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setSeriesModality(std::string modality)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_Modality);

    elem->putString( modality.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskSeriesModality);
    }
    
    //insert the tag series Modality in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskSeriesModality);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want the series time
  *              @param Series time the study to search. If this parameter is null it's supose that any mask is applied at this field. Time's format is HHMM
  */
Status SeriesMask:: setSeriesTime(std::string time)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesTime);
    
    //if the time is null we supose that the user don't apply a criterium in this field
    if ( time.length() != 4 )
    return state.setStatus(error_MaskLengthTime);
    
    elem->putString( time.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskSeriesTime);
    }
    

    //insert the tag SERIES TIME in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskSeriesTime);
    }
       
    return state.setStatus(correct);
}

/** This action especified that in the search we want to select series realized between these two times. Time's format is HHMM
  *              @param Min time  Study of the series to search.  
  *              @param Max time Study of the series to search.
  */
Status SeriesMask:: setSeriesTime(std::string timeMin,std::string timeMax)
{
    std::string value;
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesTime);

    //the length of the time must be 4 numbers. The correct format is YYYYMMDD
    
    if ( timeMin.length () !=4 || timeMax.length() != 4 ) 
    {
        return state.setStatus(error_MaskLengthTime);
    }
     
    value.insert( 0 , timeMin );
    value.append( "-" );
    value.append( timeMax );
    
    elem->putString( value.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskSeriesTime);
    }
    

    //insert the tag SERIES TIME in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskSeriesTime);
    }
       
    return state.setStatus(correct);
}


/** This action especified that in the search we want the Series instance UID
  *              @param Series instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setSeriesUID(std::string seriesUID)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesInstanceUID);

    elem->putString( seriesUID.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskSeriesUID);
    }
    

    //insert the tag SERIES UID in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskSeriesUID);
    }

    return state.setStatus(correct);
}


/** This action especified that in the search we want to query the series that have this Study UID
  *              @param Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setStudyUID(std::string studyUID)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_StudyInstanceUID);

    elem->putString( studyUID.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskStudyUID);
    }
    

    //insert the tag STUDY UID in the search mask    
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskStudyUID);
    }

    return state.setStatus(correct);
}


/** This action especified that in the search we want to query the operator's name
  *              @param Operator's name. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setSeriesOperator(std::string name)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_OperatorsName);

    elem->putString( name.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskOperatorName);
    }
     
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskOperatorName);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want to query the body part examinated
  *              @param Body Part. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setSeriesBodyPartExaminated(std::string part)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_BodyPartExamined);

    elem->putString( part.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskBodyPartExaminated);
    }
     
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskBodyPartExaminated);
    }

    return state.setStatus(correct);
}

/** This action especified that in the search we want to query the Protocol Name
  *              @param Protocol Name. If this parameter is null it's supose that any mask is applied at this field
  */
Status SeriesMask:: setSeriesProtocolName(std::string name)
{
    Status state;

    DcmElement *elem = newDicomElement(DCM_ProtocolName);

    elem->putString( name.c_str() );
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskProtocolName);
    }
     
    m_seriesMask->insert(elem, OFTrue);
    if (m_seriesMask->error() != EC_Normal) {
        return state.setStatus(error_MaskProtocolName);
    }

    return state.setStatus(correct);
}


/************************************************ GET **************************************************************/

/** Retorna el series Number
  *            @return   series Number 
  */
std::string SeriesMask::getSeriesNumber( )
{
    const char * seriesNum = NULL;
    std::string seriesNumber;
    
    DcmTagKey seriesNumberTagKey (DCM_SeriesNumber);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesNumberTagKey, seriesNum, OFFalse );;
    
    if (seriesNum != NULL) seriesNumber.insert(0,seriesNum);
        
    return seriesNum;
}


std::string SeriesMask::getSeriesDate( )
{
    const char * date = NULL;
    std::string seriesDate;
    
    DcmTagKey seriesDateTagKey (DCM_SeriesDate);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesDateTagKey, date, OFFalse );;
    
    if (date != NULL) seriesDate.insert(0,date);
        
    return seriesDate;
}


std::string SeriesMask::getSeriesTime( )
{
    const char * time = NULL;
    std::string seriesTime;
    
    DcmTagKey seriesTimeTagKey (DCM_SeriesTime);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesTimeTagKey, time, OFFalse );;
    
    if (time != NULL) seriesTime.insert(0,time);
        
    return seriesTime;
}

std::string SeriesMask::getSeriesDescription( )
{
    const char * description = NULL;
    std::string seriesDescription;
    
    DcmTagKey seriesDescriptionTagKey (DCM_SeriesDescription);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesDescriptionTagKey, description, OFFalse );;
    
    if (description != NULL) seriesDescription.insert(0,description);
        
    return seriesDescription;
}

std::string SeriesMask::getSeriesModality( )
{
    const char * modality = NULL;
    std::string seriesModality;
    
    DcmTagKey seriesModalityTagKey (DCM_Modality);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesModalityTagKey, modality, OFFalse );;
    
    if (modality != NULL) seriesModality.insert(0,modality);
        
    return seriesModality;
}

std::string SeriesMask::getSeriesOperator( )
{
    const char * oper = NULL;
    std::string operatorsName;
    
    DcmTagKey operatorsNameTagKey (DCM_OperatorsName);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( operatorsNameTagKey, oper, OFFalse );;
    
    if (oper != NULL) operatorsName.insert(0, oper);
        
    return operatorsName;
}


std::string SeriesMask::getSeriesBodyPartExaminated( )
{
    const char * bodyPart = NULL;
    std::string bodyPartExaminated;
    
    DcmTagKey bodyPartExaminatedTagKey (DCM_BodyPartExamined);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( bodyPartExaminatedTagKey, bodyPart, OFFalse );;
    
    if (bodyPart != NULL) bodyPartExaminated.insert(0, bodyPart);
        
    return bodyPartExaminated;
}

std::string SeriesMask::getSeriesProtocolName( )
{
    const char * protocol = NULL;
    std::string ProtocolName;
    
    DcmTagKey ProtocolNameTagKey (DCM_ProtocolName);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( ProtocolNameTagKey, protocol, OFFalse );;
    
    if (protocol != NULL) ProtocolName.insert(0, protocol);
        
    return ProtocolName;
}

std::string SeriesMask::getSeriesUID()
{
    const char * UID = NULL;
    std::string seriesUID;
    
    DcmTagKey seriesUIDTagKey (DCM_SeriesInstanceUID);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( seriesUIDTagKey, UID, OFFalse );;
    
    if (UID != NULL) seriesUID.insert(0,UID);
        
    return seriesUID;
}

std::string SeriesMask::getStudyUID()
{
    const char * UID=NULL;
    std::string studyUID;
    
    DcmTagKey studyUIDTagKey (DCM_StudyInstanceUID);
    OFCondition ec;
    ec = m_seriesMask->findAndGetString( studyUIDTagKey, UID, OFFalse );;
    
    if (UID != NULL) studyUID.insert(0,UID);
        
    return studyUID;
}


/**  Return the generated search mask
              @return returns de search mask
*/
DcmDataset* SeriesMask::getSeriesMask()
{
    return m_seriesMask;
}

};

