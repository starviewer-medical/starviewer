#include "series.h"

namespace udg{

Series::Series()
{
    m_imageNumber = 0;
}

/** This operator helps to us to sort the serieslist in oder by Series Number
  *
  */
bool Series::operator<( Series a )
{
    if (m_seriesNumber<a.getSeriesNumber())
    {
        return true;
    }
    else return false;
}

/***********************************************************************************************************************/
/*                                    SET STUDY FIELDS                                                                 */
/***********************************************************************************************************************/

void Series:: setSeriesNumber( std::string number)
{
   m_seriesNumber.erase();
   m_seriesNumber.insert( 0 , number );
}

void Series::setSeriesDescription( std::string description )
{
    m_seriesDescription.erase();
    m_seriesDescription.insert( 0 , description );
}

void Series::setSeriesDate( std::string date )
{
    m_seriesDate.erase();
    m_seriesDate.insert( 0 , date );
}

void Series::setSeriesTime( std::string time )
{
    m_seriesTime.erase();
    m_seriesTime.insert( 0 , time );
}

void Series::setSeriesModality( std::string modality )
{
    m_seriesModality.erase();
    m_seriesModality.insert( 0 , modality );
}

void Series::setStudyUID( std::string UID )
{
    m_studyUID.erase();
    m_studyUID.insert( 0 , UID );
}

void Series::setSeriesUID( std::string UID )
{
    m_seriesUID.erase();
    m_seriesUID.insert( 0 , UID );
}

void Series::setProtocolName( std::string protocol )
{
    m_protocolName.erase();
    m_protocolName.insert( 0 , protocol );
}

void Series::setBodyPartExaminated( std::string part )
{
    m_bodyPartExaminated.erase();
    m_bodyPartExaminated.insert( 0 , part );
}

void Series::setOperatorName( std::string name )
{
    m_operatorName.erase();
    m_operatorName.insert( 0 , name );
}

void Series::setSeriesPath( std::string path )
{
    m_seriesPath.erase();
    m_seriesPath.insert( 0 , path );
}

void Series::setImageNumber( int iNumber )
{
    m_imageNumber = iNumber;
}

/**********************************************************************************************************************/
/*                                    GET SERIES FIELDS                                                               */
/**********************************************************************************************************************/

std::string Series:: getSeriesNumber()
{
    return m_seriesNumber;
}

std::string Series::getSeriesDescription()
{
    return m_seriesDescription;
}

std::string Series::getSeriesDate()
{
    return m_seriesDate;
}

std::string Series::getSeriesTime()
{
    return m_seriesTime;
}

std::string Series::getSeriesModality()
{
    return m_seriesModality;
}

std::string Series::getStudyUID()
{
    return m_studyUID;
}

std::string Series::getSeriesUID()
{
    return m_seriesUID;
}

std::string Series::getProtocolName()
{
    return m_protocolName;
}

std::string Series::getBodyPartExaminated()
{
    return m_bodyPartExaminated;
}

std::string Series::getOperatorName()
{
    return m_operatorName;
}

std::string Series::getSeriesPath()
{
    return m_seriesPath;
}

int Series::getImageNumber()
{
    return m_imageNumber;
}

}




