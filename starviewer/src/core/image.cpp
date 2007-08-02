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

bool Image::setContentDateTime(int day, int month, int year, int hour, int minute, int second)
{
    return this->setContentDate( day, month, year ) && this->setContentTime( hour, minute, second );
}

bool Image::setContentDateTime(QString date, QString time)
{
    return this->setContentDate( date ) && this->setContentTime( time );
}

bool Image::setContentDate(int day, int month, int year)
{
    return setContentDate( QDate(year, month, day) );
}

bool Image::setContentDate(QString date)
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format yyyy.MM.dd
    return setContentDate( QDate::fromString(date.remove("."), "yyyyMMdd") );
}

bool Image::setContentDate(QDate date)
{
    if (date.isValid())
    {
        m_contentDate = date;
        return true;
    }
    else
    {
        DEBUG_LOG("La data està en un mal format" );
        return false;
    }
}

bool Image::setContentTime(int hour, int minutes, int second)
{
    return setContentTime( QTime(hour, minutes, second) );
}

bool Image::setContentTime( QString time )
{
    // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format hh:mm:ss.frac
    time = time.remove(":");

    QStringList split = time.split(".");
    QTime convertedTime = QTime::fromString(split[0], "hhmmss");

    if (split.size() == 2) //té fracció al final
    {
        // Trunquem a milisegons i no a milionèssimes de segons
        convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
    }

    return setContentTime( convertedTime );
}

bool Image::setContentTime(QTime time)
{
    if (time.isValid())
    {
        m_contentTime = time;
        return true;
    }
    else
    {
        DEBUG_LOG("El time està en un mal format" );
        return false;
    }
}

QDate Image::getContentDate() const
{
    return m_contentDate;
}

QString Image::getContentDateAsString()
{
    return m_contentDate.toString(Qt::LocaleDate);
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
    for(int i = 0; i < 3; ++i)
    {
        m_imagePosition[i] = position[i];
    }
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
