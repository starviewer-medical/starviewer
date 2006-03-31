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
Status SeriesMask:: setSeriesNumber(const char *seriesNumber )
{
    char val[70];
    val[0] = '\0';

    DcmElement *elem = newDicomElement(DCM_SeriesNumber);
    Status state;
    
    //if the seriesNumber is null we supose that the user don't apply a criterium in this field
    if (seriesNumber==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(seriesNumber)>0)
    {
        strcpy(val,seriesNumber);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesDate(const char *date )
{
    char val[10];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesDate);
    
    //if the Date is null we supose that the user don't apply a criterium in this field
    if (date==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(date)==8)
    {
        strcpy(val,date);
    }
    else return state.setStatus(error_MaskLengthDate);
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesDate(const char *dateMin,const char* dateMax )
{
    char val[20];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesDate);

    //the length of the date must be 8 numbers. The correct format is YYYYMMDD   
    if (strlen(dateMin)!=8 || strlen(dateMax)!=8) 
    {
        return state.setStatus(error_MaskLengthDate);
    }
     
    strcpy(val,dateMin);
    strcat(val,"-");
    strcat(val,dateMax);
    elem->putString(val);
    
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
Status SeriesMask:: setSeriesDescription(const char *desc)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesDescription);

    
    //if the desc is null we supose that the user don't apply a criterium in this field
    if (desc==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(desc)>0)
    {
        strcpy(val,desc);
    }
    else strcpy(val,"");

    elem->putString(val);
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
Status SeriesMask:: setSeriesModality(const char *modality)
{
    char val[50];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_Modality);

    
    //if the modality is null we supose that the user don't apply a criterium in this field
    if (modality==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(modality)>0)
    {
        strcpy(val,modality);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesTime(const char *time)
{
    char val[6];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesTime);
    
    //if the time is null we supose that the user don't apply a criterium in this field
    if (time==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(time)==4)
    {
        strcpy(val,time);
    }
    else return state.setStatus(error_MaskLengthTime);
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesTime(const char *timeMin,const char *timeMax)
{
    char val[14];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesTime);

    //the length of the time must be 4 numbers. The correct format is YYYYMMDD
    
    if (strlen(timeMin)!=4 || strlen(timeMax)!=4) 
    {
        return state.setStatus(error_MaskLengthTime);
    }
     
    strcpy(val,timeMin);
    strcat(val,"-");
    strcat(val,timeMax);
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesUID(const char *uid)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesInstanceUID);

    
    //if the time is null we supose that the user don't apply a criterium in this field
    if (uid==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(uid)>0)
    {
        strcpy(val,uid);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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
Status SeriesMask:: setStudyUID(const char *uid)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_StudyInstanceUID);

    //if the time is null we supose that the user don't apply a criterium in this field
    if (uid==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(uid)>0)
    {
        strcpy(val,uid);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesOperator(const char *name)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_OperatorsName);

    
    //if the is null we supose that the user don't apply a criterium in this field
    if (name==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(name)>0)
    {
        strcpy(val,name);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesBodyPartExaminated(const char *part)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_BodyPartExamined);

    
    //if the is null we supose that the user don't apply a criterium in this field
    if (part==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(part)>0)
    {
        strcpy(val,part);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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
Status SeriesMask:: setSeriesProtocolName(const char *name)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_ProtocolName);

    
    //if the is null we supose that the user don't apply a criterium in this field
    if (name==NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(name)>0)
    {
        strcpy(val,name);
    }
    else strcpy(val,"");
    
    elem->putString(val);
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


/**  Return the generated search mask
              @return returns de search mask
*/
DcmDataset* SeriesMask::getSeriesMask()
{
    return m_seriesMask;
}

};

