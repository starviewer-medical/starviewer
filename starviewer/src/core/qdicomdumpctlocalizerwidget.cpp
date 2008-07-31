/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpctlocalizerwidget.h"
#include "series.h"
#include "image.h"
#include "dicomtagreader.h"

namespace udg {

QDicomDumpCTLocalizerWidget::QDicomDumpCTLocalizerWidget( QWidget *parent )
 : QWidget(parent)
{
    setupUi( this );

}

QDicomDumpCTLocalizerWidget::~QDicomDumpCTLocalizerWidget()
{

}

void QDicomDumpCTLocalizerWidget::setCurrentDisplayedImage( Image *currentImage )
{
    initialize();
    
    if ( currentImage != NULL)
    {
        setSeriesDicomTagsValue( currentImage->getParentSeries() );//Definim els valors dels tags a nivell de sèrie
        setImageDicomTagsValue( currentImage );//Defini els valors dels tags a nivell d'imatge
    }
}

void QDicomDumpCTLocalizerWidget::initialize()
{
    m_labelReconstructionDiameterValue->setText( "-" );
    m_labelTableHeightValue->setText( "-" );
    m_labelSliceLocationValue->setText( "-" );
    m_labelExposureTimeValue->setText( "-" );
    m_labelPhilipsScanLengthValue->setText( "-" );
    m_labelImageTypeValue->setText( "-" );
    m_labelPhilipsViewConventionValue->setText( "-" );
    m_labelFilterTypeValue->setText( "-" );
    m_labelImageMatrixValue->setText( "-" );
    m_labelVoltageValue->setText( "-" );
    m_labelExposureValue->setText( "-" );
    m_labelProtocolNameValue->setText( "-" );
    m_labelPatientPositionValue->setText( "-" );
}

void QDicomDumpCTLocalizerWidget::setImageDicomTagsValue(Image *currentImage)
{ 
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( currentImage->getPath() );
    if( ok )
    {
        if (dicomReader.tagExists( DCM_ReconstructionDiameter ))
        {
            m_labelReconstructionDiameterValue->setText( QString::number( dicomReader.getAttributeByName( DCM_ReconstructionDiameter ).toDouble() , 'f' , 0 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( DCM_TableHeight ))
        {
            m_labelTableHeightValue->setText( QString::number( dicomReader.getAttributeByName( DCM_TableHeight ).toDouble() , 'f' , 0 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( DCM_ExposureTime ))
        {
            m_labelExposureTimeValue->setText( QString::number( dicomReader.getAttributeByName( DCM_ExposureTime ).toDouble() , 'f' , 2 ) +  QString( tr( " ms" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1008 )) //Tag Scan Length
        {
            m_labelPhilipsScanLengthValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1008 ).toDouble() , 'f' , 2 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1032 )) //Tag View Convention
        {
            m_labelPhilipsViewConventionValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x1032 ) );
        }

        if (dicomReader.tagExists( DCM_FilterType ))
        {
            m_labelFilterTypeValue->setText( dicomReader.getAttributeByName( DCM_FilterType ) );
        }

        m_labelImageMatrixValue->setText( QString::number( currentImage->getColumns() , 10 ) +  QString( tr( " x " ) ) + QString::number( currentImage->getRows() , 10 ) );
        
        if (dicomReader.tagExists( DCM_KVP ))
        {
            m_labelVoltageValue->setText( QString::number( dicomReader.getAttributeByName( DCM_KVP ).toDouble() , 'f' , 0 ) +  QString( tr( " KV" ) ) );
        }

        if (dicomReader.tagExists( DCM_ExposureInMicroAs ))
        {
            m_labelExposureValue->setText( QString::number( dicomReader.getAttributeByName( DCM_ExposureInMicroAs ).toDouble() , 'f' , 0 ) +  QString( tr( " mA" ) ) );
        }
    }

    if ( currentImage->getSliceLocation() != "" )
    {
        m_labelSliceLocationValue->setText( currentImage->getSliceLocation() + QString( tr( " mm" ) ) );
    }

    if ( dicomReader.tagExists( DCM_ImageType ) )
    {
        m_labelImageTypeValue->setText( dicomReader.getAttributeByName( DCM_ImageType ) );
    }

}

void QDicomDumpCTLocalizerWidget::setSeriesDicomTagsValue( Series *currentSeries )
{
    if ( currentSeries->getProtocolName() != "" )
    {
        m_labelProtocolNameValue->setText( currentSeries->getProtocolName() );
    }

    if ( currentSeries->getPatientPosition() != "" )
    {
        m_labelPatientPositionValue->setText( currentSeries->getPatientPosition() );
    }
}

}
