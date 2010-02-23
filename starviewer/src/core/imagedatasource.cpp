#include "imagedatasource.h"

namespace udg {

ImageDataSource::ImageDataSource()
: m_numberOfFrames(1)
{
}

ImageDataSource::~ImageDataSource()
{
}

void ImageDataSource::setSOPInstanceUID( const QString &uid )
{
    m_SOPInstanceUID = uid;
}

QString ImageDataSource::getSOPInstanceUID() const
{
    return m_SOPInstanceUID;
}

void ImageDataSource::setInstanceNumber( const QString &number )
{
    m_instanceNumber = number;
}

QString ImageDataSource::getInstanceNumber() const
{
    return m_instanceNumber;
}

void ImageDataSource::setNumberOfFrames( int frames )
{
    m_numberOfFrames = frames;
}

int ImageDataSource::getNumberOfFrames() const
{
    return m_numberOfFrames;
}

bool ImageDataSource::isMultiFrame() const
{
    if( getNumberOfFrames() > 1 )
        return true;
    else
        return false;
}

void ImageDataSource::setImageType( const QString &imageType )
{
    m_imageType = imageType;
}

QString ImageDataSource::getImageType() const
{
    return m_imageType;
}

void ImageDataSource::setFilePath( const QString &filePath )
{
    m_filePath = filePath;
}

QString ImageDataSource::getFilePath() const
{
    return m_filePath;
}

}
