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
#include "mathtools.h"

namespace udg {

OrderImagesFillerStep::OrderImagesFillerStep()
: PatientFillerStep()
{
    m_requiredLabelsList << "ImageFillerStep";
    m_priority = HighPriority;
}

OrderImagesFillerStep::~OrderImagesFillerStep()
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
                delete instanceNumberSet;
            }
            delete imagePositionSet;
        }
        delete lastOrderedImageSet;
    }
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
	// obtenim el vector normal del pla, que ens determina també a quin "stack" pertany la imatge
	double planeNormalVector[3];
	image->getImagePlaneNormal( planeNormalVector );
	// el passem a string que ens serà més fàcil de comparar,perquè així és com es guarda a l'estructura d'ordenació
	QString planeNormalString = QString("%1\\%2\\%3").arg(planeNormalVector[6]).arg(planeNormalVector[7]).arg(planeNormalVector[8]);

    QMap< double , QMap< int , Image* > * > * imagePositionSet;
    QMap< int , Image* > * instanceNumberSet;

    double distance = this->distance(image);

	// primer busquem quina és la key (normal del pla) més semblant de totes les que hi ha
	// cada key és la normal de cada pla guardat com a string. 
	// En cas que tinguem diferents normals, indicaria que tenim per exemple, diferents stacks en el mateix volum
	QStringList planeNormals = m_orderedImageSet->uniqueKeys();

	// aquest bucle serveix per trobar si la normal de la nova imatge 
	// coincideix amb alguna normal de les imatges ja processada
	QString keyPlaneNormal;
	foreach( QString normal, planeNormals )
	{
		if( normal == planeNormalString )
		{
			// la normal d'aquest pla ja existeix ( cas més típic )
			keyPlaneNormal = normal;
			break;
		}
		else // les normals són diferents, comprovar si ho són completament o no
		{
			if( normal.isEmpty() )
			{
				keyPlaneNormal = planeNormalString;
				break;
			}
			else
			{
				// tot i que siguin diferents, pot ser que siguin gairebé iguals
				// llavors cal comprovar que de fet són prou diferents
				// ja que d'avegades només hi ha petites imprecisions simplement
				double normalVector[3];
				QStringList normalSplitted = normal.split("\\");
				normalVector[0] = normalSplitted.at(0).toDouble();
				normalVector[1] = normalSplitted.at(1).toDouble();
				normalVector[2] = normalSplitted.at(2).toDouble();

				double angle = MathTools::angleInDegrees( normalVector, planeNormalVector );
				if( angle < 1.0 )
				{
					// si l'angle entre les normals
					// està dins d'un threshold, 
					// les podem considerar iguals
					// TODO definir millor aquest threshold
					keyPlaneNormal = normal;
					break;
				}
			}
		}
	}
	// ara cal inserir la imatge a la llista ordenada
	// si no hem posat cap valor, vol dir que la normal és nova i no existia fins el moment
	if( keyPlaneNormal.isEmpty() )
	{
		// assignem la clau
		keyPlaneNormal = planeNormalString;
		// ara cal inserir la nova clau
		instanceNumberSet = new QMap< int , Image* >();
        instanceNumberSet->insert( image->getInstanceNumber().toInt(), image );

        imagePositionSet = new QMap< double , QMap< int , Image* > * >();
        imagePositionSet->insert( distance, instanceNumberSet );
		m_orderedImageSet->insert( keyPlaneNormal, imagePositionSet );
	}
	else // la normal ja existia [ m_orderedImageSet->contains( keyPlaneNormal ) == true ], per tant només cal actualitzar l'estructura
	{
		imagePositionSet = m_orderedImageSet->value( keyPlaneNormal );
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
    double distance = .0;
    const double *imagePosition = image->getImagePositionPatient();
    double normal[3];
    image->getImagePlaneNormal( normal );

    for ( int i = 0; i < 3; i++ )
    {
        distance += normal[i] * imagePosition[i];
    }

    return distance;
}

}
