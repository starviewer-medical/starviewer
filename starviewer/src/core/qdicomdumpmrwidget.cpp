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
#include "dicomdictionary.h"

namespace udg {

const QString NotAvailableValue( QObject::tr("N/A") );

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
        if (dicomReader.tagExists( DICOMReconstructionDiameter ))
        {
            m_labelFieldOfViewValue->setText( QString::number( dicomReader.getAttributeByName( DICOMReconstructionDiameter ).toDouble() , 'f' , 0 ) +  QString( tr( " mm" ) ) );
        }
        else
            m_labelFieldOfViewValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMPercentPhaseFieldOfView ))
        {
            m_labelRatioFieldOfViewValue->setText( QString::number( dicomReader.getAttributeByName( DICOMPercentPhaseFieldOfView ).toDouble() , 'f' , 0 ) +  QString( tr( " %" ) ) );
        }
        else
            m_labelRatioFieldOfViewValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMRepetitionTime ))
        {
            m_labelRepetitionTimeValue->setText( QString::number( dicomReader.getAttributeByName( DICOMRepetitionTime ).toDouble() , 'f' , 0 ) +  QString( tr( " ms" ) ) );
        }
        else
            m_labelRepetitionTimeValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMEchoTime ))
        {
            m_labelEchoTimeValue->setText( QString::number( dicomReader.getAttributeByName( DICOMEchoTime ).toDouble() , 'f' , 1 ) +  QString( tr( " ms" ) ) );
        }
        else
            m_labelEchoTimeValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMInversionTime ))
        {
            m_labelInversionTimeValue->setText( QString::number( dicomReader.getAttributeByName( DICOMInversionTime ).toDouble() , 'f' , 0 ) +  QString( tr( " ms" ) ) );
        }
        else
            m_labelInversionTimeValue->setText( NotAvailableValue );
        
        if (dicomReader.tagExists( DICOMFlipAngle))
        {
            m_labelFlipAngleValue->setText( dicomReader.getAttributeByName( DICOMFlipAngle) +  QString( tr( " degrees" ) ));
        }
        else
            m_labelFlipAngleValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x2001, 0x1082 )) //Tag Turbo-Factor
        {
            m_labelPhilipsEPIFactorValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1082 ) );
        }        
        else
            m_labelPhilipsEPIFactorValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x2001, 0x1013 )) //Tag EPI-Factor
        {
            m_labelPhilipsTurboFactorValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1013 ) );
        }
        else
            m_labelPhilipsTurboFactorValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMNumberOfAverages ))
        {
            m_labelNumberOfAveragesValue->setText( dicomReader.getAttributeByName( DICOMNumberOfAverages ) );
        }
        else
            m_labelNumberOfAveragesValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x2001, 0x1003 )) //Tag B-Factor
        {
            m_labelPhilipsBFactorValue->setText( QString::number( dicomReader.getAttributeByTag( 0x2001, 0x1003 ).toDouble() , 'f' , 1 ) );
        }
        else
            m_labelPhilipsBFactorValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( 0x2001, 0x100b )) //Tag Image Position
        {
            m_labelPhilipsSpacialPlaneValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x100b ) );
        }
        else
            m_labelPhilipsSpacialPlaneValue->setText( NotAvailableValue );

        if (dicomReader.tagExists( DICOMReceiveCoilName ))
        {
            m_labelReceiveCoilValue->setText( dicomReader.getAttributeByName( DICOMReceiveCoilName ) );
        }
        else
            m_labelReceiveCoilValue->setText( NotAvailableValue );

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
        else
            m_labelPhilipsScanningTechniqueValue->setText( NotAvailableValue );

        if ( dicomReader.tagExists( 0x2001, 0x1060 ) )
        {
            m_labelPhilipsNumberOfStacksValue->setText( dicomReader.getAttributeByTag( 0x2001, 0x1060 ) );
        }
        else
            m_labelPhilipsNumberOfStacksValue->setText( NotAvailableValue );
    }

    m_labelPhilipsDynamicScansValue->setText( QString::number( currentSeries->getNumberOfPhases() , 10 ) );

    if ( currentSeries->getProtocolName() != "" )
    {
        m_labelProtocolNameValue->setText( currentSeries->getProtocolName() );
    }
    else
        m_labelProtocolNameValue->setText( NotAvailableValue );
}

}
