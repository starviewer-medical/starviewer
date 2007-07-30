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

void Image::setInstanceNumber( QString number )
{
    m_instanceNumber = number;
}

void Image::setPatientOrientation( QString orientation )
{
    m_patientOrientation = orientation;
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

QString Image::getContentDateAsString()
{
    return m_contentDate.toString("dd/MM/yyyy");
}

QString Image::getContentTimeAsString()
{
    return m_contentTime.toString("HH:mm:ss");
}

void Image::setImagesInAcquisition( int images )
{
    m_imagesInAcquisition = images;
}

void Image::setComments( QString comments  )
{
    m_comments = comments;
}

void Image::setImagePosition( double position[3] )
{
    for(int i = 0; i < 3; i++)
        m_imagePosition[i] = position[i];
}

void Image::setSamplesPerPixel( int samples )
{
    m_samplesPerPixel = samples;
}

void Image::setPhotometricInterpretation( int value )
{
    m_photometricInterpretation = value;
}

void Image::setRows( int rows  )
{
    m_rows = rows;
}

void Image::setColumns( int columns  )
{
    m_columns = columns;
}

void Image::setParentSeries( Series *series )
{
    m_parentSeries = series;
}

void Image::setPath( QString path )
{
    m_path = path;
}

}
