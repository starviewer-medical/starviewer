/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�ics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#define HAVE_CONFIG_H 1

#include "imagedicominformation.h"
#include "status.h"
#include <string>
#include <dcdeftag.h>


namespace udg {

ImageDicomInformation::ImageDicomInformation()
{

}

Status ImageDicomInformation:: openDicomFile(std::string imagePath)
{
    Status state;
    int opt_readAsDataset = 0;
    E_TransferSyntax    opt_transferSyntax = EXS_Unknown;
    DcmTagKey studyInstanceUIDTagKey( DCM_StudyInstanceUID );
    const char *SoPUID = NULL;
    
    m_dicomFile = new DcmFileFormat();
    
    OFCondition cond = m_dicomFile->loadFile( imagePath.c_str() , opt_transferSyntax , EGL_withoutGL , DCM_MaxReadLength , opt_readAsDataset );
        
    if ( !cond.good() ) return state.setStatus( cond );    
    
    m_dataset = m_dicomFile->getDataset();
        
    return state.setStatus( cond );
}

std::string ImageDicomInformation::getStudyUID()
{
    DcmTagKey studyInstanceUIDTagKey( DCM_StudyInstanceUID ); 
    const char *studyUID = NULL;

    OFCondition cond = m_dataset->findAndGetString( studyInstanceUIDTagKey , studyUID , OFFalse  );
       
    if ( !cond.good() || studyUID == NULL )
    {
        return "";
    }
    else return studyUID;
}

std::string ImageDicomInformation::getStudyID()
{
    DcmTagKey studyIDTagKey( DCM_StudyID ); 
    const char *studyID = NULL;

    OFCondition cond = m_dataset->findAndGetString( studyIDTagKey , studyID , OFFalse );
       
    if ( !cond.good() || studyID == NULL )
    {
        return "";
    }
    else return studyID;
}

std::string ImageDicomInformation::getSeriesUID()
{
    DcmTagKey seriesInstanceUIDTagKey( DCM_SeriesInstanceUID ); 
    const char *seriesInstanceUID = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesInstanceUIDTagKey , seriesInstanceUID , OFFalse );
       
    if ( !cond.good() || seriesInstanceUID == NULL )
    {
        return "";
    }
    else return seriesInstanceUID;
}

std::string ImageDicomInformation::getSeriesNumber()
{
    DcmTagKey seriesNumberTagKey( DCM_SeriesNumber );
    const char *seriesNumber = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesNumberTagKey , seriesNumber , OFFalse );
       
    if ( !cond.good() || seriesNumber == NULL )
    {
        return "";
    }
    else return seriesNumber;
}

std::string ImageDicomInformation::getSeriesModality()
{
    DcmTagKey seriesModalityTagKey( DCM_Modality ); 
    const char *seriesModality = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesModalityTagKey , seriesModality , OFFalse );
       
    if ( !cond.good() || seriesModality == NULL )
    {
        return "";
    }
    else return seriesModality;
}

std::string ImageDicomInformation::getSeriesProtocolName()
{
    DcmTagKey seriesProtocolNameTagKey( DCM_ProtocolName ); 
    const char *seriesProtocolName = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesProtocolNameTagKey , seriesProtocolName , OFFalse );
       
    if ( !cond.good() || seriesProtocolName == NULL )
    {
        return "";
    }
    else return seriesProtocolName;
}

std::string ImageDicomInformation::getSeriesDescription()
{
    DcmTagKey seriesDescriptionTagKey( DCM_SeriesDescription ); 
    const char *seriesDescription = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesDescriptionTagKey , seriesDescription , OFFalse );
       
    if ( !cond.good() || seriesDescription == NULL )
    {
        return "";
    }
    else if ( seriesDescription == NULL )
    {
        return "";
    }
    else return seriesDescription;
}

std::string ImageDicomInformation::getSeriesBodyPartExamined ()
{
    DcmTagKey seriesBodyPartExaminedTagKey( DCM_BodyPartExamined  ); 
    const char *seriesBodyPartExamined  = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesBodyPartExaminedTagKey , seriesBodyPartExamined , OFFalse );
       
    if ( !cond.good() || seriesBodyPartExamined == NULL )
    {
        return "";
    }
    else return seriesBodyPartExamined ;
}

std::string ImageDicomInformation::getSeriesTime()
{
    DcmTagKey seriesTimeTagKey( DCM_SeriesTime ); 
    const char *seriesTime = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesTimeTagKey , seriesTime , OFFalse );
       
    if ( !cond.good() || seriesTime == NULL )
    {
        return "";
    }
    else return seriesTime;
}

std::string ImageDicomInformation::getSeriesDate()
{
    DcmTagKey seriesDateTagKey( DCM_SeriesDate ); 
    const char *seriesDate = NULL;

    OFCondition cond = m_dataset->findAndGetString( seriesDateTagKey , seriesDate , OFFalse );
       
    if ( !cond.good() || seriesDate == NULL )
    {
        return "";
    }
    else return seriesDate;
}

ImageDicomInformation::~ImageDicomInformation()
{
}

}
