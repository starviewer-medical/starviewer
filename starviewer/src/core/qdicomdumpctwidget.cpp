/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpctwidget.h"
#include "series.h"
#include "image.h"
#include "dicomtagreader.h"

namespace udg {

QDicomDumpCTWidget::QDicomDumpCTWidget( QWidget *parent )
 : QWidget(parent)
{
    setupUi( this );

}

QDicomDumpCTWidget::~QDicomDumpCTWidget()
{
}

void QDicomDumpCTWidget::setCurrentDisplayedImage( Image *currentImage )
{
    initialize();
    
    if ( currentImage != NULL)
    {
        setSeriesDicomTagsValue( currentImage->getParentSeries() );//Definim els valors dels tags a nivell de sèrie
        setImageDicomTagsValue( currentImage );//Defini els valors dels tags a nivell d'imatge
    }
}

void QDicomDumpCTWidget::initialize()
{
    m_labelReconstructionDiameterValue->setText( "-" );
    m_labelTableHeightValue->setText( "-" );
    m_labelSliceLocationValue->setText( "-" );
    m_labelExposureTimeValue->setText( "-" );
    m_labelPhilipsScanLengthValue->setText( "-" );
    m_labelPhilipsRotationTimeValue->setText( "-" );
    m_labelImageTypeValue->setText( "-" );
    m_labelPhilipsViewConventionValue->setText( "-" );
    m_labelFilterTypeValue->setText( "-" );
    m_labelPhilipsCollimationValue->setText( "-" );
    m_labelPhilipsCycleTimeValue->setText( "-" );
    m_labelTiltValue->setText( "-" );
    m_labelPhilipsTableIncrementValue->setText( "-" );
    m_labelScanArcValue->setText( "-" );
    m_labelImageMatrixValue->setText( "-" );
    m_labelVoltageValue->setText( "-" );
    m_labelExposureValue->setText( "-" );
    m_labelSliceThicknessValue->setText( "-" );
    m_labelProtocolNameValue->setText( "-" );
    m_labelPatientPositionValue->setText( "-" );
}

void QDicomDumpCTWidget::setImageDicomTagsValue( Image *currentImage )
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

        if (dicomReader.tagExists( 0x01f1, 0x1027 )) //Tag Rotation Time
        {
            m_labelPhilipsRotationTimeValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1027 ).toDouble() , 'f' , 2 ) +  QString( tr( " s" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1032 )) //Tag View Convention
        {
            m_labelPhilipsViewConventionValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x1032 ) );
        }

        if (dicomReader.tagExists( DCM_FilterType ))
        {
            m_labelFilterTypeValue->setText( dicomReader.getAttributeByName( DCM_FilterType ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x104b )) //Tag Collimation
        {
            m_labelPhilipsCollimationValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x104b ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1033 )) //Tag CycleTime
        {
            m_labelPhilipsCycleTimeValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1033 ).toDouble() , 'f' , 2 ) +  QString( tr( " s" ) ) );
        }

        if (dicomReader.tagExists( DCM_GantryDetectorTilt ))
        {
            m_labelTiltValue->setText( dicomReader.getAttributeByName( DCM_GantryDetectorTilt ) +  QString( tr( " degrees" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1028 )) //Tag Table Increment
        {
            m_labelPhilipsTableIncrementValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1028 ).toDouble() , 'f' , 2 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( DCM_ScanArc ))
        {
            m_labelScanArcValue->setText(dicomReader.getAttributeByName( DCM_ScanArc ) +  QString( " degrees" ) );
        }

        if (dicomReader.tagExists( DCM_KVP ))
        {
            m_labelVoltageValue->setText( QString::number( dicomReader.getAttributeByName( DCM_KVP ).toDouble() , 'f' , 0 ) +  QString( tr( " KV" ) ) );
        }

        if (dicomReader.tagExists( DCM_ExposureInMicroAs ))
        {
            m_labelExposureValue->setText( QString::number( dicomReader.getAttributeByName( DCM_ExposureInMicroAs ).toDouble() , 'f' , 0 ) +  QString( tr( " mA" ) ) );
        }

        m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' ,2 )+  QString( tr( " mm" ) ) );
        m_labelImageMatrixValue->setText( QString::number( currentImage->getColumns() , 10 ) +  QString( tr( " x " ) ) + QString::number( currentImage->getRows() , 10 ) );
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

void QDicomDumpCTWidget::setSeriesDicomTagsValue( Series *currentSeries )
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
