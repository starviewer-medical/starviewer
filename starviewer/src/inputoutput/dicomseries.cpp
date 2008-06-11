#include "dicomseries.h"

#include <dimse.h> // provide the structure DcmDataSet
#include <dcdeftag.h> //provide the information for the tags
#include <dcsequen.h>

namespace udg{

DICOMSeries::DICOMSeries()
{
    m_imageNumber = 0;
}

DICOMSeries::DICOMSeries(DcmDataset *seriesDataset)
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
    if ( text != NULL ) setSeriesDescription( QString::fromLatin1( text ) );

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
    if ( text != NULL ) setProtocolName( QString::fromLatin1( text ) );

    seriesDataset->findAndGetString( DCM_OperatorsName , text , false );
    if ( text != NULL ) setOperatorName( QString::fromLatin1( text ) );

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
bool DICOMSeries::operator<( DICOMSeries series )
{
    if( m_seriesNumber < series.getSeriesNumber() )
    {
        return true;
    }
    else return false;
}

/***********************************************************************************************************************/
/*                                    SET STUDY FIELDS                                                                 */
/***********************************************************************************************************************/

void DICOMSeries:: setSeriesNumber( QString number)
{
   m_seriesNumber = number;
}

void DICOMSeries::setSeriesDescription( QString description )
{
    m_seriesDescription = description;
}

void DICOMSeries::setSeriesDate( QString date )
{
    m_seriesDate = date;
}

void DICOMSeries::setSeriesTime( QString time )
{
    m_seriesTime = time;
}

void DICOMSeries::setSeriesModality( QString modality )
{
    m_seriesModality = modality;
}

void DICOMSeries::setStudyUID( QString UID )
{
    m_studyUID = UID;
}

void DICOMSeries::setSeriesUID( QString UID )
{
    m_seriesUID = UID;
}

void DICOMSeries::setProtocolName( QString protocol )
{
    m_protocolName = protocol;
}

void DICOMSeries::setBodyPartExaminated( QString part )
{
    m_bodyPartExaminated = part;
}

void DICOMSeries::setOperatorName( QString name )
{
    m_operatorName = name;
}

void DICOMSeries::setSeriesPath( QString path )
{
    m_seriesPath = path;
}

void DICOMSeries::setImageNumber( int iNumber )
{
    m_imageNumber = iNumber;
}

void DICOMSeries::setPacsAETitle( QString AETitlePACS )
{
    m_seriesAETitlePACS = AETitlePACS;
}

void DICOMSeries::setRequestedProcedureID( QString requestedProcedureID )
{
    m_requestedProcedureID = requestedProcedureID;
}

void DICOMSeries::setScheduledProcedureStepID( QString scheduledProcedureID )
{
    m_scheduledProcedureStepID = scheduledProcedureID;
}

void DICOMSeries::setPPSStartDate( QString startDate )
{
    m_ppsStartDate = startDate;
}

void DICOMSeries::setPPSStartTime( QString startTime )
{
    m_ppsStartTime = startTime;
}

/**********************************************************************************************************************/
/*                                    GET SERIES FIELDS                                                               */
/**********************************************************************************************************************/

QString DICOMSeries:: getSeriesNumber()
{
    return m_seriesNumber;
}

QString DICOMSeries::getSeriesDescription()
{
    return m_seriesDescription;
}

QString DICOMSeries::getSeriesDate()
{
    return m_seriesDate;
}

QString DICOMSeries::getSeriesTime()
{
    return m_seriesTime;
}

QString DICOMSeries::getSeriesModality()
{
    return m_seriesModality;
}

QString DICOMSeries::getStudyUID()
{
    return m_studyUID;
}

QString DICOMSeries::getSeriesUID()
{
    return m_seriesUID;
}

QString DICOMSeries::getProtocolName()
{
    return m_protocolName;
}

QString DICOMSeries::getBodyPartExaminated()
{
    return m_bodyPartExaminated;
}

QString DICOMSeries::getOperatorName()
{
    return m_operatorName;
}

QString DICOMSeries::getSeriesPath()
{
    return m_seriesPath;
}

int DICOMSeries::getImageNumber()
{
    return m_imageNumber;
}

QString DICOMSeries::getRequestedProcedureID()
{
    return m_requestedProcedureID;
}

QString DICOMSeries::getScheduledProcedureStepID()
{
    return m_scheduledProcedureStepID;
}

QString DICOMSeries::getPPSStartDate()
{
    return m_ppsStartDate;
}

QString DICOMSeries::getPPStartTime()
{
    return m_ppsStartTime;
}

QString DICOMSeries::getPacsAETitle()
{
    return m_seriesAETitlePACS;
}

}




