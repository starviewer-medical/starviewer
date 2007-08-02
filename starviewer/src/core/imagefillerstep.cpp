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
            DEBUG_LOG("Afegim " + file);
            Image *image = new Image;
            image->setPath( file );

            processImage( image );

            series->addImage( image );
        }
        m_input->addLabelToSeries("ImageFillerStep", series->getInstanceUID() );
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
        image->setPatientOrientation( dicomReader.getAttributeByName( DCM_PatientOrientation ) );

        QString value = dicomReader.getAttributeByName( DCM_ContentDate );
        if( !value.isEmpty() )
            image->setContentDate(value);

        value = dicomReader.getAttributeByName( DCM_ContentTime );
        if( !value.isEmpty() )
            image->setContentTime(value);

        image->setImagesInAcquisition( dicomReader.getAttributeByName( DCM_ImagesInAcquisition ).toInt() );
        image->setComments( dicomReader.getAttributeByName( DCM_ImageComments ) );

        value = dicomReader.getAttributeByName( DCM_ImageOrientationPatient );

        QStringList list = value.split( "\\" );
        if( list.size() == 6 )
        {
            double orientation[6];
            for( int i = 0; i < 6; i++ )
            {
                orientation[ i ] = list.at( i ).toDouble();
            }

            image->setImageOrientation( orientation );
        }

        value = dicomReader.getAttributeByName( DCM_ImagePosition );
        list = value.split("\\");
        if( list.size() == 3 )
        {
            double position[3] = { list.at(0).toDouble(), list.at(1).toDouble(), list.at(2).toDouble() };
            image->setImagePosition( position );
        }

        image->setSamplesPerPixel( dicomReader.getAttributeByName( DCM_SamplesPerPixel ).toInt() );
        image->setPhotometricInterpretation( dicomReader.getAttributeByName( DCM_PhotometricInterpretation ).toInt() );
        image->setRows( dicomReader.getAttributeByName( DCM_Rows ).toInt() );
        image->setColumns( dicomReader.getAttributeByName( DCM_Columns ).toInt() );
    }
    else
    {
        DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + image->getPath() );
    }
}

}
