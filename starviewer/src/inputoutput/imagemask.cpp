#include "imagemask.h"
#include "status.h"



namespace udg{

/** Constructor 
 */
 
ImageMask::ImageMask()
{
    m_imageMask = new DcmDataset;
    retrieveLevel();
    
    //afegim els camps obligatoris
    setStudyUID(NULL);
    setSeriesUID(NULL);
    
}

/** This action especified that the query search, will use the retrieve level I. For any doubts about this retrieve level and the query/retrieve fields,
 consult DICOMS's documentation in Chapter 4, C.6.2.1
*/
//Per cada cerca a la màscara s'ha d'espeficiar el nivell al que anirem a buscar les dades, en aquest cas s'especifica image
void ImageMask:: retrieveLevel()
{

    char val[15];
   
    DcmElement *elem = newDicomElement(DCM_QueryRetrieveLevel);

    
    strcpy(val,"IMAGE");
    elem->putString(val);

    m_imageMask->insert(elem, OFTrue);
    if (m_imageMask->error() != EC_Normal) {
        printf("cannot insert tag: ");
    }

}

/** Set SeriesUID.
  *              @param Series UID
  *              @return The status of the action
  */
Status ImageMask:: setSeriesUID(const char *uid)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_SeriesInstanceUID);

    
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
    m_imageMask->insert(elem, OFTrue);
    if (m_imageMask->error() != EC_Normal) {
        return state.setStatus(error_MaskSeriesUID);
    }

    return state.setStatus(correct);
}


/** set the StudyId of the images
  *              @param   Study instance UID the study to search. If this parameter is null it's supose that any mask is applied at this field
  *              @return The state of the action
  */
Status ImageMask:: setStudyUID(const char *uid)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_StudyInstanceUID);

    
    if (uid == NULL)
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
    m_imageMask->insert(elem, OFTrue);
    if (m_imageMask->error() != EC_Normal) {
        return state.setStatus(error_MaskStudyUID);
    }

    return state.setStatus(correct);
}

/** Set ImageNumber.
  *              @param image Number
  *              @return The status of the action
  */
Status ImageMask:: setImageNumber(const char *imgNum)
{
    char val[70];
    val[0] = '\0';
    Status state;

    DcmElement *elem = newDicomElement(DCM_InstanceNumber);

    if (imgNum == NULL)
    {
        strcpy(val,"");
    }
    else if (strlen(imgNum)>0)
    {
        strcpy(val,imgNum);
    }
    else strcpy(val,"");
    
    elem->putString(val);
    if (elem->error() != EC_Normal)
    {
        return state.setStatus(error_MaskInstanceNumber);
    }

    //insert the tag SERIES UID in the search mask    
    m_imageMask->insert(elem, OFTrue);
    if (m_imageMask->error() != EC_Normal) {
        return state.setStatus(error_MaskInstanceNumber);
    }
    return state.setStatus(correct);
}


/**  Return the generated image mask
              @return returns the image mask
*/
DcmDataset* ImageMask::getImageMask()
{
    return m_imageMask;
}

};

