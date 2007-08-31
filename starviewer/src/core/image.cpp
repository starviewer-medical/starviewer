/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "image.h"

#include "logging.h"

#include <QStringList>
#include <QPainter>
#include <QBuffer>

#include <vtkMath.h> // pel ::Cross()

// fem servir dcmtk per l'escalat de les imatges dicom
//\TODO trobar perquè això és necessari amb les dcmtk
#define HAVE_CONFIG_H 1
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/ofstd/ofbmanip.h"

namespace udg {

Image::Image(QObject *parent)
 : QObject(parent), m_sliceThickness(1.0), m_numberOfFrames(1)
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

void Image::setImageOrientationPatient( double orientation[6] )
{
    memcpy( m_imageOrientationPatient, orientation, 6*sizeof(double) );

    double normal[3];
    // calculem la Z
    vtkMath::Cross( &orientation[0] , &orientation[3] , normal );

    memcpy( &m_imageOrientationPatient[6], normal, 3*sizeof(double) );
}

void Image::setImageOrientationPatient( double xVector[3], double yVector[3] )
{
    memcpy( m_imageOrientationPatient, xVector, 3*sizeof(double) );
    memcpy( &m_imageOrientationPatient[3], yVector, 3*sizeof(double) );
    double normal[3];
    // calculem la Z
    vtkMath::Cross( xVector , yVector , normal );

    memcpy( &m_imageOrientationPatient[6], normal, 3*sizeof(double) );
}

const double* Image::getImageOrientationPatient() const
{
    return m_imageOrientationPatient;
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
        retu