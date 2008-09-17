/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "orderimagesfillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"

namespace udg {

OrderImagesFillerStep::OrderImagesFillerStep()
: PatientFillerStep()
{
    m_requiredLabelsList << "ImageFillerStep";
    m_priority = HighPriority;
}

OrderImagesFillerStep::~OrderImagesFillerStep()
{
}

bool OrderImagesFillerStep::fill()
{
    bool ok = false;
    if( m_input )
    {
        QStringList requiredLabels;
        m_orderedImageSet = new QMap< QString, QMap< double , QMap< int , Image* >* >* >();
        requiredLabels << "ImageFillerStep";
        QList<Series *> seriesList = m_input->getSeriesWithLabels( requiredLabels );
        foreach( Series *series, seriesList )
        {
            this->processSeries( series );
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

bool OrderImagesFillerStep::fillIndividually() 
{

    if ( OrderImagesInternalInfo.contains( m_input->getCurrentSeries() ) )
    {
        m_orderedImageSet = OrderImagesInternalInfo.value( m_input->getCurrentSeries() );
    }
    else
    {
        DEBUG_LOG(QString("Llista nul·la en creem una de nova per la serie %1.").arg(m_input->getCurrentSeries()->getInstanceUID()));
        m_orderedImageSet = new QMap< QString, QMap< double , QMap< int , Image* >* >* >();
        OrderImagesInternalInfo.insert( m_input->getCurrentSeries() , m_orderedImageSet );
    }
    processImage( m_input->getCurrentImage() );

    m_input->addLabelToSeries("OrderImagesFillerStep", m_input->getCurrentSeries() );

    return true;
}

void OrderImagesFillerStep::postProcessing()
{
    foreach ( Series * key , OrderImagesInternalInfo.keys() )
    {
        m_orderedImageSet = OrderImagesInternalInfo.take(key);
        setOrderedImagesIntoSeries(key);
    }
}

void OrderImagesFillerStep::abort()
{
    QMap< int , Image* > * instanceNumberSet;
    QMap< double , QMap< int , Image* >* >* imagePositionSet;
    QMap< QString, QMap< double , QMap< int , Image* >* >* > *lastOrderedImageSet;
    
    foreach ( Series * key , OrderImagesInternalInfo.keys() )
    {
        lastOrderedImageSet = OrderImagesInternalInfo.take(key);
        foreach ( QString key, lastOrderedImageSet->keys() )
        {
            imagePositionSet = lastOrderedImageSet->take(key);
            foreach ( double key2 , imagePositionSet->keys() )
            {
                instanceNumberSet = imagePositionSet->take(key2);
                foreach ( int key3 , instanceNumberSet->keys() )
                {
                    instanceNumberSet->take(key3);
                }
            }
        }
    }
}

void OrderImagesFillerStep::processSeries( Series *series )
{
    QList<Image *> imageList = series->getImages();

    foreach( Image *image, imageList )
    {
        this->processImage( image );
    }

    setOrderedImagesIntoSeries( series );

    m_input->addLabelToSeries("OrderImagesFillerStep", series );
}

void OrderImagesFillerStep::processImage( Image *image )
{
    const double *imageOrientation = image->getImageOrientationPatient();
    QString imageOrientationString;
    for( int i = 0 ; i < 6 ; i++ )
    {
        imageOrientationString += QString("%1\\").arg(imageOrientation[i]);
    }

    QMap< double , QMap< int , Image* > * > * imagePositionSet;
    QMap< int , Image* > * instanceNumberSet;

    double distance = this->distance(image);

    // Inserir la image a la llista
    if ( m_orderedImageSet->contains( imageOrientationString ) )
    {
        imagePositionSet = m_orderedImageSet->value( imageOrientationString );
        if ( imagePositionSet->contains( distance ) )
        {
            // Hi ha series on les imatges comparteixen el mateix instance number.
            // Per evitar el problema es fa un insertMulti.
            imagePositionSet->value( distance )->insertMulti( image->getInstanceNumber().toInt(), image );
        }
        else
        {
            instanceNumberSet = new QMap< int , Image* >();
            instanceNumberSet->insert( image->getInstanceNumber().toInt(), image );
            imagePositionSet->insert( distance, instanceNumberSet );
        }
    }
    else
    {
        instanceNumberSet = new QMap< int , Image* >();
        instanceNumberSet->insert( image->getInstanceNumber().toInt(), image );

        imagePositionSet = new QMap< double , QMap< int , Image* > * >();
        imagePositionSet->insert( distance, instanceNumberSet );

        m_orderedImageSet->insert( imageOrientationString, imagePositionSet );
    }
}

void OrderImagesFillerStep::setOrderedImagesIntoSeries( Series *series )
{
    QList<Image *> imageSet;
    QMap< int , Image* > * instanceNumberSet;
    QMap< double , QMap< int , Image* >* >* imagePositionSet;
    QMap< double, QMap< double , QMap< int , Image* >* >* > lastOrderedImageSet;

    if ( m_orderedImageSet->count() > 1 ) // Cal ordernar les agrupacions d'imatges
    {
        foreach (QString key, m_orderedImageSet->keys() )
        {
            imagePositionSet = m_orderedImageSet->take(key);
            Image *image = (*(*imagePositionSet->begin())->begin());

            lastOrderedImageSet.insertMulti(this->distance(image),imagePositionSet);
        }
    }
    else
    {
        lastOrderedImageSet.insert(0, (*m_orderedImageSet->begin()) );
        foreach (QString key, m_orderedImageSet->keys() )
        {
            m_orderedImageSet->take(key);
        }
    }

    // Passar l'estructura a la series
    foreach ( double key, lastOrderedImageSet.keys() )
    {
        imagePositionSet = lastOrderedImageSet.take(key);
        foreach ( double key2 , imagePositionSet->keys() )
        {
            instanceNumberSet = imagePositionSet->take(key2);
            foreach ( int key3 , instanceNumberSet->keys() )
            {
                imageSet += instanceNumberSet->take(key3);
            }
        }
    }
    series->setImages( imageSet );
}

double OrderImagesFillerStep::distance( Image *image )
{
    //Càlcul de la distància (basat en l'algorisme de Jolinda Smith)
    double distance = 0;
    const double *imageOrientation = image->getImageOrientationPatient();
    const double *imagePosition = image->getImagePositionPatient();
    double normal[3];
    normal[0] = imageOrientation[1]*imageOrientation[5] - imageOrientation[2]*imageOrientation[4];
    normal[1] = imageOrientation[2]*imageOrientation[3] - imageOrientation[0]*imageOrientation[5];
    normal[2] = imageOrientation[0]*imageOrientation[4] - imageOrientation[1]*imageOrientation[3];

    for ( int i = 0; i < 3; i++ )
    {
        distance += normal[i] * imagePosition[i];
    }

    return distance;
}

}
