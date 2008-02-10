/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "optimalviewpointinputparametersform.h"

#include "optimalviewpointparameters.h"
#include "transferfunctionio.h"
#include "logging.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

namespace udg {

OptimalViewpointInputParametersForm::OptimalViewpointInputParametersForm( QWidget * parent )
    : QInputParameters( parent )
{
    setupUi( this );

    m_advancedControlsFrame->hide();
    m_applyPushButton->setDisabled( true );

    m_parameters = 0;

    m_transferFunction.addPoint( 0.0, QColor( 0, 0, 0, 0 ) );
    m_transferFunction.addPoint( 255.0, QColor( 255, 255, 255, 255 ) );
    m_inited = false;

    this->disableIndividualSincronization();


//     connect( m_gradientEditor, SIGNAL( gradientStopsChanged(const QGradientStops &) ),
//              this, SLOT( setTransferFunction(const QGradientStops &) ) );
    connect( m_applyPushButton, SIGNAL( clicked() ), SLOT( writeAllParameters() ) );
    connect( m_applyPushButton, SIGNAL( clicked() ), SIGNAL( executionRequested() ) );

    connect( m_loadTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( loadTransferFunction() ) );
    connect( m_saveTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( saveTransferFunction() ) );

    connect( m_clusterFirstSpinBox, SIGNAL( valueChanged(int) ), SLOT( setClusterFirst(int) ) );
    connect( m_clusterLastSpinBox, SIGNAL( valueChanged(int) ), SLOT( setClusterLast(int) ) );

    connect( m_newMethodOkPushButton, SIGNAL( clicked() ), SLOT( requestNewMethod() ) );
}

OptimalViewpointInputParametersForm::~OptimalViewpointInputParametersForm()
{
}

// millor comprovar que només tingui efecte la primera vegada
void OptimalViewpointInputParametersForm::setParameters( OptimalViewpointParameters * parameters )
{
    if ( m_parameters != parameters )   // per evitar múltiples connexions
    {
        disconnect( this, SLOT( setAdjustedTransferFunction(const TransferFunction&) ) );
        m_parameters = parameters;
        connect( m_parameters, SIGNAL( changed(int) ), SLOT( readParameter(int) ) );
        connect( m_parameters, SIGNAL( signalAdjustedTransferFunction(const TransferFunction&) ),
                 SLOT( setAdjustedTransferFunction(const TransferFunction&) ) );
    }
}

void OptimalViewpointInputParametersForm::readParameter( int index )
{
    if( !m_parameters )
    {
        DEBUG_LOG("OptimalViewpointInputParametersForm: No hi ha paràmetres establerts");
    }
    else
    {
        switch ( index )
        {
            case OptimalViewpointParameters::VisualizationImageSampleDistance:
                m_doubleSpinBoxVisualizationImageSampleDistance->setValue( m_parameters->getVisualizationImageSampleDistance() );
                break;

            case OptimalViewpointParameters::VisualizationSampleDistance:
                m_doubleSpinBoxVisualizationSampleDistance->setValue( m_parameters->getVisualizationSampleDistance() );
                break;

            case OptimalViewpointParameters::VisualizationBlockLength:
                m_spinBoxVisualizationBlockLength->setValue( m_parameters->getVisualizationBlockLength() );
                break;

            case OptimalViewpointParameters::TransferFunctionObject:
//                 m_transferFunction = m_parameters->getTransferFunction();
                this->setTransferFunction( m_parameters->getTransferFunctionObject() );
                break;


            // nous paràmetres

            case OptimalViewpointParameters::ComputeWithOpacity:
                m_computeWithOpacityCheckBox->setChecked( m_parameters->getComputeWithOpacity() );
                break;

            case OptimalViewpointParameters::Compute:
                m_computeCheckBox->setChecked( m_parameters->getCompute() );
                break;
/*
            case OptimalViewpointParameters::NumberOfClusters:
                m_numberOfClustersLabel->setText( QString("<b>%1 clusters</b>").arg( (short) m_parameters->getNumberOfClusters() ) );
                DEBUG_LOG( QString("nclusters = %1").arg((short) m_parameters->getNumberOfClusters()) );
                break;
*/
            case OptimalViewpointParameters::SimilarityThreshold:
                m_similarityThresholdDoubleSpinBox->setValue( m_parameters->getSimilarityThreshold() );
                break;

            case OptimalViewpointParameters::Cluster:
                m_clusterCheckBox->setChecked( m_parameters->getCluster() );
                break;

            case OptimalViewpointParameters::ClusterFirst:
                m_clusterFirstSpinBox->setValue( m_parameters->getClusterFirst() );
                break;

            case OptimalViewpointParameters::ClusterLast:
                m_clusterLastSpinBox->setValue( m_parameters->getClusterLast() );
                break;

            case OptimalViewpointParameters::ReadExtentFromFile:
                m_readExtentFromFileCheckBox->setChecked( m_parameters->getReadExtentFromFile() );
                break;

            case OptimalViewpointParameters::GroupingMethod:
                m_groupingMethodComboBox->setCurrentIndex( m_parameters->getGroupingMethod() );
                break;
        }
    }
}

void OptimalViewpointInputParametersForm::writeAllParameters()
{
    if( !m_parameters )
    {
        DEBUG_LOG("OptimalViewpointInputParametersForm: No hi ha paràmetres establerts");
    }
    else
    {
        this->setTransferFunction( m_editorByValues->getTransferFunction() );

        // actualitzem els valors dels paràmetres
        m_parameters->setVisualizationImageSampleDistance( m_doubleSpinBoxVisualizationImageSampleDistance->value() );
        m_parameters->setVisualizationSampleDistance( m_doubleSpinBoxVisualizationSampleDistance->value() );
        m_parameters->setVisualizationBlockLength( m_spinBoxVisualizationBlockLength->value() );
        m_parameters->setTransferFunctionObject( m_transferFunction );


        // nous paràmetres
        m_parameters->setComputeWithOpacity( m_computeWithOpacityCheckBox->isChecked() );
        m_parameters->setCompute( m_computeCheckBox->isChecked() );
        m_parameters->setSimilarityThreshold( m_similarityThresholdDoubleSpinBox->value() );

        m_parameters->setCluster( m_clusterCheckBox->isChecked() );
        m_parameters->setClusterFirst( m_clusterFirstSpinBox->value() );
        m_parameters->setClusterLast( m_clusterLastSpinBox->value() );

        m_parameters->setReadExtentFromFile( m_readExtentFromFileCheckBox->isChecked() );

        m_parameters->setGroupingMethod( m_groupingMethodComboBox->currentIndex() );
    }
}


void OptimalViewpointInputParametersForm::setTransferFunction( const TransferFunction & transferFunction )
{
    m_transferFunction = transferFunction;

    m_gradientEditor->setTransferFunction( transferFunction );
//     m_gradientEditor->pointsUpdated();

    m_editorByValues->setTransferFunction( m_transferFunction );
}

void OptimalViewpointInputParametersForm::showEvent( QShowEvent * event )
{
    QWidget::showEvent( event );

    if ( !m_inited )
    {
        QGradientStops stops;
        stops << QGradientStop( 0, QColor( 0, 0, 0, 0 ) )
              << QGradientStop( 1, QColor( 255, 255, 255, 255 ) );
        m_gradientEditor->setGradientStops( stops );
        m_gradientEditor->pointsUpdated();
        m_inited = true;
    }
}

void OptimalViewpointInputParametersForm::setAdjustedTransferFunction( const TransferFunction & adjustedTransferFunction )
{
    DEBUG_LOG("OVIPF::satf");
    adjustedTransferFunction.print();
    m_gradientEditor->setTransferFunction( adjustedTransferFunction );
//     m_gradientEditor->pointsUpdated();

    m_transferFunction = adjustedTransferFunction;

    m_editorByValues->setTransferFunction( m_transferFunction );

    m_parameters->setTransferFunctionObject( m_transferFunction );

//     m_groupBoxSegmentation->setDisabled( true );
//     m_segmentationFileLabel->setDisabled( true );
//     m_openSegmentationFilePushButton->setDisabled( true );

    m_applyPushButton->setEnabled( true );
}


void OptimalViewpointInputParametersForm::loadTransferFunction()
{
    QSettings settings;

    settings.beginGroup( "OptimalViewpoint" );

    QString transferFunctionDir = settings.value( "transferFunctionDir", QString() ).toString();

    QString transferFunctionFileName =
            QFileDialog::getOpenFileName( this, tr("Load transfer function"),
                                          transferFunctionDir, tr("Transfer function files (*.tf);;All files (*)") );

    if ( !transferFunctionFileName.isNull() )
    {
        TransferFunction * transferFunction = TransferFunctionIO::fromFile( transferFunctionFileName );
        this->setTransferFunction( *transferFunction );
        delete transferFunction;

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "transferFunctionDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}

void OptimalViewpointInputParametersForm::saveTransferFunction()
{
    this->setTransferFunction( m_editorByValues->getTransferFunction() );

    QSettings settings;

    settings.beginGroup( "OptimalViewpoint" );

    QString transferFunctionDir = settings.value( "transferFunctionDir", QString() ).toString();

    QFileDialog saveDialog( this, tr("Save transfer function"), transferFunctionDir, tr("Transfer function files (*.tf);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "tf" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString transferFunctionFileName = saveDialog.selectedFiles().first();
        TransferFunctionIO::toFile( transferFunctionFileName, m_transferFunction );

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "transferFunctionDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}

void OptimalViewpointInputParametersForm::setClusterFirst( int slice )
{
    if ( m_clusterLastSpinBox->value() < slice )
        m_clusterLastSpinBox->setValue( slice );
}

void OptimalViewpointInputParametersForm::setClusterLast( int slice )
{
    if ( m_clusterFirstSpinBox->value() > slice )
        m_clusterFirstSpinBox->setValue( slice );
}


void OptimalViewpointInputParametersForm::setNumberOfSlices( unsigned short numberOfSlices )
{
    m_clusterFirstSpinBox->setMaximum( numberOfSlices - 1 );
    m_clusterLastSpinBox->setMaximum( numberOfSlices - 1 );
    m_clusterLastSpinBox->setValue( numberOfSlices - 1 );
}

void OptimalViewpointInputParametersForm::requestNewMethod()
{
    if ( m_newMethodComboBox->currentText() == "2" )
    {
        emit newMethod2Requested( m_newMethod2StepSpinBox->value(), m_newMethod2NormalizedCheckBox->isChecked() );
    }
}

void OptimalViewpointInputParametersForm::setRangeMax( unsigned char rangeMax )
{
    m_gradientEditor->setMaximum( rangeMax );
    m_editorByValues->setMaximum( rangeMax );
}


};
