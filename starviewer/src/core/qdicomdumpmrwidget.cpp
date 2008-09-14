/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "qdicomdumpmrwidget.h"
#include "image.h"
#include "series.h"
#include "dicomtagreader.h"

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
    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( currentImage->getPath() );
    if( ok )
    {
        if (dicomReader.tagExists( DCM_ReconstructionDiameter ))
        {
            m_labelFieldOfViewValue->setText( QString::number( dicomReader.getAttributeByName( DCM_ReconstructionDiameter ).toDouble() , 'f' , 0 ) +  QString( tr( " mm" ) ) );
        }

        if (dicomReader.tagExists( DCM_PercentPhaseFieldOfView ))
        {
            m_labelRatioFieldOfViewValue->setText( QString::number( dicomReader.getAttributeByName( DCM_PercentPhaseFieldOfView ).toDouble() , 'f' , 0 ) +  QString( tr( " %" ) ) );
        }

        if (dicomReader.tagExists( DCM_RepetitionTime ))
        {
            m_labelRepetitionTimeValue->setText( QString::number( dicomReader.getAttributeByName( DCM_RepetitionTime ).toDouble() , 'f' , 0 ) +  QString( tr( " ms" ) ) );
        }

        if (dicomReader.tagExists( DCM_EchoTime ))
        {
            m_labelEchoTimeValue->setText( QString::number( dicomReader.getAttributeByName( DCM_EchoTime ).toDouble() , 'f' , 1 ) +  QString( tr( " ms" ) ) );
        }

        if (dicomReader.tagExists( DCM_InversionTime ))
        {
            m_labelInversionTimeValue->setText( QString::number( dicomReader.getAttributeByName( DCM_InversionTime ).toDouble() , 'f' , 0 ) +  QString( tr( " ms" ) ) );
        }
        if (dicomReader.tagExists( DCM_FlipAngle))
        {
            m_labelFlipAngleValue->setText( dicomReader.getAttributeByName( DCM_FlipAngle) +  QString( tr( " degrees" ) ));
        }

        if (dicomReader.tagExists( 0x2001, 0x1082 )) //Tag Turbo-Factor
        {
            m_labelPhilipsEPIFactorValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1082 ) );
        }

        if (dicomReader.tagExists( 0x2001, 0x1013 )) //Tag EPI-Factor
        {
            m_labelPhilipsTurboFactorValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1013 ) );
        }

        if (dicomReader.tagExists( DCM_NumberOfAverages ))
        {
            m_labelNumberOfAveragesValue->setText( dicomReader.getAttributeByName( DCM_NumberOfAverages ) );
        }

        if (dicomReader.tagExists( 0x2001, 0x1003 )) //Tag B-Factor
        {
            m_labelPhilipsBFactorValue->setText( QString::number( dicomReader.getAttributeByTag( 0x2001, 0x1003 ).toDouble() , 'f' , 1 ) );
        }

        if (dicomReader.tagExists( 0x2001, 0x100b )) //Tag Image Position
        {
            m_labelPhilipsSpacialPlaneValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x100b ) );
        }

        if (dicomReader.tagExists( DCM_ReceiveCoilName ))
        {
            m_labelReceiveCoilValue->setText( dicomReader.getAttributeByName( DCM_ReceiveCoilName ) );
        }

        m_labelSliceThicknessValue->setText( QString::number( currentImage->getSliceThickness() , 'f' , 1 ) +  QString( tr( " mm" ) ) );
    }
}

void QDicomDumpMRWidget::setSeriesDicomTagsValue( Series *currentSeries )
{

    DICOMTagReader dicomReader;
    bool ok = dicomReader.setFile( currentSeries->getImagesPathList().first() );
    if( ok )
    {
        if ( dicomReader.tagExists( 0x2001, 0x1020 ) )
        {
            m_labelPhilipsScanningTechniqueValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1020 ) );
        }

        if ( dicomReader.tagExists( 0x2001, 0x1060 ) )
        {
            m_labelPhilipsNumberOfStacksValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1060 ) );
        }
    }

    m_labelPhilipsDynamicScansValue->setText( QString::number( currentSeries->getNumberOfPhases() , 10 ) );

    if ( currentSeries->getProtocolName() != "" )
    {
        m_labelProtocolNameValue->setText( currentSeries->getProtocolName() );
    }
}

}
