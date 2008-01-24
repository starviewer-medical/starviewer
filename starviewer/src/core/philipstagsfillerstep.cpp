/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "philipstagsfillerstep.h"
#include "logging.h"
#include "patientfillerinput.h"
#include "dicomtagreader.h"
#include "series.h"
#include "image.h"
#include <QStringList>

namespace udg {

PhilipsTagsFillerStep::PhilipsTagsFillerStep()
: PatientFillerStep()
{
    m_requiredLabelsList << "ImageFillerStep";
}

PhilipsTagsFillerStep::~PhilipsTagsFillerStep()
{
}

bool PhilipsTagsFillerStep::fill()
{
    bool ok = false;

    if( m_input )
    {
        QStringList requiredLabels;
        requiredLabels << "ImageFillerStep";
        QList<Series *> seriesList = m_input->getSeriesWithLabels( requiredLabels );

        foreach( Series *series, seriesList )
        {
            //si l'aparell amb que s'han generat les sèries és philips
            if ( series->getManufacturer().contains( "Philips" , Qt::CaseInsensitive ) ) 
                this->processSeries( series );
        }
    }
    else
    {
        DEBUG_LOG("No tenim input!");
    }

    return ok;
}

void PhilipsTagsFillerStep::processSeries( Series *series )
{
    QStringList list = series->getImagesPathList();
    DICOMTagReader dicomReader( list[0] );
    QList<Image *> imageList = series->getImages();

    if (isImageSeries( series ))
    {
        if ( imageList.count() > 0 )
        {
            DICOMTagReader dicomReader;
            bool ok = dicomReader.setFile( imageList[0]->getPath() );

            if ( ok )
            {
                if ( dicomReader.tagExists( 0x2001, 0x1020 ) ) series->setPhilipsScanningTechnique( dicomReader.getAttributeByTag( 0x2001, 0x1020 ) );
                if ( dicomReader.tagExists( 0x2001, 0x1060 ) ) series->setPhilipsNumberOfStacks( dicomReader.getAttributeByTag( 0x2001, 0x1060 ) );
                foreach( Image *image , imageList )
                {
                    processImage( image );
                }
            }
        }

        m_input->addLabelToSeries("PhilipsTagsFillerStep", series );
    }
}

void PhilipsTagsFillerStep::processImage( Image *image )
{
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( image->getPath() );
    
    if( ok )
    {
        if (dicomReader.tagExists( 0x2001, 0x1082 )) //Tag Turbo-Factor
        {  
            image->setPhilipsTurboFactor( dicomReader.getAttributeByTag( 0x2001, 0x1082 ) );
        }
        
        if (dicomReader.tagExists( 0x2001, 0x1013 )) //Tag EPI-Factor
        {  
            image->setPhilipsEPIFactor( dicomReader.getAttributeByTag( 0x2001, 0x1013 ) );
        }

        if (dicomReader.tagExists( 0x2001, 0x1003 )) //Tag B-Factor
        {  
            image->setPhilipsBFactor( QString::number( dicomReader.getAttributeByTag( 0x2001, 0x1003 ).toDouble() , 'f' , 1 ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1008 )) //TAg Scan Length
        {  
            image->setPhilipsScanLength( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1008 ).toDouble() , 'f' , 2 ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1032 )) //Tag View Convention 
        {  
            image->setPhilipsViewConvention( dicomReader.getAttributeByTag( 0x01f1, 0x1032 ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1027 )) //Tag Rototation Time
        {  
            image->setPhilipsRotationTime( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1027 ).toDouble() , 'f' , 2 ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1007 )) //Tag table spped
        {  
            image->setPhilipsTableSpeed( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1007 ).toDouble() , 'f' , 0 ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x104b )) //Tag Collimation
        {  
            image->setPhilipsCollimation( dicomReader.getAttributeByTag( 0x01f1, 0x104b ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1033 )) //Tag CycleTime
        {  
            image->setPhilipsCycleTime( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1033 ).toDouble() , 'f' , 2 ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1028 )) //Tag Table Increment
        {  
            image->setPhilipsTableIncrement( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1028 ).toDouble() , 'f' , 2 ) );
        }

        if (dicomReader.tagExists( 0x2001, 0x100b )) //Tag Image Position
        {  
            image->setPhilipsImagePosition( dicomReader.getAttributeByTag( 0x2001, 0x100b ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1026)) //Tag Pitch
        {
            image->setPhilipsPitch( dicomReader.getAttributeByTag( 0x01f1, 0x1026));
        }

        if (dicomReader.tagExists( 0x00e1, 0x1050)) //Tag Scan time
        {
            image->setPhilipsScanTime( QString::number( dicomReader.getAttributeByTag( 0x00e1, 0x1050).toDouble() , 'f' , 2 ) );
        }


    }
}

}
