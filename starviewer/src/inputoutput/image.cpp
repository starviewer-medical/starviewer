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

void Image::setSoPUID( std::string UID )
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

std::string Image::getSoPUID()
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

Image::~Image()
{
}

};
