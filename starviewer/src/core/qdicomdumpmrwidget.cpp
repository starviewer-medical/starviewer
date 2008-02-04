/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpmrwidget.h"
#include "image.h"
#include "series.h"

namespace udg {

QDicomDumpMRWidget::QDicomDumpMRWidget(QWidget *parent)
 : QWidget(parent)
{
    setupUi( this );

}

QDicomDumpMRWidget::~QDicomDumpMRWidget()
{
}

void QDicomDumpMRWidget::setCurrentDisplayedImage( Image *currentImage )
{
    initialize();

    if ( currentImage != NULL )
    {
        setSeriesDicomTagsValue( currentImage->getParentSeries() );//Definim els valors dels tags a nivell de sèrie
        setImageDicomTagsValue( currentImage );//Defini els valors dels tags a nivell d'imatge
    }
}

void QDicomDumpMRWidget::initialize()
{
    m_labelFieldOfViewValue->setText( "-" );
    m_labelRatioFieldOfViewValue->setText( "-" );
    m_labelEchoTimeValue->setText( "-" );
    m_labelInversionTimeValue->setText( "-" );
    m_labelFlipAngleValue->setText( "-" );
    m_labelPhilipsEPIFactorValue->setText( "-" );
    m_labelPhilipsTurboFactorValue->setText( "-" );
    m_labelNumberOfAveragesValue->setText( "-" );
    m_labelPhilipsBFactorValue->setText( "-" );
    m_labelPhilipsSpacialPlaneValue->setText( "-" );
    m_labelReceiveCoilValue->setText( "-" );
    m_labelPhilipsNumberOfStacksValue->setText( "-" );
    m_labelProtocolNameValue->setText( "-" );
    m_labelPhilipsScanningTechniqueValue->setText( "-" );
}

void QDicomDumpMRWidget::setImageDicomTagsValue( Image *currentImage )
{
    if ( currentImage->getReconstructionDiameter() != "" )
    {
        m_labelFieldOfViewValue->setText( currentImage->getReconstructionDiameter() +  QString( tr( " mm" ) ) );
    }

    if ( currentImage->getPercentPhaseFieldOfView() != "" ) 
    {
        m_labelRatioFieldOfViewValue->setText( currentImage->getPercentPhaseFieldOfView() +  QString( tr( " %" ) ) );
    }

    if ( currentImage->getRepetitionTime() != "" ) 
    {
        m_labelRepetitionTimeValue->setText( currentImage->getRepetitionTime() +  QString( tr( " ms" ) ) );
    }

    if ( currentImage->getEchoTime() != "" ) 
    {
        m_labelEchoTimeValue->setText( currentImage->getEchoTime() +  QString( tr( " ms" ) ) );
    }

    if ( currentImage->getInversionTime() != "" ) 
    {
        m_labelInversionTimeValue->setText( currentImage->getInversionTime() +  QString( tr( " ms" ) ) );
    }

    if ( currentImage->getFlipAngle() != "" ) 
    {
        m_labelFlipAngleValue->setText( currentImage->getFlipAngle() +  QString( tr( " degrees" ) ) );
    }

    if ( currentImage->getPhilipsEPIFactor() != "" ) 
    {
        m_labelPhilipsEPIFactorValue->setText( currentImage->getPhilipsEPIFactor() );
    }

    if ( currentImage->getPhilipsTurboFactor() != "" ) 
    {
        m_labelPhilipsTurboFactorValue->setText( currentImage->getPhilipsTurboFactor() );
    }

    if ( currentImage->getNumberOfAverages() != "" ) 
    {
        m_labelNumberOfAveragesValue->setText( currentImage->getNumberOfAverages() );
    }

    if ( currentImage->getPhilipsBFactor() != "" ) 
    {
        m_labelPhilipsBFactorValue->setText( currentImage->getPhilipsBFactor() );
    }

    if ( currentImage->getPhilipsSpacialPlane() != "" )  
    {
        m_labelPhilipsSpacialPlaneValue->setText( currentImage->getPhilipsSpacialPlane() );
    }

    if ( currentImage->getReceiveCoilName() != "" ) 
    {
        m_labelReceiveCoilValue->setText( currentImage->getReceiveCoilName() );
    }

    m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' , 1 ) +  QString( tr( " mm" ) ) );
}

void QDicomDumpMRWidget::setSeriesDicomTagsValue( Series *currentSeries )
{
    m_labelPhilipsDynamicScansValue->setText( QString::number( currentSeries->getNumberOfPhases() , 10 ) );

    if ( currentSeries->getPhilipsNumberOfStacks() != "" ) 
    {
        m_labelPhilipsNumberOfStacksValue->setText( currentSeries->getPhilipsNumberOfStacks() );
    }

    if ( currentSeries->getProtocolName() != "" ) 
    {
        m_labelProtocolNameValue->setText( currentSeries->getProtocolName() );
    }
    else m_labelProtocolNameValue->setText( "-" );

    if ( currentSeries->getPhilipsScanningTechnique() != "" ) 
    {
        m_labelPhilipsScanningTechniqueValue->setText( currentSeries->getPhilipsScanningTechnique() );
    }
    else m_labelPhilipsScanningTechniqueValue->setText( "-" );
}

}
