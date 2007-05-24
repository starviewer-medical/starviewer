#include "series.h"
#include "dcsequen.h"

namespace udg{

Series::Series()
{
    m_imageNumber = 0;
}

Series::Series(DcmDataset *seriesDataset)
{
    const char *text;
    string path;

    //set the series number
    seriesDataset->findAndGetString( DCM_SeriesNumber , text , false );
    if ( text != NULL ) setSeriesNumber( text );

    //set series date
    seriesDataset->findAndGetString( DCM_SeriesDate , text , false );
    if ( text != NULL ) setSeriesDate( text );

    //set series description
    seriesDataset->findAndGetString( DCM_SeriesDescription , text , false );
    if ( text != NULL ) setSeriesDescription( text );

    //set Study UID
    seriesDataset->findAndGetString( DCM_StudyInstanceUID , text , false );
    if ( text != NULL )
    {
        setStudyUID( text );
        path.insert( 0 , text );
        path.append( "/" );
    }
    //set series modality
    seriesDataset->findAndGetString( DCM_Modality , text , false );
    if ( text != NULL ) setSeriesModality( text );

    //set series time
    seriesDataset->findAndGetString( DCM_SeriesTime ,text , false );
    if ( text != NULL ) setSeriesTime( text );

    //set series UID
    seriesDataset->findAndGetString( DCM_SeriesInstanceUID , text , false );
    if ( text != NULL )
    {
        setSeriesUID( text );
        path.append( text );
        path.append( "/" );
    }

    seriesDataset->findAndGetString( DCM_BodyPartExamined , text , false );
    if ( text != NULL ) setBodyPartExaminated( text );

    seriesDataset->findAndGetString(DCM_ProtocolName , text , false );
    if ( text != NULL ) setProtocolName( text );

    seriesDataset->findAndGetString( DCM_OperatorsName , text , false );
    if ( text != NULL ) setOperatorName( text );

    seriesDataset->findAndGetString( DCM_RetrieveAETitle , text , false );
    if ( text != NULL ) setPacsAETitle( text );

    DcmSequenceOfItems *requestAttributesSequence;
    seriesDataset->findAndGetSequence( DCM_RequestAttributesSequence , requestAttributesSequence , false );
    if ( requestAttributesSequence != NULL )
    {
        DcmItem * itemsSequence = requestAttributesSequence->getItem( 0 );

        itemsSequence->findAndGetString( DCM_ScheduledProcedureStepID , text , false );
        if ( text != NULL ) setScheduledProcedureStepID( text );

        itemsSequence->findAndGetString( DCM_RequestedProcedureID , text , false );
        if ( text != NULL )  setRequestedProcedureID( text );
    }

    seriesDataset->findAndGetString( DCM_PerformedProcedureStepStartDate , text , false );
    if ( text != NULL ) setPPSStartDate( text );

    seriesDataset->findAndGetString( DCM_PerformedProcedureStepStartTime , text , false );
    if ( text != NULL ) setPPSStartTime( text );
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

void Series::setPacsAETitle( std::string AETitlePACS )
{
    m_seriesAETitlePACS.erase();
    m_seriesAETitlePACS.insert( 0 , AETitlePACS );
}

void Series::setRequestedProcedureID( std::string requestedProcedureID )
{
    m_requestedProcedureID = requestedProcedureID;
}

void Series::setScheduledProcedureStepID( std::string scheduledProcedureID )
{
    m_scheduledProcedureStepID = scheduledProcedureID;
}

void Series::setPPSStartDate( std::string startDate )
{
    m_ppsStartDate = startDate;
}

void Series::setPPSStartTime( std::string startTime )
{
    m_ppsStartTime = startTime;
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

std::string Series::getRequestedProcedureID()
{
    return m_requestedProcedureID;
}

std::string Series::getScheduledProcedureStepID()
{
    return m_scheduledProcedureStepID;
}

std::string Series::getPPSStartDate()
{
    return m_ppsStartDate;
}

std::string Series::getPPStartTime()
{
    return m_ppsStartTime;
}

std::string Series::getPacsAETitle()
{
    return m_seriesAETitlePACS;
}

}




