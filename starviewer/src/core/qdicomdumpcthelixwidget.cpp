/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpcthelixwidget.h"
#include "series.h"
#include "image.h"
#include "dicomtagreader.h"

namespace udg {

QDicomDumpCTHelixWidget::QDicomDumpCTHelixWidget( QWidget *parent )
 : QWidget(parent)
{
    setupUi( this );

}

QDicomDumpCTHelixWidget::~QDicomDumpCTHelixWidget()
{
}

void QDicomDumpCTHelixWidget::setCurrentDisplayedImage( Image *currentImage )
{
    initialize();
    
    if ( currentImage != NULL)
    {
        setSeriesDicomTagsValue( currentImage->getParentSeries() );//Definim els valors dels tags a nivell de sèrie
        setImageDicomTagsValue( currentImage );//Defini els valors dels tags a nivell d'imatge*
    }
}

void QDicomDumpCTHelixWidget::initialize()
{
    m_labelReconstructionDiameterValue->setText( "-" );
    m_labelTableHeightValue->setText( "-" );
    m_labelSliceLocationValue->setText( "-" );
    m_labelPhilipsScanTimeValue->setText( "-" );
    m_labelPhilipsScanLengthValue->setText( "-" );
    m_labelPhilipsRotationTimeValue->setText( "-" );
    m_labelImageTypeValue->setText( "-" );
    m_labelPhilipsViewConventionValue->setText( "-" );
    m_labelFilterTypeValue->setText( "-" );
    m_labelPhilipsCollimationValue->setText( "-" );
    m_labelImageMatrixValue->setText( "-" );
    m_labelVoltageValue->setText( "-" );
    m_labelExposureValue->setText( "-" );
    m_labelSliceThicknessValue->setText( "-" );
    m_labelPhilipsPitchValue->setText( "-" );
    m_labelProtocolNameValue->setText( "-" );
    m_labelPatientPositionValue->setText( "-" );
    m_labelPhilipsTableSpeedValue->setText( "-" );
    m_labelSpacingBetweenSlicesValue->setText( "-" );
}

void QDicomDumpCTHelixWidget::setImageDicomTagsValue( Image *currentImage )
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

        if (dicomReader.tagExists( 0x00e1, 0x1050)) //Tag Scan time
        {
            m_labelPhilipsScanTimeValue->setText( QString::number( dicomReader.getAttributeByTag( 0x00e1, 0x1050).toDouble() , 'f' , 2 ) +  QString( tr( " s" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1008 )) //Tag Scan Length
        {
            m_labelPhilipsScanLengthValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1008 ).toDouble() , 'f' , 2 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1026)) //Tag Pitch
        {
            m_labelPhilipsPitchValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x1026) +  QString( tr( " " ) ) );
        }

        if (dicomReader.tagExists( DCM_SpacingBetweenSlices ))
        {
            m_labelSpacingBetweenSlicesValue->setText( QString::number( dicomReader.getAttributeByName( DCM_SpacingBetweenSlices ).toDouble() , 'f' , 1 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( 0x01f1, 0x1007 )) //Tag table speed
        {
            m_labelPhilipsTableSpeedValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1007 ).toDouble() , 'f' , 0 ) +  QString( tr( " mm/s" ) ) );
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

        if (dicomReader.tagExists( DCM_KVP ))
        {
            m_labelVoltageValue->setText( QString::number( dicomReader.getAttributeByName( DCM_KVP ).toDouble() , 'f' , 0 ) +  QString( tr( " KV" ) ) );
        }

        if (dicomReader.tagExists( DCM_ExposureInMicroAs ))
        {
            m_labelExposureValue->setText( QString::number( dicomReader.getAttributeByName( DCM_ExposureInMicroAs ).toDouble() , 'f' , 0 ) +  QString( tr( " mA" ) ) );
        }

        m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' , 2 )+  QString( tr( " mm" ) ) );
        m_labelImageMatrixValue->setText( QString::number(currentImage->getColumns() , 10 ) +  QString( tr( " x " ) ) + QString::number( currentImage->getRows() , 10 ) );
    }

    if ( currentImage->getSliceLocation() != "" )
    {
        m_labelSliceLocationValue->setText( currentImage->getSliceLocation() + QString( tr( " mm" ) ) );
    }

    if ( currentImage->getImageType() != "" )
    {
        m_labelImageTypeValue->setText( currentImage->getImageType() );
    }
}

void QDicomDumpCTHelixWidget::setSeriesDicomTagsValue( Series *currentSeries )
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
