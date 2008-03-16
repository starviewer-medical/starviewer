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
#include <QPixmap>

#include <vtkMath.h> // pel ::Cross()

// fem servir dcmtk per l'escalat de les imatges dicom
#include "dcmtk/dcmimgle/dcmimage.h"
#include "dcmtk/ofstd/ofbmanip.h"

namespace udg {

Image::Image(QObject *parent)
 : QObject(parent), m_sliceThickness(1.0), m_samplesPerPixel(1), m_photometricInterpretation("MONOCHROME2"), m_rows(0), m_columns(0), m_bitsAllocated(16), m_bitsStored(16), m_pixelRepresentation(0), m_rescaleSlope(1), m_rescaleIntercept(0), m_numberOfFrames(1), m_parentSeries(NULL), m_KiloVoltagePeak(0), m_microAmpersSecond(0), m_milliAmpersSecond(0), m_CTLocalizer(false)
{
    m_pixelSpacing[0] = 1.;
    m_pixelSpacing[0] = 2.;
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
    memcpy( m_imagePositionPatient, position, 3*sizeof(double) );
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

void Image::setKiloVoltagePeak( double kvp )
{
 m_KiloVoltagePeak = kvp;
}

double Image::getKiloVoltagePeak()
{
 return m_KiloVoltagePeak;
}

void Image::setMicroAmpersSecond( double uas )
{
     m_microAmpersSecond = uas;
}

double Image::getMicroAmpersSecond()
{
     return m_microAmpersSecond;
}

void Image::setMilliAmpersSecond( double mas )
{
    m_milliAmpersSecond = mas;
}

double Image::getMilliAmpersSecond()
{
    return m_milliAmpersSecond;
}

void Image::setRepetitionTime( QString repetitionTime )
{
    m_repetitionTime = repetitionTime;
}

QString Image::getRepetitionTime()
{
    return m_repetitionTime;
}

void Image::setEchoTime( QString echoTime )
{
    m_echoTime = echoTime;
}

QString Image::getEchoTime()
{
    return m_echoTime;
}

void Image::setInversionTime( QString inversionTime )
{
    m_inversionTime = inversionTime;
}

QString Image::getInversionTime()
{
    return m_inversionTime;
}

void Image::setSpacingBetweenSlices( QString spacingBetweenSlices )
{
    m_spacingBetweenSlices = spacingBetweenSlices;
}

QString Image::getSpacingBetweenSlices()
{
    return m_spacingBetweenSlices;
}

void Image::setFlipAngle( QString flipAngle )
{
    m_flipAngle = flipAngle;
}

QString Image::getFlipAngle()
{
    return m_flipAngle;
}

void Image::setPhilipsTurboFactor( QString philipsTurboFactor )
{
    m_philipsTurboFactor = philipsTurboFactor ;
}

QString Image::getPhilipsTurboFactor()
{
    return m_philipsTurboFactor;
}

void Image::setPhilipsEPIFactor( QString philipsEPIFactor )
{
    m_philipsEPIFactor = philipsEPIFactor;
}

QString Image::getPhilipsEPIFactor()
{
    return m_philipsEPIFactor;
}

void Image::setPhilipsSpacialPlane( QString philipsSpacialPlane )
{
    m_philipsSpacialPlane = philipsSpacialPlane;
}

QString Image::getPhilipsSpacialPlane()
{
    return m_philipsSpacialPlane;
}

void Image::setNumberOfAverages( QString numberOfAverages )
{
    m_numberOfAverages = numberOfAverages;
}

QString Image::getNumberOfAverages()
{
    return m_numberOfAverages;
}

void Image::setPercentPhaseFieldOfView( QString PercentPhaseFieldOfView )
{
    m_percentPhaseFieldOfView = PercentPhaseFieldOfView;
}

QString Image::getPercentPhaseFieldOfView()
{
    return m_percentPhaseFieldOfView;
}

void Image::setReceiveCoilName( QString ReceiveCoilName )
{
    m_receiveCoilName = ReceiveCoilName;
}

QString Image::getReceiveCoilName()
{
    return m_receiveCoilName;
}

void Image::setPhilipsBFactor( QString philipsBFactor )
{
    m_philipsBFactor = philipsBFactor;
}

QString Image::getPhilipsBFactor()
{
    return m_philipsBFactor;
}

void Image::setReconstructionDiameter( QString reconstructionDiameter )
{
    m_reconstructionDiameter = reconstructionDiameter;
}

QString Image::getReconstructionDiameter()
{
    return m_reconstructionDiameter;
}

void Image::setExposureTime( QString exposureTime )
{
    m_exposureTime = exposureTime;
}

QString Image::getExposureTime()
{
    return m_exposureTime;
}

void Image::setPhilipsScanLength( QString philipsScanLength )
{
    m_philipsScanLength = philipsScanLength;
}

QString Image::getPhilipsScanLength()
{
    return m_philipsScanLength;
}

void Image::setTableHeight( QString tableHeight )
{
    m_tableHeight = tableHeight;
}

QString Image::getTableHeight()
{
    return m_tableHeight;
}

void Image::setSliceLocation( QString sliceLocation )
{
    m_sliceLocation = sliceLocation;
}

QString Image::getSliceLocation()
{
    return m_sliceLocation;
}

void Image::setPhilipsViewConvention( QString philipsViewConvention )
{
    m_philipsViewConvention = philipsViewConvention;
}

QString Image::getPhilipsViewConvention()
{
    return m_philipsViewConvention;
}

void Image::setFilterType( QString filterType )
{
    m_filterType = filterType;
}

QString Image::getFilterType()
{
    return m_filterType;
}

void Image::setImageType( QString imageType )
{
    m_imageType = imageType;
}

QString Image::getImageType()
{
    return m_imageType;
}

void Image::setPhilipsTableSpeed( QString philipsTableSpeed )
{
    m_philipsTableSpeed = philipsTableSpeed;
}

QString Image::getPhilipsTableSpeed()
{
    return m_philipsTableSpeed;
}

void Image::setPhilipsCollimation( QString philipsCollimation )
{
    m_philipsCollimation = philipsCollimation;
}

QString  Image::getPhilipsCollimation()
{
    return m_philipsCollimation;
}

void Image::setScanArc( QString scanArc )
{
    m_scanArc = scanArc;
}

QString Image::getScanArc()
{
    return m_scanArc;
}

void Image::setTilt( QString tilt )
{
    m_tilt = tilt;
}

QString Image::getTilt()
{
    return m_tilt;
}

void Image::setPhilipsCycleTime( QString philipsCycleTime )
{
    m_philipsCycleTime = philipsCycleTime;
}

QString Image::getPhilipsCycleTime()
{
    return m_philipsCycleTime;
}

void Image::setPhilipsPitch( QString philipsPitch )
{
    m_philipsPitch = philipsPitch;
}

QString Image::getPhilipsPitch()
{
    return m_philipsPitch;
}

void Image::setPhilipsRotationTime( QString philipsRotationTime )
{
    m_philipsRotationTime = philipsRotationTime;
}

QString Image::getPhilipsRotationTime()
{
    return m_philipsRotationTime;
}

void Image::setPhilipsTableIncrement( QString philipsTableIncrement )
{
    m_philipsTableIncrement = philipsTableIncrement;
}

QString Image::getPhilipsTableIncrement()
{
    return m_philipsTableIncrement;
}

void Image::setPhilipsScanTime( QString philipsScanTime )
{
    m_philipsScanTime = philipsScanTime;
}

QString Image::getPhilipsScanTime()
{
    return m_philipsScanTime;
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

void Image::setNumberOfFrames( int frames )
{
    m_numberOfFrames = frames;
}

int Image::getNumberOfFrames() const
{
    return m_numberOfFrames;
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
        DEBUG_LOG("Index out of range");
        return QString();
    }
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

QPixmap Image::getThumbnail(int resolution)
{
    return QPixmap::fromImage( createThumbnail(resolution) );
}

QImage Image::createThumbnail(int resolution)
{
    if(! m_thumbnail.isNull() )
    {
        return m_thumbnail;
    }

    QImage thumbnail;
    bool ok = false;

    //carreguem el fitxer dicom a escalar
    DicomImage *dicomImage = new DicomImage( qPrintable( getPath() ) );

    if( dicomImage == NULL )
    {
        ok = false;
        DEBUG_LOG("Memòria insuficient");
    }
    else if( dicomImage->getStatus() == EIS_Normal )
    {
        dicomImage->hideAllOverlays();
        dicomImage->setMinMaxWindow(1);
        //escalem l'imatge
        DicomImage *scaledImage;
        //Escalem pel cantó més gran
        unsigned long width, height;
        if( dicomImage->getWidth() < dicomImage->getHeight() )
        {
            width = 0;
            height = resolution;
        }
        else
        {
            width = resolution;
            height = 0;
        }
        scaledImage = dicomImage->createScaledImage( width,height, 1, 1 );
        if( scaledImage == NULL )
        {
            ok = false;
            DEBUG_LOG("La imatge escalada s'ha retornat com a nul");
        }
        else if( scaledImage->getStatus() == EIS_Normal )
        {
            // el següent codi crea una imatge pgm a memòria i carreguem aquest buffer directament al pixmap
            // obtingut de http://forum.dcmtk.org/viewtopic.php?t=120&highlight=qpixmap
            // get image extension
            const int width = (int)(scaledImage->getWidth());
            const int height = (int)(scaledImage->getHeight());
            char header[32];
            // create PGM header
            sprintf(header, "P5\n%i %i\n255\n", width, height);
            const int offset = strlen(header);
            const unsigned int length = width * height + offset;
            // create output buffer for DicomImage class
            Uint8 *buffer = new Uint8[length];
            if (buffer != NULL)
            {
                // copy PGM header to buffer
                OFBitmanipTemplate<Uint8>::copyMem((const Uint8 *)header, buffer, offset);
                if( scaledImage->getOutputData((void *)(buffer + offset), length, 8))
                {
                    if( thumbnail.loadFromData((const unsigned char *)buffer, length, "PGM") )
                    {
                        ok = true;
                    }
                    else
                        DEBUG_LOG(">>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>>Ha fallat :(");

                }
                // delete temporary pixel buffer
                delete[] buffer;
            }
        }
        else
        {
            ok = false;
            DEBUG_LOG( QString( "La imatge escalada té errors. Error: %1 ").arg( DicomImage::getString( scaledImage->getStatus() ) ) );
        }
    }
    else
    {
        ok = false;
        DEBUG_LOG( QString( "Error en carregar la DicomImage. Error: %1 ").arg( DicomImage::getString( dicomImage->getStatus() ) ) );
    }

    if( !ok ) // no hem pogut generar el thumbnail, creem un de buit
    {
        thumbnail = QImage(resolution, resolution, QImage::Format_RGB32);
        thumbnail.fill(Qt::black);

        QPainter painter( &thumbnail );
        painter.setPen(Qt::white);
        painter.drawText(0, 0, resolution, resolution, Qt::AlignCenter | Qt::TextWordWrap, tr("No Image Available"));
    }
    m_thumbnail = thumbnail;
    return thumbnail;
}

}
