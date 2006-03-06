#include "series.h"
#include <iostream>
#include <list>


namespace udg{


/** Initialize the fields of the series
*/
Series::Series()
{

    m_imageNumber = 0;

}

/** This operator helps to us to sort the serieslist in oder by Series Number
  *
  */
bool Series::operator<(Series a)
{
    if (m_seriesNumber<a.getSeriesNumber())
    {
        return true;
    }
    else return false;
}               

/*******************************************************************************************************************************/
/*                                    SET STUDY FIELDS                                                                         */
/*******************************************************************************************************************************/

/** This action set the series Number
  *              @param Series Number
  */
void Series:: setSeriesNumber(std::string number)
{
   m_seriesNumber.erase();
   m_seriesNumber.insert(0,number);
}

/** This action set the series description 
  *              @param Series description
  */
void Series::setSeriesDescription(std::string description)
{
    m_seriesDescription.erase();
    m_seriesDescription.insert(0,description);
}

/** This action set the series date.  
  *              @param Series date  
  */
void Series::setSeriesDate(std::string date)
{
    m_seriesDate.erase();
    m_seriesDate.insert(0,date);
}

/** This action set the series time.  
  *              @param Series time  
  */
void Series::setSeriesTime(std::string time)
{
    m_seriesTime.erase();
    m_seriesTime.insert(0,time);
}

/** This action set the series modality
  *              @param series modality
  */
void Series::setSeriesModality(std::string modality)
{
    m_seriesModality.erase();
    m_seriesModality.insert(0,modality);
}

/** This action set the study UID          
  *              @param  study UID
  */
void Series::setStudyUID(std::string UID)
{
    m_studyUID.erase();
    m_studyUID.insert(0,UID);
}

/** This action set the series UID        
  *              @param   Series UID 
  */
void Series::setSeriesUID(std::string UID)
{
    m_seriesUID.erase();
    m_seriesUID.insert(0,UID);
}

/** This action set the series Protocol name       
  *              @param   Series Protocol 
  */
void Series::setProtocolName(std::string protocol)
{
    m_protocolName.erase();
    m_protocolName.insert(0,protocol);
}

/** This action set the body part examinated  
  *              @param Body part examinated in the series 
  */
void Series::setBodyPartExaminated(std::string part)
{
    m_bodyPartExaminated.erase();
    m_bodyPartExaminated.insert(0,part);
}

/** This action set the operator's name 
  *              @param  Series name 
  */
void Series::setOperatorName(std::string name)
{
    m_operatorName.erase();
    m_operatorName.insert(0,name);
}

/** This action set series paths in de local data base 
  *              @param  Series path
  */
void Series::setSeriesPath(std::string path)
{
    m_seriesPath.erase();
    m_seriesPath.insert(0,path);
}

/** Estableix el número d'imatges que conté la serie
  *             @param Número d'imatges
  */
void Series::setImageNumber(int iNumber)
{
    m_imageNumber = iNumber;
}

/*******************************************************************************************************************************/
/*                                    GET SERIES FIELDS                                                                         */
/*******************************************************************************************************************************/


/** This function get the Series Number  
  *              @return  Series Number  
  */
std::string Series:: getSeriesNumber( )
{

    return m_seriesNumber;
}

/** This function get the Series Description  
  *              @return  Series Description  
  */
std::string Series::getSeriesDescription( )
{
    return m_seriesDescription;
}

/** This function get the Series Date    
  *              @return  Series Date    
  */
std::string Series::getSeriesDate( )
{
    return m_seriesDate;
}

/** This function get the Series Time    
  *              @return  Series Time   
  */
std::string Series::getSeriesTime( )
{
    return m_seriesTime;
}

/** This function get the Series Modality
  *              @return  Series Modality
  */
std::string Series::getSeriesModality( )
{
    return m_seriesModality;
}

/** This function get the Study UID   
  *              @return  Study UID         
  */
std::string Series::getStudyUID( )
{
    return m_studyUID;
}

/** This function get the Series UID  
  *              @return  Series's UID  
  */
std::string Series::getSeriesUID( )
{
    return m_seriesUID;
}

/** This function get the Protocol Name  
  *              @return  Protocol Name 
  */
std::string Series::getProtocolName( )
{
    return m_protocolName;
}

/** This function get the Body Part Examinated
  *              @return  SBody Part Examinated 
  */
std::string Series::getBodyPartExaminated( )
{
    return m_bodyPartExaminated;
}

/** This function get the operator name  
  *              @return  operator name   
  */
std::string Series::getOperatorName( )
{
    return m_operatorName;
}

/** This function get the series path in the local data base
  *              @return  path 
  */
std::string Series::getSeriesPath( )
{
    return m_seriesPath;
}

/** retorna el número d'imatges que conté la serie
  *             @return Número d'imatges
  */
int Series::getImageNumber()
{
    return m_imageNumber;
}

}




