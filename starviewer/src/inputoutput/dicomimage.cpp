/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "dicomimage.h"

namespace udg {

DICOMImage::DICOMImage()
{
}

DICOMImage::DICOMImage(DcmDataset *imageDataset)
{
    const char *text;

    imageDataset->findAndGetString( DCM_StudyInstanceUID , text , false );
    if ( text != NULL ) setStudyUID( text );

    imageDataset->findAndGetString( DCM_SeriesInstanceUID , text , false );
    if ( text != NULL ) setSeriesUID( text );

    imageDataset->findAndGetString( DCM_SOPInstanceUID , text , false );
    if ( text != NULL ) setSOPInstanceUID( text );

    imageDataset->findAndGetString( DCM_InstanceNumber , text , false );
    if ( text != NULL )
    {
        setImageNumber ( atoi ( text ) );
    }
    //TODO mirar perquè posem un número d'imatge per defecte si no el tenen
    else setImageNumber( 99999 ); //algunes imatges no tenen número d'imatge per defecte els hi posem aquest

    imageDataset->findAndGetString( DCM_RetrieveAETitle , text , false );
    if ( text != NULL ) setPacsAETitle( text );

}

DICOMImage::~DICOMImage()
{
}


void DICOMImage::setSOPInstanceUID( QString UID )
{
    m_SoPUID = UID;
}

void DICOMImage::setImagePath( QString path )
{
    m_imagePath =  path;
}

void DICOMImage::setSeriesUID( QString UID )
{
    m_seriesUID = UID;
}

void DICOMImage::setImageName( QString name )
{
    m_imageName = name;
}

void DICOMImage::setStudyUID( QString UID )
{
    m_studyUID = UID;
}

void DICOMImage::setImageNumber( int imageNum )
{
    m_imageNumber = imageNum;
}

void DICOMImage::setImageSize( int bytes )
{
    m_imageSize = bytes;
}

void DICOMImage::setPacsAETitle( QString AETitle )
{
    m_pacsAETitle = AETitle;
}

QString DICOMImage::getSOPInstanceUID()
{
    return m_SoPUID;
}

QString DICOMImage::getImagePath()
{
    return m_imagePath;
}

QString DICOMImage::getSeriesUID()
{
    return m_seriesUID;
}

QString DICOMImage::getImageName()
{
    return m_imageName;
}

QString DICOMImage::getStudyUID()
{
    return m_studyUID;
}

int DICOMImage::getImageNumber()
{
    return m_imageNumber;
}

int DICOMImage::getImageSize()
{
    return m_imageSize;
}

QString DICOMImage::getPacsAETitle()
{
    return m_pacsAETitle;
}

};
