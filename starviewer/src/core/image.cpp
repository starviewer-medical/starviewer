/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "image.h"

#include "logging.h"

#include <QStringList>

#define HAVE_CONFIG_H 1
#include "dcmtk/dcmdata/dcdatset.h"
#include "dcmtk/dcmdata/dcfilefo.h"
#include "dcmtk/dcmdata/dcdeftag.h"

namespace udg {

Image::Image(QObject *parent)
 : QObject(parent)
{
}

Image::~Image()
{
}

bool Image::fillInformationFromSource()
{
    DcmDataset *dicomData = new DcmDataset;

    DcmFileFormat dicomFile;
    OFCondition status = dicomFile.loadFile( qPrintable( m_path ) );
    if( status.good() )
    {
        dicomData = dicomFile.getAndRemoveDataset();

        OFCondition status;
        const char *value = NULL;
        Float64 doubleValue;
        Uint16 uint16Value;

        // instance number
        status = dicomData->findAndGetString( DCM_InstanceNumber , value );
        if( status.good() )
            this->setInstanceNumber( value );
        else
            DEBUG_LOG( QString("No s'ha pogut llegir l'instance number: error msg: %1").arg( status.text() ) );

        // pixel spacing
        status = dicomData->findAndGetString( DCM_PixelSpacing , value );
        if( status.good() )
        {
            QStringList spacing( QString(value).split( "\\" ) );
            for( int i = 0; i < spacing.size(); i++ )
                m_pixelSpacing[i] = spacing.at(i).toDouble();
        }
        else
            DEBUG_LOG( QString("No s'ha pogut llegir l'espaiat de pixel: error msg: %1").arg( status.text() ) );

        // image orientation (direction cosines)
        status = dicomData->findAndGetString( DCM_ImageOrientationPatient , value );
        if( status.good() )
        {
            QStringList orientation = QString(value).split( "\\" );
            for( int i = 0; i < orientation.size(); i++ )
                m_imageOrientation[i] = orientation.at(i).toDouble();
        }
        else
            DEBUG_LOG( QString("No s'ha pogut llegir l'orientació de pacient(direction cosines): error msg: %1").arg( status.text() ) );

        // image position
        status = dicomData->findAndGetString( DCM_ImagePositionPatient , value );
        if( status.good() )
        {
            QStringList position = QString(value).split( "\\" );
            for( int i = 0; i < position.size(); i++ )
                m_imagePosition[i] = position.at(i).toDouble();
        }
        else
            DEBUG_LOG( QString("No s'ha pogut llegir la posició de la imatge: error msg: %1").arg( status.text() ) );

        // slice thickness
        status = dicomData->findAndGetFloat64( DCM_SliceThickness , doubleValue );
        if( status.good() )
        {
            m_sliceThickness = doubleValue;
        }
        else
            DEBUG_LOG( QString("No s'ha pogut llegir l'slice thickness: error msg: %1").arg( status.text() ) );

        // slice location
        status = dicomData->findAndGetFloat64( DCM_SliceLocation , doubleValue );
        if( status.good() )
        {
            m_sliceLocation = doubleValue;
        }
        else
            DEBUG_LOG( QString("No s'ha pogut llegir l'slice location: error msg: %1").arg( status.text() ) );

        // Image Pixel Module C.6.7.3

        // samples per pixel
        status = dicomData->findAndGetUint16( DCM_SamplesPerPixel , uint16Value );
        if( status.good() )
        {
            m_samplesPerPixel = uint16Value;
        }
        else
            DEBUG_LOG( QString("No s'ha pogut llegir el samples per pixel: error msg: %1").arg( status.text() ) );

        return true;
    }
    else
    {
        DEBUG_LOG( QString( "No s'ha pogut llegir l'arxiu %1\n Error dcmtk: %2 ").arg( qPrintable( m_path ) ).arg( status.text() ) );
        return false;
    }
}

}
