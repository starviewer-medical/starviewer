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
#include "dicomdictionary.h"

namespace udg {

const QString NotAvailableValue( QObject::tr("N/A") );

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
        if (dicomReader.tagExists( DICOMReconstructionDiameter ))
        {
            m_labelReconstructionDiameterValue->setText( QString::number( dicomReader.getAttributeByName( DICOMReconstructionDiameter ).toDouble() , 'f' , 0 ) +  QString( tr( " mm" ) ) );
        }
        else
            m_labelReconstructionDiameterValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMTableHeight ))
        {
            m_labelTableHeightValue->setText( QString::number( dicomReader.getAttributeByName( DICOMTableHeight ).toDouble() , 'f' , 0 ) +  QString( tr( " mm" ) ) );
        }
        else
            m_labelTableHeightValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x00e1, 0x1050)) //Tag Scan time
        {
            m_labelPhilipsScanTimeValue->setText( QString::number( dicomReader.getAttributeByTag( 0x00e1, 0x1050).toDouble() , 'f' , 2 ) +  QString( tr( " s" ) ) );
        }
        else
            m_labelPhilipsScanTimeValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x01f1, 0x1008 )) //Tag Scan Length
        {
            m_labelPhilipsScanLengthValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1008 ).toDouble() , 'f' , 2 ) +  QString( tr( " mm" ) ) );
        }
        else
            m_labelPhilipsScanLengthValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x01f1, 0x1026)) //Tag Pitch
        {
            m_labelPhilipsPitchValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x1026) +  QString( tr( " " ) ) );
        }
        else
            m_labelPhilipsPitchValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMSpacingBetweenSlices ))
        {
            m_labelSpacingBetweenSlicesValue->setText( QString::number( dicomReader.getAttributeByName( DICOMSpacingBetweenSlices ).toDouble() , 'f' , 1 ) +  QString( tr( " mm" ) ) );
        }
        else
            m_labelSpacingBetweenSlicesValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x01f1, 0x1007 )) //Tag table speed
        {
            m_labelPhilipsTableSpeedValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1007 ).toDouble() , 'f' , 0 ) +  QString( tr( " mm/s" ) ) );
        }
        else
            m_labelPhilipsTableSpeedValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x01f1, 0x1027 )) //Tag Rotation Time
        {
            m_labelPhilipsRotationTimeValue->setText( QString::number( dicomReader.getAttributeByTag( 0x01f1, 0x1027 ).toDouble() , 'f' , 2 ) +  QString( tr( " s" ) ) );
        }
        else
            m_labelPhilipsRotationTimeValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x01f1, 0x1032 )) //Tag View Convention
        {
            m_labelPhilipsViewConventionValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x1032 ) );
        }
        else
            m_labelPhilipsViewConventionValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMFilterType ))
        {
            m_labelFilterTypeValue->setText( dicomReader.getAttributeByName( DICOMFilterType ) );
        }
        else
            m_labelFilterTypeValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x01f1, 0x104b )) //Tag Collimation
        {
            m_labelPhilipsCollimationValue->setText( dicomReader.getAttributeByTag( 0x01f1, 0x104b ) );
        }
        else
            m_labelPhilipsCollimationValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMKVP ))
        {
            m_labelVoltageValue->setText( QString::number( dicomReader.getAttributeByName( DICOMKVP ).toDouble() , 'f' , 0 ) +  QString( tr( " KV" ) ) );
        }
        else
            m_labelVoltageValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMExposureInMicroAs ))
        {
            m_labelExposureValue->setText( QString::number( dicomReader.getAttributeByName( DICOMExposureInMicroAs ).toDouble() , 'f' , 0 ) +  QString( tr( " mA" ) ) );
        }
        else
            m_labelExposureValue->setText( NotAvailableValue );

        m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' , 2 )+  QString( tr( " mm" ) ) );
        m_labelImageMatrixValue->setText( QString::number(currentImage->getColumns() , 10 ) +  QString( tr( " x " ) ) + QString::number( currentImage->getRows() , 10 ) );
    }

    if ( currentImage->getSliceLocation() != "" )
    {
        m_labelSliceLocationValue->setText( currentImage->getSliceLocation() + QString( tr( " mm" ) ) );
    }
    else
        m_labelSliceLocationValue->setText( NotAvailableValue );

    if ( dicomReader.tagExists( DICOMImageType ) )
    {
        m_labelImageTypeValue->setText( dicomReader.getAttributeByName( DICOMImageType ) );
    }
    else
        m_labelImageTypeValue->setText( NotAvailableValue );
}

void QDicomDumpCTHelixWidget::setSeriesDicomTagsValue( Series *currentSeries )
{
    if ( currentSeries->getProtocolName() != "" )
    {
        m_labelProtocolNameValue->setText( currentSeries->getProtocolName() );
    }
    else
        m_labelProtocolNameValue->setText( NotAvailableValue );

    if ( currentSeries->getPatientPosition() != "" )
    {
        m_labelPatientPositionValue->setText( currentSeries->getPatientPosition() );
    }
    else
        m_labelPatientPositionValue->setText( NotAvailableValue );
}

}
