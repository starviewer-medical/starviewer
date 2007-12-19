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
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( writeSegmentationParameters() ) );
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( doSegmentation() ) );




    connect( m_inputParametersWidget, SIGNAL( executionRequested() ), SLOT( execute() ) );

    connect( m_inputParametersWidget, SIGNAL( newMethod2Requested(int,bool) ), m_method, SLOT( newMethod2(int,bool) ) );

    connect( m_method, SIGNAL( scalarRange(unsigned char,unsigned char) ), SLOT( setScalarRange(unsigned char,unsigned char) ) );

    connect( m_inputParametersWidget, SIGNAL( renderPlaneRequested(short) ), SLOT( renderPlane(short) ) );


    m_automaticSegmentationWidget->hide();
    m_regularSegmentationWidget->hide();

    m_segmentationFileChosen = false;

    connect( m_parameters, SIGNAL( changed(int) ), SLOT( readParameter(int) ) );    // hauria de poder funcionar tot sense això, però de moment cal
}


QOptimalViewpointExtension::~QOptimalViewpointExtension()
{
//     delete m_method;
}


void QOptimalViewpointExtension::setInput( Volume * input )
{
    Q_ASSERT( input );
    Q_ASSERT( !m_parameters->getVolumeObject() );
    m_parameters->setVolumeObject( input );
    int dims[3];
    input->getDimensions( dims );
    m_inputParametersWidget->setNumberOfSlices( dims[2] );
}


void QOptimalViewpointExtension::doSegmentation()
{
    if ( m_loadSegmentationRadioButton->isChecked() )
    {
        m_parameters->setSegmentation( OptimalViewpointParameters::LoadSegmentation );
        if ( !m_segmentationFileChosen )
            QMessageBox::warning( this, tr("No segmentation file chosen"),
                                   tr("Please, choose a segmentation file or do an automatic segmentation.") );
    }
    else if ( m_regularSegmentationRadioButton->isChecked() )
    {
        m_parameters->setSegmentation( OptimalViewpointParameters::RegularSegmentation );
    }
    else
    {
        m_parameters->setSegmentation( OptimalViewpointParameters::AutomaticSegmentation );
    }

    switch ( m_parameters->getSegmentation() )
    {
        case OptimalViewpointParameters::LoadSegmentation:
            if ( !m_method->loadSegmentationFromFile() )
            {
                QMessageBox::critical( this, tr("Segmentation error"),
                                       QString( tr("Cannot load segmentation from file %1.") ).arg( m_parameters->getSegmentationFileName() ) );
                return;
            }
            break;

        case OptimalViewpointParameters::AutomaticSegmentation:
            m_method->doAutomaticSegmentation();
            break;

        case OptimalViewpointParameters::RegularSegmentation:
            m_method->rescale( m_segmentationNumberOfBinsSpinBox->value() );
            break;
    }

    m_loadSegmentationRadioButton->setDisabled( true );
    m_loadSegmentationWidget->setDisabled( true );
    m_automaticSegmentationRadioButton->setDisabled( true );
    m_automaticSegmentationWidget->setDisabled( true );
    m_regularSegmentationRadioButton->setDisabled( true );
    m_regularSegmentationWidget->setDisabled( true );
//     m_segmentationOkPushButton->setDisabled( true );
    toggleSegmentationParameters();

    disconnect( m_segmentationOkPushButton, SIGNAL( clicked() ), this, SLOT( writeSegmentationParameters() ) );
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


void QOptimalViewpointExtension::writeSegmentationParameters()
{
    if( !m_parameters )
    {
        DEBUG_LOG("OptimalViewpointInputParametersForm: No hi ha paràmetres establerts");
        return;
    }

    m_parameters->setSegmentationFileName( m_segmentationFileLabel->text() );
    m_parameters->setSegmentationNumberOfIterations( m_segmentationIterationsSpinBox->value() );
    m_parameters->setSegmentationBlockLength( m_segmentationBlockLengthSpinBox->value() );
    m_parameters->setSegmentationNumberOfClusters( m_segmentationNumberOfClustersSpinBox->value() );
    m_parameters->setSegmentationNoise( m_segmentationNoiseDoubleSpinBox->value() );
    m_parameters->setSegmentationImageSampleDistance( m_segmentationImageSampleDistanceDoubleSpinBox->value() );
    m_parameters->setSegmentationSampleDistance( m_segmentationSampleDistanceDoubleSpinBox->value() );
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


void QOptimalViewpointExtension::readParameter( int index )
{
    if( !m_parameters )
    {
        DEBUG_LOG("OptimalViewpointInputParametersForm: No hi ha paràmetres establerts");
    }
    else
    {
        switch ( index )
        {
            case OptimalViewpointParameters::NumberOfClusters:
                m_numberOfClustersLabel->setText( QString("<b>%1 clusters</b>").arg( (short) m_parameters->getNumberOfClusters() ) );
                DEBUG_LOG( QString("nclusters = %1").arg((short) m_parameters->getNumberOfClusters()) );
                break;
        }
    }
}


}
