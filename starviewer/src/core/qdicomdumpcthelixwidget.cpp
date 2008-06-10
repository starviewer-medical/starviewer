/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpcthelixwidget.h"
#include "series.h"
#include "image.h"


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
    if ( currentImage->getReconstructionDiameter() != "" )
    {
        m_labelReconstructionDiameterValue->setText( currentImage->getReconstructionDiameter()  +  QString( tr( " mm" ) ) );
    }

    if ( currentImage->getTableHeight() != "" )
    {
        m_labelTableHeightValue->setText( currentImage->getTableHeight() +  QString( tr( " mm" ) ) );
    }

    if ( currentImage->getSliceLocation() != "" )
    {
        m_labelSliceLocationValue->setText( currentImage->getSliceLocation() + QString( tr( " mm" ) ) );
    }

    if ( currentImage->getPhilipsScanTime() != "" )
    {
        m_labelPhilipsScanTimeValue->setText( currentImage->getPhilipsScanTime() +  QString( tr( " s" ) ) );
    }

    if ( currentImage->getPhilipsScanLength() != "" )
    {
        m_labelPhilipsScanLengthValue->setText( currentImage->getPhilipsScanLength()  +  QString( tr( " mm" ) ) );
    }

    if ( currentImage->getPhilipsPitch() != "" )
    {
        m_labelPhilipsPitchValue->setText( currentImage->getPhilipsPitch() +  QString( tr( " " ) ) );
    }

    if ( currentImage->getSpacingBetweenSlices() != "" )
    {
        m_labelSpacingBetweenSlicesValue->setText( currentImage->getSpacingBetweenSlices() +  QString( tr( " mm" ) ) );
    }

    if ( currentImage->getPhilipsTableSpeed() != "" )
    {
        m_labelPhilipsTableSpeedValue->setText( currentImage->getPhilipsTableSpeed() +  QString( tr( " mm/s" ) ) );
    }

    if ( currentImage->getPhilipsRotationTime() != "" )
    {
        m_labelPhilipsRotationTimeValue->setText( currentImage->getPhilipsRotationTime() +  QString( tr( " s" ) ) );
    }

    if ( currentImage->getImageType() != "" )
    {
        m_labelImageTypeValue->setText( currentImage->getImageType() );
    }

    if ( currentImage->getPhilipsViewConvention() != "" )
    {
        m_labelPhilipsViewConventionValue->setText( currentImage->getPhilipsViewConvention() );
    }

    if ( currentImage->getFilterType() != "" )
    {
        m_labelFilterTypeValue->setText( currentImage->getFilterType() );
    }

    if ( currentImage->getPhilipsCollimation() != "" )
    {
        m_labelPhilipsCollimationValue->setText( currentImage->getPhilipsCollimation() );
    }

    if ( currentImage->getKiloVoltagePeak() != 0 )
    {
        m_labelVoltageValue->setText( QString::number( currentImage->getKiloVoltagePeak() , 'f' , 0 ) +  QString( tr( " KV" ) ) );
    }
    else m_labelVoltageValue->setText( "-" );

    if ( currentImage->getMilliAmpersSecond() != 0 )
    {
        m_labelExposureValue->setText( QString::number( currentImage->getMilliAmpersSecond() , 'f' , 0 ) +  QString( tr( " mA" ) ) );
    }
    else m_labelExposureValue->setText( "-" );

    m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' , 2 )+  QString( tr( " mm" ) ) );
    m_labelImageMatrixValue->setText( QString::number(currentImage->getColumns() , 10 ) +  QString( tr( " x " ) ) + QString::number( currentImage->getRows() , 10 ) );
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
