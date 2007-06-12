/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "image.h"

namespace udg {

Image::Image()
{
}

Image::Image(DcmDataset *imageDataset)
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

void Image::setSOPInstanceUID( QString UID )
{
    m_SoPUID = UID;
}

void Image::setImagePath( QString path )
{
    m_imagePath =  path;
}

void Image::setSeriesUID( QString UID )
{
    m_seriesUID = UID;
}

void Image::setImageName( QString name )
{
    m_imageName = name;
}

void Image::setStudyUID( QString UID )
{
    m_studyUID = UID;
}

void Image::setImageNumber( int imageNum )
{
    m_imageNumber = imageNum;
}

void Image::setImageSize( int bytes )
{
    m_imageSize = bytes;
}

void Image::setPacsAETitle( QString AETitle )
{
    m_pacsAETitle = AETitle;
}

QString Image::getSOPInstanceUID()
{
    return m_SoPUID;
}

QString Image::getImagePath()
{
    return m_imagePath;
}

QString Image::getSeriesUID()
{
    return m_seriesUID;
}

QString Image::getImageName()
{
    return m_imageName;
}

QString Image::getStudyUID()
{
    return m_studyUID;
}

int Image::getImageNumber()
{
    return m_imageNumber;
}

int Image::getImageSize()
{
    return m_imageSize;
}

QString Image::getPacsAETitle()
{
    return m_pacsAETitle;
}

Image::~Image()
{
}

};
