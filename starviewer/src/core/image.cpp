/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "image.h"

#include "series.h"
#include "logging.h"
#include "thumbnailcreator.h"
#include "mathtools.h"

#include <QFileInfo>

namespace udg {

Image::Image(QObject *parent)
 : QObject(parent), m_sliceThickness(0.0), m_samplesPerPixel(1), m_photometricInterpretation("MONOCHROME2"), m_rows(0), m_columns(0), m_bitsAllocated(16), m_bitsStored(16), m_pixelRepresentation(0), m_rescaleSlope(1), m_rescaleIntercept(0), m_parentSeries(NULL), m_frameNumber(0), m_phaseNumber(0), m_volumeNumberInSeries(0), m_orderNumberInVolume(0)
{
    m_pixelSpacing[0] = 0.;
    m_pixelSpacing[1] = 0.;
    memset( m_imageOrientationPatient, 0, 9*sizeof(double) );
    memset( m_imagePositionPatient, 0, 3*sizeof(double) );
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

    // calculem la Z
    double normal[3];
    MathTools::crossProduct( &orientation[0] , &orientation[3], normal );

    memcpy( &m_imageOrientationPatient[6], normal, 3*sizeof(double) );
}

void Image::setImageOrientationPatient( double xVector[3], double yVector[3] )
{
    memcpy( m_imageOrientationPatient, xVector, 3*sizeof(double) );
    memcpy( &m_imageOrientationPatient[3], yVector, 3*sizeof(double) );

    // calculem la Z
    double normal[3];
    MathTools::crossProduct( xVector , yVector, normal );

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

QString Image::getOrientationLabel()
{
    return getProjectionLabelFromPlaneOrientation( m_patientOrientation );
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
    memcpy( m_imagePositionPatient, position, 3*sizeof(double) );
}

const double *Image::getImagePositionPatient() const
{
    return m_imagePositionPatient;
}

void Image::getImagePlaneNormal( double normal[3] )
{
	normal[0] = m_imageOrientationPatient[6];
	normal[1] = m_imageOrientationPatient[7];
	normal[2] = m_imageOrientationPatient[8];
}

void Image::setSamplesPerPixel( int samples )
{
    m_samplesPerPixel = samples;
}

int Image::getSamplesPerPixel() const
{
    return m_samplesPerPixel;
}

void Image::setPhotometricInterpretation( QString value )
{
    m_photometricInterpretation = value;
}

QString Image::getPhotometricInterpretation() const
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

void Image::setHighBit( int highBit )
{
    m_highBit = highBit;
}

int Image::getHighBit() const
{
    return m_highBit;
}

void Image::setPixelRepresentation( int representation )
{
    m_pixelRepresentation = representation;
}

int Image::getPixelRepresentation() const
{
    return m_pixelRepresentation;
}

void Image::setRescaleSlope( double slope )
{
    m_rescaleSlope = slope;
}

double Image::getRescaleSlope() const
{
    return m_rescaleSlope;
}

void Image::setRescaleIntercept( double intercept )
{
    m_rescaleIntercept = intercept;
}

double Image::getRescaleIntercept() const
{
    return m_rescaleIntercept;
}

void Image::setSliceLocation( QString sliceLocation )
{
    m_sliceLocation = sliceLocation;
}

QString Image::getSliceLocation() const
{
    return m_sliceLocation;
}

void Image::addWindowLevel( double window, double level )
{
    QPair<double, double> windowLevel( window, level );
    m_windowLevelList << windowLevel;
}

QPair<double,double> Image::getWindowLevel( int index ) const
{
    if( index >= 0 && index < m_windowLevelList.size() )
        return m_windowLevelList.at(index);
    else
    {
        DEBUG_LOG("Index out of range");
        return QPair<double,double>();
    }
}

int Image::getNumberOfWindowLevels()
{
    return m_windowLevelList.size();
}

void Image::addWindowLevelExplanation( QString explanation )
{
    m_windowLevelExplanationList << explanation;
}

void Image::setWindowLevelExplanations( const QStringList &explanations )
{
    m_windowLevelExplanationList = explanations;
}

QString Image::getWindowLevelExplanation( int index ) const
{
    if( index >= 0 && index < m_windowLevelExplanationList.size() )
        return m_windowLevelExplanationList.at(index);
    else
    {
        return QString();
    }
}

void Image::setRetrievedDate(QDate retrievedDate)
{
    m_retrievedDate = retrievedDate;
}

void Image::setRetrievedTime(QTime retrievedTime)
{
    m_retrieveTime = retrievedTime;
}

QDate Image::getRetrievedDate()
{
    return m_retrievedDate;
}

QTime Image::getRetrievedTime()
{
    return m_retrieveTime;
}

void Image::setImageType( const QString &imageType )
{
    m_imageType = imageType;
}

QString Image::getImageType() const
{
    return m_imageType;
}

void Image::setViewPosition( const QString &viewPosition )
{
    m_viewPosition = viewPosition;
}

QString Image::getViewPosition() const
{
    return m_viewPosition;
}

void Image::setImageLaterality( const QChar &imageLaterality )
{
    m_imageLaterality = imageLaterality;
}

QChar Image::getImageLaterality() const
{
    return m_imageLaterality;
}

void Image::setViewCodeMeaning( const QString &viewCodeMeaning )
{
    m_viewCodeMeaning = viewCodeMeaning;
}

QString Image::getViewCodeMeaning() const
{
    return m_viewCodeMeaning;
}

void Image::setFrameNumber( int frameNumber )
{
    m_frameNumber = frameNumber;
}

int Image::getFrameNumber() const
{
    return m_frameNumber;
}

void Image::setPhaseNumber( int phaseNumber )
{
    m_phaseNumber = phaseNumber;
}

int Image::getPhaseNumber() const
{
    return m_phaseNumber;
}

void Image::setVolumeNumberInSeries(int volumeNumberInSeries)
{
    m_volumeNumberInSeries = volumeNumberInSeries;
}

int Image::getVolumeNumberInSeries() const
{
    return m_volumeNumberInSeries;
}

void Image::setOrderNumberInVolume(int orderNumberInVolume)
{
    m_orderNumberInVolume = orderNumberInVolume;
}

int Image::getOrderNumberInVolume() const
{
    return m_orderNumberInVolume;
}

void Image::setImageTime( const QString &imageTime )
{
    m_imageTime = imageTime ;
}

QString Image::getImageTime() const
{
    return m_imageTime;
}

QString Image::getFormattedImageTime() const
{
    QString formattedTime = m_imageTime;
    if( !formattedTime.isEmpty() )
    {
        // Seguim la suggerència de la taula 6.2-1 de la Part 5 del DICOM standard de tenir en compte el format hh:mm:ss.frac
        formattedTime = formattedTime.remove(":");

        QStringList split = formattedTime.split(".");
        QTime convertedTime = QTime::fromString(split[0], "hhmmss");

        if (split.size() == 2) // Té fracció al final
        {
            // Trunquem a milisegons i no a milionèssimes de segons
            convertedTime = convertedTime.addMSecs( split[1].leftJustified(3,'0',true).toInt() );
        }
        formattedTime = convertedTime.toString("HH:mm:ss");
    }

    return formattedTime;
}

QString Image::getKeyIdentifier() const
{
    return m_SOPInstanceUID + "#" + QString::number(m_frameNumber);
}

void Image::setParentSeries( Series *series )
{
    m_parentSeries = series;
    this->setParent( m_parentSeries );
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

QPixmap Image::getThumbnail(bool getFromCache, int resolution)
{
    ThumbnailCreator thumbnailCreator;
    bool createThumbnail = true;

    if (m_thumbnail.isNull())
    {
        if( getFromCache )
        {
            // Primer provem de trobar el thumbnail amb número de volum i després sense número de volum
            // Si no trobem cap fitxer, l'haurem de crear de nou
            
            // Obtenim el directori base on es pot trobar el thumbnail
            QString thumbnailPath = QFileInfo( getPath() ).absolutePath();
            // Path absolut de l'arxiu de thumbnail
            QString thumbnailFilePath = QString("%1/thumbnail%2.pgm").arg(thumbnailPath).arg(getVolumeNumberInSeries());
            
            QFileInfo thumbnailFile( thumbnailFilePath );
            if( thumbnailFile.exists() )
            {
                m_thumbnail = QPixmap(thumbnailFilePath);
                createThumbnail = false;
            }
            else
            {
                thumbnailFilePath = QString("%1/thumbnail.pgm").arg(thumbnailPath);
                thumbnailFile.setFile( thumbnailFilePath );
                if ( thumbnailFile.exists() )
                {
                    m_thumbnail = QPixmap( thumbnailFilePath );
                    createThumbnail = false;
                }
            }
        }

        if( createThumbnail )
        {
            m_thumbnail = QPixmap::fromImage(thumbnailCreator.getThumbnail(this, resolution));
        }
    }
    return m_thumbnail;
}

QString Image::getProjectionLabelFromPlaneOrientation( const QString &orientation )
{
    QString label;
    
    QStringList axisList = orientation.split("\\");
    // comprovem si tenim les annotacions esperades
    if( axisList.count() >= 2 )
    {
        QString rowAxis = axisList.at(0).trimmed();
        QString columnAxis = axisList.at(1).trimmed();

        if( !rowAxis.isEmpty() && !columnAxis.isEmpty() )
        {
            if( (rowAxis.startsWith("R") || rowAxis.startsWith("L")) && (columnAxis.startsWith("A") || columnAxis.startsWith("P")) )
                label="AXIAL";
            else if( (columnAxis.startsWith("R") || columnAxis.startsWith("L")) && (rowAxis.startsWith("A") || rowAxis.startsWith("P")) )
                label="AXIAL";
            else if ( (rowAxis.startsWith("R") || rowAxis.startsWith("L")) && (columnAxis.startsWith("H") || columnAxis.startsWith("F")) )
                label="CORONAL";
            else if( (columnAxis.startsWith("R") || columnAxis.startsWith("L")) && (rowAxis.startsWith("H") || rowAxis.startsWith("F")) )
                label="CORONAL";
            else if( (rowAxis.startsWith("A") || rowAxis.startsWith("P")) && (columnAxis.startsWith("H") || columnAxis.startsWith("F")) )
                label="SAGITAL";
            else if( (columnAxis.startsWith("A") || columnAxis.startsWith("P")) && (rowAxis.startsWith("H") || rowAxis.startsWith("F")) )
                label="SAGITAL";
            else
                label="OBLIQUE";
        }
        else
        {
            label="N/A";
        }
    }

    return label;
}

QStringList Image::getSupportedModalities()
{
    // Modalitats extretes de DICOM PS 3.3 C.7.3.1.1.1
    QStringList supportedModalities;
    // Modalitats que sabem que són d'imatge i que en principi hem de poder suportar
    supportedModalities << "CR" << "CT" << "MR" << "US" << "BI" << "DD" << "ES" << "PT" << "ST" << "XA" << "RTIMAGE" << "DX" << "IO" << "GM" << "XC" << "OP" << "NM" << "OT" << "CD" << "DG" << "LS" << "RG" << "TG" << "RF" << "MG" << "PX" << "SM" << "ECG" << "IVUS";
    // Modalitats "no estàndars" però que es correspondrien amb imatges que podem suportar
    supportedModalities << "SC";

    // Aquestes modalitats en principi no són d'imatge. Les mantenim documentades per si calgués incloure-les a la llista
    // TODO Cal comprovar si són modalitats d'imatge i eliminar-les segons el cas
    // "RTSTRUCT" << "RTRECORD" << "EPS" << "RTDOSE" << "RTPLAN" << "HD" << "SMR" << "AU"

    return supportedModalities;
}

}
