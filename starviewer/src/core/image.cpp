/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "image.h"

#include "logging.h"

#include <QStringList>

#define HAVE_CONFIG_H 1
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"

namespace udg {

Image::Image(QObject *parent)
 : QObject(parent)
{
}

Image::~Image()
{
}

void Image::setSOPInstanceUID( QString uid )
{
    m_SOPInstanceUID = uid;
}

QString Image::getSOPInstanceUID() const
{
    return m_SOPInstanceUID;
}

void Image::setInstanceNumber( QString number )
{
    m_instanceNumber = number;
}

QString Image::getInstanceNumber() const
{
    return m_instanceNumber;
}

void Image::setImageOrientation( double orientation[6] )
{
    for(int i = 0; i < 6; i++)
    {
        m_imageOrientation[i] = orientation[i];
    }
}

const double* Image::getImageOrientation() const
{
    return m_imageOrientation;
}

void Image::setPatientOrientation( QString orientation )
{
    m_patientOrientation = orientation;
}

QString Image::getPatientOrientation() const
{
    return m_patientOrientation;
}

bool Image::setContentDateTime( int day , int month , int year , int hour , int minute, int seconds )
{
    return this->setContentDate( day, month, year ) && this->setContentTime( hour, minute, seconds );
}

bool Image::setContentDateTime( QString date , QString time )
{
    return this->setContentDate( date ) && this->setContentTime( time );
}

bool Image::setContentDate( int day , int month , int year )
{
    bool ok = true;
    if( !m_contentDate.setDate( year, month, day ) )
    {
        ok = false;
        DEBUG_LOG("Error en el format de la data");
    }
    return ok;
}

bool Image::setContentDate( QString date )
{
    QStringList split = date.split("/");
    bool ok = false;
    if( split.size() == 3 )
    {
        ok = this->setContentDate( split.at(0).toInt(), split.at(1).toInt(), split.at(2).toInt() );
    }
    else
        DEBUG_LOG("La data està en un mal format-> " + date );

    return ok;
}

bool Image::setContentTime( int hour , int minute, int seconds )
{
    bool ok = true;
    if( !m_contentTime.setHMS( hour, minute, seconds ) )
    {
        ok = false;
        DEBUG_LOG("Error en el format de la hora");
    }
    return ok;
}

bool Image::setContentTime( QString time )
{
    QStringList split = time.split(":");
    bool ok = false;
    switch( split.size() )
    {
    case 2:
        ok = this->setContentTime( split.at(0).toInt(), split.at(1).toInt() );
    break;

    case 3:
        ok = this->setContentTime( split.at(0).toInt(), split.at(1).toInt(), split.at(2).toInt() );
    break;

    default:
        DEBUG_LOG("La hora està en un mal format-> " + time );
    break;
    }

    return ok;
}

QDate Image::getContentDate() const
{
    return m_contentDate;
}

QString Image::getContentDateAsString()
{
    return m_contentDate.toString("dd/MM/yyyy");
}

QTime Image::getContentTime() const
{
    return m_contentTime;
}

QString Image::getContentTimeAsString()
{
    return m_contentTime.toString("HH:mm:ss");
}

void Image::setImagesInAcquisition( int images )
{
    m_imagesInAcquisition = images;
}

int Image::getImagesInAcquisition() const
{
    return m_imagesInAcquisition;
}

void Image::setComments( QString comments  )
{
    m_comments = comments;
}

QString Image::getComments() const
{
    return m_comments;
}

void Image::setImagePosition( double position[3] )
{
}

const double *Image::getImagePosition() const
{
    return m_imagePosition;
}

void Image::setSamplesPerPixel( int samples )
{
    m_samplesPerPixel = samples;
}

int Image::getSamplesPerPixel() const
{
    return m_samplesPerPixel;
}

void Image::setPhotometricInterpretation( int value )
{
    m_photometricInterpretation = value;
}

int Image::getPhotometricInterpretation() const
{
    return m_photometricInterpretation;
}

void Image::setRows( int rows  )
{
    m_rows = rows;
}

int Image::getRows() const
{
    return m_rows;
}

void Image::setColumns( int columns  )
{
    m_columns = columns;
}

int Image::getColumns() const
{
    return m_columns;
}

void Image::setParentSeries( Series *series )
{
    m_parentSeries = series;
}

void Image::setPath( QString path )
{
    m_path = path;
}

QString Image::getPath() const
{
    return m_path;
}

}
