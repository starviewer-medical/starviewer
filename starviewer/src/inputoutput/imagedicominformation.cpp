/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "imagedicominformation.h"
#include "status.h"

namespace udg {

ImageDicomInformation::ImageDicomInformation()
{

}

Status ImageDicomInformation::openDicomFile(QString imagePath)
{
    Status state;
    return state.setStatus( "Tot bé?", m_dicomTagReader.setFile(imagePath), 0 );
}

QString ImageDicomInformation::getStudyUID()
{
    return m_dicomTagReader.getAttributeByName(DCM_StudyInstanceUID);
}

QString ImageDicomInformation::getStudyID()
{
    return m_dicomTagReader.getAttributeByName(DCM_StudyID);
}

QString ImageDicomInformation::getSeriesUID()
{
    return m_dicomTagReader.getAttributeByName(DCM_SeriesInstanceUID);
}

QString ImageDicomInformation::getSeriesNumber()
{
    return m_dicomTagReader.getAttributeByName(DCM_SeriesNumber);
}

QString ImageDicomInformation::getSeriesModality()
{
    return m_dicomTagReader.getAttributeByName(DCM_Modality);
}

QString ImageDicomInformation::getSeriesProtocolName()
{
    return m_dicomTagReader.getAttributeByName(DCM_ProtocolName);
}

QString ImageDicomInformation::getSeriesDescription()
{
    return m_dicomTagReader.getAttributeByName(DCM_SeriesDescription);
}

QString ImageDicomInformation::getSeriesBodyPartExamined ()
{
    return m_dicomTagReader.getAttributeByName(DCM_BodyPartExamined);
}

QString ImageDicomInformation::getSeriesTime()
{
    return m_dicomTagReader.getAttributeByName(DCM_SeriesTime);
}

QString ImageDicomInformation::getSeriesDate()
{
    return m_dicomTagReader.getAttributeByName(DCM_SeriesDate);
}

QString ImageDicomInformation::getSOPInstanceUID()
{
    return m_dicomTagReader.getAttributeByName(DCM_SOPInstanceUID);
}

QString ImageDicomInformation::getSOPClassUID()
{
    return m_dicomTagReader.getAttributeByName(DCM_SOPClassUID);
}

ImageDicomInformation::~ImageDicomInformation()
{
}

}
