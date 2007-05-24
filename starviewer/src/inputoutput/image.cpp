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

void Image::setSOPInstanceUID( std::string UID )
{
    m_SoPUID.erase();
    m_SoPUID.insert( 0 , UID );
}

void Image::setImagePath( std::string path )
{
    m_imagePath.erase();
    m_imagePath.insert( 0 , path );
}

void Image::setSeriesUID( std::string UID )
{
    m_seriesUID.erase();
    m_seriesUID.insert( 0 , UID );
}

void Image::setImageName( std::string name )
{
    m_imageName.erase();
    m_imageName.insert( 0 , name );
}

void Image::setStudyUID( std::string UID )
{
    m_studyUID.erase();
    m_studyUID.insert( 0 , UID );
}

void Image::setImageNumber( int imageNum )
{
    m_imageNumber = imageNum;
}

void Image::setImageSize( int bytes )
{
    m_imageSize = bytes;
}

void Image::setPacsAETitle( string AETitle )
{
    m_pacsAETitle = AETitle;
}

std::string Image::getSOPInstanceUID()
{
    return m_SoPUID;
}

std::string Image::getImagePath()
{
    return m_imagePath;
}

std::string Image::getSeriesUID()
{
    return m_seriesUID;
}

std::string Image::getImageName()
{
    return m_imageName;
}

std::string Image::getStudyUID()
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

string Image::getPacsAETitle()
{
    return m_pacsAETitle;
}

Image::~Image()
{
}

};
