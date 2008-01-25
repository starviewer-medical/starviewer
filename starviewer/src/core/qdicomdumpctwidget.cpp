/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpctwidget.h"
#include "series.h"
#include "image.h"

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

    if ( currentImage->getExposureTime() != "" )
    {
        m_labelExposureTimeValue->setText( currentImage->getExposureTime() +  QString( tr( " ms" ) ) );
    }

    if ( currentImage->getPhilipsScanLength() != "" )
    {
        m_labelPhilipsScanLengthValue->setText( currentImage->getPhilipsScanLength()  +  QString( tr( " mm" ) ) );
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

    if ( currentImage->getPhilipsCycleTime() != "" )
    {
        m_labelPhilipsCycleTimeValue->setText( currentImage->getPhilipsCycleTime() +  QString( tr( " s" ) ) );
    }

    if ( currentImage->getTilt() != "" )
    {
        m_labelTiltValue->setText( currentImage->getTilt() +  QString( tr( " degrees" ) ) );
    }

    if ( currentImage->getPhilipsTableIncrement() != "" )
    {
        m_labelPhilipsTableIncrementValue->setText( currentImage->getPhilipsTableIncrement() +  QString( tr( " mm" ) ) );
    }

    if ( currentImage->getScanArc() != "" )
    {
        m_labelScanArcValue->setText( currentImage->getScanArc() +  QString( " degrees" ) );
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

    m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' ,2 )+  QString( tr( " mm" ) ) );
    m_labelImageMatrixValue->setText( QString::number( currentImage->getColumns() , 10 ) +  QString( tr( " x " ) ) + QString::number( currentImage->getRows() , 10 ) );
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
