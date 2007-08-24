/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "image.h"

#include "logging.h"

#include <QStringList>

#include <vtkMath.h> // pel ::Cross()

namespace udg {

Image::Image(QObject *parent)
 : QObject(parent), m_sliceThickness(1.0)
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

void Image::setPixelSpacing( double x, double y )
{
    m_pixelSpacing[0] = x;
    m_pixelSpacing[1] = y;
}

const double *Image::getPixelSpacing() const
{
    return m_pixelSpacing;
}

void Image::setSliceThickness( double z )
{
    m_sliceThickness = z;
}

double Image::getSliceThickness() const
{
    return m_sliceThickness;
}

void Image::setImagePositionPatient( double position[3] )
{
    memcpy( m_imagePositionPatient, position, 3 );
}

const double *Image::getImagePositionPatient() const
{
    return m_imagePositionPatient;
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

void Image::setBitsAllocated( int bits )
{
    m_bitsAllocated = bits;
}

int Image::getBitsAllocated() const
{
    return m_bitsAllocated;
}

void Image::setBitsStored( int bits )
{
    m_bitsStored = bits;
}

int Image::getBitsStored() const
{
    return m_bitsStored;
}

void Image::setParentSeries( Series *series )
{
    m_parentSeries = series;
}

Series *Image::getParentSeries() const
{
    return m_parentSeries;
}

void Image::setPath( QString path )
{
    m_path = path;
}

QString Image::getPath() const
{
    return m_path;
}

void Image::addReferencedImage( Image *image )
{
    QString uid = image->getSOPInstanceUID();
    if( uid.isEmpty() )
    {
        DEBUG_LOG("L'uid de la imatge està buit! No la podem insertar per inconsistent");
    }
    else
    {
        m_referencedImageSequence.push_back( image );
    }

}

Image *Image::getReferencedImage( QString SOPInstanceUID )
{
    int i = 0;
    bool found = false;
    while( i < m_referencedImageSequence.size() && !found )
    {
        if( m_referencedImageSequence.at(i)->getSOPInstanceUID() == SOPInstanceUID )
            found = true;
        else
            i++;
    }
    if( !found )
        return 0;
    else
        return m_referencedImageSequence.at(i);
}

QList<Image *> Image::getReferencedImages()
{
    return m_referencedImageSequence;
}

bool Image::hasReferencedImages() const
{
    return ! m_referencedImageSequence.isEmpty();
}

void Image::setCTLocalizer( bool localizer )
{
    m_CTLocalizer = localizer;
}

bool Image::isCTLocalizer() const
{
    return m_CTLocalizer;
}

}
