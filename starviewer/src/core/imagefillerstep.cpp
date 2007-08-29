/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "imagefillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"

#include <cmath> // pel fabs

namespace udg {

ImageFillerStep::ImageFillerStep()
 : PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
}

ImageFillerStep::~ImageFillerStep()
{
}

bool ImageFillerStep::fill()
{
    bool ok = false;
    // processarem cadascun dels pacients que hi hagi en l'input i per cadascun totes les sèries que siguin de tipus imatge
    if( m_input )
    {
        unsigned int i = 0;
        while( i < m_input->getNumberOfPatients() )
        {
            Patient *patient = m_input->getPatient( i );
            this->processPatient( patient );
            i++;
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

void ImageFillerStep::processPatient( Patient *patient )
{
    QList<Study *> studyList = patient->getStudies();
    foreach( Study *study, studyList )
    {
        QList<Series *> seriesList = study->getSeries();
        foreach( Series *series, seriesList )
        {
            this->processSeries( series );
        }
    }
}

void ImageFillerStep::processSeries( Series *series )
{
    // Podrem tenir o bé Images, o bé KINs o bé PresentationStates
    if( isImageSeries(series) )
    {
        foreach (QString file, series->getFilesPathList())
        {
            Image *image = new Image;
            image->setPath( file );

            processImage( image );

            series->addImage( image );
        }
        // li afegim el thumbnail a la sèrie partir de la imatge central
        Image *imageToScale = series->getImages()[ series->getImages().size()/2 ];
        if( imageToScale )
        {
            series->setThumbnail( imageToScale->getThumbnail() );
        }
        else
        {
            DEBUG_LOG("La imatge retornada és NUL");
        }
        m_input->addLabelToSeries("ImageFillerStep", series );
    }
    else
    {
        DEBUG_LOG("La serie amb uid " + series->getInstanceUID() + " no es processa perquè no és una sèrie d'Imatges. És de modalitat: " + series->getModality() );
    }
}

void ImageFillerStep::processImage( Image *image )
{
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( image->getPath() );
    if( ok )
    {
        image->setSOPInstanceUID( dicomReader.getAttributeByName( DCM_SOPInstanceUID ) );
        image->setInstanceNumber( dicomReader.getAttributeByName( DCM_InstanceNumber ) );

        QString value = dicomReader.getAttributeByName( DCM_ContentDate );
        if( !value.isEmpty() )
            image->setContentDate(value);

        value = dicomReader.getAttributeByName( DCM_ContentTime );
        if( !value.isEmpty() )
            image->setContentTime(value);

        value = dicomReader.getAttributeByName( DCM_PixelSpacing );
        QStringList list = value.split( "\\" );
        if( list.size() == 2 )
            image->setPixelSpacing( list.at(0).toDouble(), list.at(1).toDouble() );
        else
            DEBUG_LOG("Error a l'obtenir el pixel spacing")

        value = dicomReader.getAttributeByName( DCM_SliceThickness );
        if( !value.isEmpty() )
            image->setSliceThickness( value.toDouble() );

        value = dicomReader.getAttributeByName( DCM_ImageOrientationPatient );
        list = value.split( "\\" );
        if( list.size() == 6 )
        {
            double orientation[6];
            for( int i = 0; i < 6; i++ )
            {
                orientation[ i ] = list.at( i ).toDouble();
            }
            image->setImageOrientationPatient( orientation );
        }
        else
            DEBUG_LOG("Error inesperat llegint ImageOrientationPatient. Els valors trobats no són 6!");

        // cerquem l'string amb la orientació del pacient
        value = dicomReader.getAttributeByName( DCM_PatientOrientation );
        if( !value.isEmpty() )
            image->setPatientOrientation( value );
        else // si no tenim aquest valor, el calculem a partir dels direction cosines
        {
            // I ara ens disposem a crear l'string amb l'orientació del pacient
            double *orientation = (double *)image->getImageOrientationPatient();
            double dirCosinesX[3], dirCosinesY[3], dirCosinesZ[3];
            for( int i = 0; i < 3; i++ )
            {
                dirCosinesX[i] = orientation[i];
                dirCosinesY[i] = orientation[3+i];
                dirCosinesZ[i] = orientation[6+i];
            }
            QString patientOrientationString;
            // \TODO potser el delimitador hauria de ser '\' en comptes de ','
            patientOrientationString = this->mapDirectionCosinesToOrientationString( dirCosinesX );
            patientOrientationString += ",";
            patientOrientationString += this->mapDirectionCosinesToOrientationString( dirCosinesY );
            patientOrientationString += ",";
            patientOrientationString += this->mapDirectionCosinesToOrientationString( dirCosinesZ );
            image->setPatientOrientation( patientOrientationString );
        }

        value = dicomReader.getAttributeByName( DCM_ImagePositionPatient );
        list = value.split("\\");
        if( list.size() == 3 )
        {
            double position[3] = { list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble() };
            image->setImagePositionPatient( position );
        }

        image->setSamplesPerPixel( dicomReader.getAttributeByName( DCM_SamplesPerPixel ).toInt() );
        image->setPhotometricInterpretation( dicomReader.getAttributeByName( DCM_PhotometricInterpretation ).toInt() );
        image->setRows( dicomReader.getAttributeByName( DCM_Rows ).toInt() );
        image->setColumns( dicomReader.getAttributeByName( DCM_Columns ).toInt() );
        image->setBitsAllocated( dicomReader.getAttributeByName( DCM_BitsAllocated ).toInt() );
        image->setBitsStored( dicomReader.getAttributeByName( DCM_BitsStored ).toInt() );
    }
    else
    {
        DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + image->getPath() );
    }
}

QString ImageFillerStep::mapDirectionCosinesToOrientationString( double vector[3] )
{
    char *orientation = new char[4];
    char *optr = orientation;
    *optr='\0';

    char orientationX = vector[0] < 0 ? 'R' : 'L';
    char orientationY = vector[1] < 0 ? 'A' : 'P';
    char orientationZ = vector[2] < 0 ? 'I' : 'S';

    double absX = fabs( vector[0] );
    double absY = fabs( vector[1] );
    double absZ = fabs( vector[2] );

    int i;
    for ( i = 0; i < 3; ++i )
    {
        if ( absX > .0001 && absX > absY && absX > absZ )
        {
            *optr++= orientationX;
            absX = 0;
        }
        else if ( absY > .0001 && absY > absX && absY > absZ )
        {
            *optr++= orientationY;
            absY = 0;
        }
        else if ( absZ > .0001 && absZ > absX && absZ > absY )
        {
            *optr++= orientationZ;
            absZ = 0;
        }
        else break;
        *optr='\0';
    }
    return QString( orientation );
}

}
