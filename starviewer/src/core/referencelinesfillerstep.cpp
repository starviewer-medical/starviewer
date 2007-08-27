/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "referencelinesfillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "patient.h"
#include "study.h"
#include "series.h"
#include "image.h"

namespace udg {

ReferenceLinesFillerStep::ReferenceLinesFillerStep()
: PatientFillerStep()
{
    m_requiredLabelsList << "DICOMFileClassifierFillerStep";
    m_requiredLabelsList << "ImageFillerStep";
}

ReferenceLinesFillerStep::~ReferenceLinesFillerStep()
{
}

bool ReferenceLinesFillerStep::fill()
{
    bool ok = false;

    if( m_input )
    {
        QStringList requiredLabels;
        requiredLabels << "ImageFillerStep";
        QList<Series *> seriesList = m_input->getSeriesWithLabels( requiredLabels );
        foreach( Series *series, seriesList )
        {
            this->processSeries( series );
        }
        m_cacheImageSet.clear();
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}


void ReferenceLinesFillerStep::processSeries( Series *series )
{
    QList<Image *> imageList = series->getImages();
    foreach( Image *image, imageList )
    {
        this->processImage( image );
    }
    m_input->addLabelToSeries("ReferenceLinesFillerStep", series );
}

void ReferenceLinesFillerStep::processImage( Image *image )
{
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( image->getPath() );
    if( ok )
    {

        // Tractament del ReferencedImageSequence
        Study * study = image->getParentSeries()->getParentStudy();
        Image * referencedImage = 0;
        QStringList referecedUIDList = dicomReader.getSequenceAttributeByName(DCM_ReferencedImageSequence, DCM_ReferencedSOPInstanceUID);

        foreach( QString referencedUID, referecedUIDList )
        {
            //Buscar la imatge a la que fa referència:
            // 1- Buscar-la dins les imatges ja cercades
            // 2- Cercar-la dins l'estudi i afegir-la a la cache d'imatges cercades.


            if ( m_cacheImageSet.contains(referencedUID) )
            {
                referencedImage = m_cacheImageSet[referencedUID];
            }
            else
            {
                referencedImage = findImageIn( study , referencedUID );

                if ( referencedImage != 0 )
                    m_cacheImageSet.insert( referencedUID, referencedImage );
            }

            //Afegir la imatge obtinguda a la llista de l'actual.
            if (referencedImage != 0 )
            {
                image->addReferencedImage( referencedImage );
            }
            else
            {
                DEBUG_LOG("No s'ha trobat la imatge: " + referencedUID + " dins l'estudi: " + study->getInstanceUID());
            }
        }

        // Identificar els localizers en cas de ser CT
        if ( image->getParentSeries()->getModality() == "CT" )
        {
            QString value = dicomReader.getAttributeByName( DCM_ImageType );
            QStringList valueList = value.split( "\\" );

            if ( valueList.at(2) == "LOCALIZER" )
            {
                image->setCTLocalizer( true );
                DEBUG_LOG( " La imatge " + image->getSOPInstanceUID() + " de la serie " + image->getParentSeries()->getInstanceUID() + " és un localitzador " );
            }
        }
    }
    else
    {
        DEBUG_LOG("No s'ha pogut obrir amb el tagReader l'arxiu: " + image->getPath() );
    }
}

Image *ReferenceLinesFillerStep::findImageIn( Study *study, QString SOPInstanceUID )
{
    QList<Series *> seriesList = study->getSeries();
    Image * image = 0;
    int i = 0;
    bool found = false;
    while( i < seriesList.size() && !found )
    {
        image = seriesList.at(i)->getImage( SOPInstanceUID );
        if( image != 0 )
            found = true;
        else
            i++;
    }
    return image;
}

}
