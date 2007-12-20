/***************************************************************************
 *   Copyright (C) 2007 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "qoptimalviewpointextension.h"

#include <QMessageBox>

#include "optimalviewpoint.h"
#include "optimalviewpointparameters.h"

#include "logging.h"
#include "volume.h"

#include <iostream>
#include <QSettings>
#include <QFileDialog>


namespace udg {


QOptimalViewpointExtension::QOptimalViewpointExtension( QWidget * parent )
    : QWidget( parent )
{
    setupUi( this );

    m_parameters = new OptimalViewpointParameters( this );

    m_inputParametersWidget->setParameters( m_parameters );

    m_method = new OptimalViewpoint( this );
    m_method->setParameters( m_parameters );
    m_method->setMainRenderer( m_viewerWidget->getRenderer() );

    m_parameters->init();

    connect( m_openSegmentationFilePushButton, SIGNAL( clicked() ), SLOT( openSegmentationFile() ) );
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( doSegmentation() ) );




    connect( m_inputParametersWidget, SIGNAL( executionRequested() ), SLOT( execute() ) );

    connect( m_inputParametersWidget, SIGNAL( newMethod2Requested(int,bool) ), m_method, SLOT( newMethod2(int,bool) ) );

    connect( m_method, SIGNAL( scalarRange(unsigned char,unsigned char) ), SLOT( setScalarRange(unsigned char,unsigned char) ) );

    connect( m_inputParametersWidget, SIGNAL( renderPlaneRequested(short) ), SLOT( renderPlane(short) ) );


    m_automaticSegmentationWidget->hide();
    m_regularSegmentationWidget->hide();

    m_segmentationFileChosen = false;
}


QOptimalViewpointExtension::~QOptimalViewpointExtension()
{
//     delete m_method;
}


void QOptimalViewpointExtension::setInput( Volume * input )
{
    Q_ASSERT( input );
    m_method->setImage( input->getVtkData() );
    int dims[3];
    input->getDimensions( dims );
    m_inputParametersWidget->setNumberOfSlices( dims[2] );
}


void QOptimalViewpointExtension::doSegmentation()
{
    if ( m_automaticSegmentationRadioButton->isChecked() )
    {
        m_method->doAutomaticSegmentation(
                                           m_segmentationIterationsSpinBox->value(),
                                           m_segmentationBlockLengthSpinBox->value(),
                                           m_segmentationNumberOfClustersSpinBox->value(),
                                           m_segmentationNoiseDoubleSpinBox->value(),
                                           m_segmentationImageSampleDistanceDoubleSpinBox->value(),
                                           m_segmentationSampleDistanceDoubleSpinBox->value()
                                         );
    }
    else if ( m_loadSegmentationRadioButton->isChecked() )
    {
        if ( !m_segmentationFileChosen )
        {
            QMessageBox::warning( this, tr("No segmentation file chosen"),
                                  tr("Please, choose a segmentation file or do another type of segmentation.") );
            return;
        }

        if ( !m_method->doLoadSegmentation( m_segmentationFileLabel->text() ) )
        {
            QMessageBox::critical( this, tr("Segmentation error"),
                                   QString( tr("Cannot load segmentation from file %1.") ).arg( m_segmentationFileLabel->text() ) );
            return;
        }
    }
    else //if ( m_regularSegmentationRadioButton->isChecked() )
    {
        m_method->doRegularSegmentation( m_segmentationNumberOfBinsSpinBox->value() );
    }

    m_numberOfClustersLabel->setText( QString("<b>%1 clusters</b>").arg( static_cast<short>( m_method->getNumberOfClusters() ) ) );
    DEBUG_LOG( QString("nclusters = %1").arg( static_cast<short>( m_method->getNumberOfClusters() ) ) );

    m_loadSegmentationRadioButton->setDisabled( true );
    m_loadSegmentationWidget->setDisabled( true );
    m_automaticSegmentationRadioButton->setDisabled( true );
    m_automaticSegmentationWidget->setDisabled( true );
    m_regularSegmentationRadioButton->setDisabled( true );
    m_regularSegmentationWidget->setDisabled( true );
//     m_segmentationOkPushButton->setDisabled( true );
    toggleSegmentationParameters();

    disconnect( m_segmentationOkPushButton, SIGNAL( clicked() ), this, SLOT( doSegmentation() ) );
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( toggleSegmentationParameters() ) );

    // De moment cal posar aquestes dues línies. Potser es pot arreglar perquè no calguin.
    m_method->setNumberOfPlanes( 0 );
    m_method->setTransferFunction( m_parameters->getTransferFunctionObject() );
    m_viewerWidget->render();
}


void QOptimalViewpointExtension::openSegmentationFile()
{
    QSettings settings;

    settings.beginGroup( "OptimalViewpoint" );

    QString segmentationFileDir = settings.value( "segmentationFiledir", QString() ).toString();

    QString segmentationFileName =
            QFileDialog::getOpenFileName( this, tr("Open segmentation file"),
                                          segmentationFileDir, tr("Segmentation files (*.seg);;All files (*)") );

    if ( !segmentationFileName.isNull() )
    {
        m_segmentationFileLabel->setText( segmentationFileName );
        m_segmentationFileChosen = true;

        QFileInfo segmentationFileInfo( segmentationFileName );
        settings.setValue( "segmentationFileDir", segmentationFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QOptimalViewpointExtension::execute()
{
    // nous paràmetres

    m_method->setOpacityForComputing( m_parameters->getComputeWithOpacity() );
    m_method->setInterpolation( m_parameters->getInterpolation() );
    m_method->setSpecular( m_parameters->getSpecular() );
    m_method->setSpecularPower( m_parameters->getSpecularPower() );
    m_method->setUpdatePlane( m_parameters->getUpdatePlane() );
    m_method->setCompute( m_parameters->getCompute() );
    m_method->setSimilarityThreshold( m_parameters->getSimilarityThreshold() );

    bool renderCluster = m_parameters->getCluster();
    if ( renderCluster ) m_method->setClusterLimits( m_parameters->getClusterFirst(), m_parameters->getClusterLast() );
    m_method->setRenderCluster( renderCluster );

    m_method->setReadExtentFromFile( m_parameters->getReadExtentFromFile() );



    m_method->updatePlanes();
    std::cout << "OVD: update planes" << std::endl;
//     m_viewer->render();
    m_viewerWidget->render();

    if ( m_method->resultsChanged() )
    {
        std::vector<double> * entropyRateResults = m_method->getEntropyRateResults();
        std::vector<double> * excessEntropyResults = m_method->getExcessEntropyResults();

        QMessageBox * resultsDialog = new QMessageBox( tr("Results"), "", QMessageBox::Information,
                QMessageBox::Ok, QMessageBox::NoButton, QMessageBox::NoButton, m_viewerWidget );
        resultsDialog->setModal( false );
        resultsDialog->setAttribute( Qt::WA_DeleteOnClose );

        QString text = "<table cellspacing=\"8\"><tr><td></td><td align=\"center\"><b><i><u>entropy rate</u></i></b></td><td align=\"center\"><b><i><u>excess entropy</u></i></b></td></tr>";
        QString planeString = tr("Plane");
        for ( unsigned char i = 1; i <= m_parameters->getNumberOfPlanes(); i++ )
            text += "<tr><td><b>" + planeString + QString( " %1:</b></td><td align=\"center\">%2</td><td align=\"center\">%3</td></tr>" ).arg( i ).arg( (*entropyRateResults)[i], 0, 'g', 7 ).arg( (*excessEntropyResults)[i], 0, 'g', 7 );
        text += "</table>";

        resultsDialog->setText( text );
        resultsDialog->show();

        delete entropyRateResults;
        delete excessEntropyResults;
    }
}


void QOptimalViewpointExtension::setScalarRange( unsigned char rangeMin, unsigned char rangeMax )
{
    m_inputParametersWidget->setRangeMax( rangeMax );
    unsigned short maximum = rangeMax - rangeMin + 1;
    if ( maximum < m_segmentationNumberOfBinsSpinBox->maximum() )
        m_segmentationNumberOfBinsSpinBox->setMaximum( maximum );
}


void QOptimalViewpointExtension::renderPlane( short plane )
{
    m_method->renderPlanes( plane );
}


void QOptimalViewpointExtension::toggleSegmentationParameters()
{
    if ( m_segmentationLine->isVisible() )
    {
        m_segmentationOkPushButton->setText( tr("Show parameters") );
        m_loadSegmentationRadioButton->hide();
        m_loadSegmentationWidget->hide();
        m_automaticSegmentationRadioButton->hide();
        m_automaticSegmentationWidget->hide();
        m_regularSegmentationRadioButton->hide();
        m_regularSegmentationWidget->hide();
        m_segmentationLine->hide();
    }
    else
    {
        m_segmentationOkPushButton->setText( tr("Hide parameters") );
        m_loadSegmentationRadioButton->show();
        if ( m_loadSegmentationRadioButton->isChecked() ) m_loadSegmentationWidget->show();
        m_automaticSegmentationRadioButton->show();
        if ( m_automaticSegmentationRadioButton->isChecked() ) m_automaticSegmentationWidget->show();
        m_regularSegmentationRadioButton->show();
        if ( m_regularSegmentationRadioButton->isChecked() ) m_regularSegmentationWidget->show();
        m_segmentationLine->show();
    }
}


}
