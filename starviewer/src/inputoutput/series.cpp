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
        path = text;
        path += "/";
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

void Series:: setSeriesNumber( QString number)
{
   m_seriesNumber = number;
}

void Series::setSeriesDescription( QString description )
{
    m_seriesDescription = description;
}

void Series::setSeriesDate( QString date )
{
    m_seriesDate = date;
}

void Series::setSeriesTime( QString time )
{
    m_seriesTime = time;
}

void Series::setSeriesModality( QString modality )
{
    m_seriesModality = modality;
}

void Series::setStudyUID( QString UID )
{
    m_studyUID = UID;
}

void Series::setSeriesUID( QString UID )
{
    m_seriesUID = UID;
}

void Series::setProtocolName( QString protocol )
{
    m_protocolName = protocol;
}

void Series::setBodyPartExaminated( QString part )
{
    m_bodyPartExaminated = part;
}

void Series::setOperatorName( QString name )
{
    m_operatorName = name;
}

void Series::setSeriesPath( QString path )
{
    m_seriesPath = path;
}

void Series::setImageNumber( int iNumber )
{
    m_imageNumber = iNumber;
}

void Series::setPacsAETitle( QString AETitlePACS )
{
    m_seriesAETitlePACS = AETitlePACS;
}

void Series::setRequestedProcedureID( QString requestedProcedureID )
{
    m_requestedProcedureID = requestedProcedureID;
}

void Series::setScheduledProcedureStepID( QString scheduledProcedureID )
{
    m_scheduledProcedureStepID = scheduledProcedureID;
}

void Series::setPPSStartDate( QString startDate )
{
    m_ppsStartDate = startDate;
}

void Series::setPPSStartTime( QString startTime )
{
    m_ppsStartTime = startTime;
}

/**********************************************************************************************************************/
/*                                    GET SERIES FIELDS                                                               */
/**********************************************************************************************************************/

QString Series:: getSeriesNumber()
{
    return m_seriesNumber;
}

QString Series::getSeriesDescription()
{
    return m_seriesDescription;
}

QString Series::getSeriesDate()
{
    return m_seriesDate;
}

QString Series::getSeriesTime()
{
    return m_seriesTime;
}

QString Series::getSeriesModality()
{
    return m_seriesModality;
}

QString Series::getStudyUID()
{
    return m_studyUID;
}

QString Series::getSeriesUID()
{
    return m_seriesUID;
}

QString Series::getProtocolName()
{
    return m_protocolName;
}

QString Series::getBodyPartExaminated()
{
    return m_bodyPartExaminated;
}

QString Series::getOperatorName()
{
    return m_operatorName;
}

QString Series::getSeriesPath()
{
    return m_seriesPath;
}

int Series::getImageNumber()
{
    return m_imageNumber;
}

QString Series::getRequestedProcedureID()
{
    return m_requestedProcedureID;
}

QString Series::getScheduledProcedureStepID()
{
    return m_scheduledProcedureStepID;
}

QString Series::getPPSStartDate()
{
    return m_ppsStartDate;
}

QString Series::getPPStartTime()
{
    return m_ppsStartTime;
}

QString Series::getPacsAETitle()
{
    return m_seriesAETitlePACS;
}

}




