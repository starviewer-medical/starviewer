/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "optimalviewpointinputparametersform.h"

#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>

#include "optimalviewpointparameters.h"
#include "transferfunctionio.h"

#include <iostream>


namespace udg {


OptimalViewpointInputParametersForm::OptimalViewpointInputParametersForm( QWidget * parent )
    : QInputParameters( parent )
{
    setupUi( this );

    m_automaticSegmentationWidget->hide();
    m_advancedControlsFrame->hide();
    m_applyPushButton->setDisabled( true );

    m_parameters = 0;

    m_segmentationFileChosen = false;

    m_transferFunction.addPoint( 0.0, QColor( 0, 0, 0, 0 ) );
    m_transferFunction.addPoint( 255.0, QColor( 255, 255, 255, 255 ) );
    m_inited = false;

    this->disableIndividualSincronization();

    connect( m_openSegmentationFilePushButton, SIGNAL( clicked() ), SLOT( openSegmentationFile() ) );
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( writeSegmentationParameters() ) );
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( requestSegmentation() ) );

//     connect( m_gradientEditor, SIGNAL( gradientStopsChanged(const QGradientStops &) ),
//              this, SLOT( setTransferFunction(const QGradientStops &) ) );
    connect( m_comboNumberOfPlanes, SIGNAL( currentIndexChanged(const QString &) ),
             this, SLOT( setNumberOfPlanes(const QString &) ) );
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

/**
 * Slot que ens serveix per indicar que hem d'actualitzar el paràmetre que
 * ens diguin mitjançant un identificador (que, en realitat, serà un enum).
 * Serveix per canviar els valors a partir d'una classe Parameters.
 */
void OptimalViewpointInputParametersForm::readParameter( int index )
{
    if( !m_parameters )
    {
        std::cerr << "OptimalViewpointInputParametersForm: No hi ha paràmetres establerts" << std::endl;
    }
    else
    {
        switch ( index )
        {
            case OptimalViewpointParameters::VolumeObject:
//                 m_volume = m_parameters->getVolume();
                break;

            case OptimalViewpointParameters::SegmentationFileName:
                m_segmentationFileLabel->setText( m_parameters->getSegmentationFileName() );
                break;

            case OptimalViewpointParameters::SegmentationNumberOfIterations:
                m_spinBoxSegmentationIterations->setValue( m_parameters->getSegmentationNumberOfIterations() );
                break;

            case OptimalViewpointParameters::SegmentationBlockLength:
                m_spinBoxSegmentationBlockLength->setValue( m_parameters->getSegmentationBlockLength() );
                break;

            case OptimalViewpointParameters::SegmentationNumberOfClusters:
                m_spinBoxSegmentationNumberOfClusters->setValue( m_parameters->getSegmentationNumberOfClusters() );
                break;

            case OptimalViewpointParameters::SegmentationNoise:
                m_doubleSpinBoxSegmentationNoise->setValue( m_parameters->getSegmentationNoise() );
                break;

            case OptimalViewpointParameters::SegmentationImageSampleDistance:
                m_doubleSpinBoxSegmentationImageSampleDistance->setValue( m_parameters->getSegmentationImageSampleDistance() );
                break;

            case OptimalViewpointParameters::SegmentationSampleDistance:
                m_doubleSpinBoxSegmentationSampleDistance->setValue( m_parameters->getSegmentationSampleDistance() );
                break;

            case OptimalViewpointParameters::NumberOfPlanes:
                m_comboNumberOfPlanes->setCurrentIndex( m_comboNumberOfPlanes->findText(
                        QString::number( m_parameters->getNumberOfPlanes() ) ) );
                break;

            case OptimalViewpointParameters::Shade:
                m_checkBoxShade->setChecked( m_parameters->getShade() );
                break;

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

            case OptimalViewpointParameters::Interpolation:
                m_interpolationComboBox->setCurrentIndex( m_parameters->getInterpolation() );
                break;

            case OptimalViewpointParameters::Specular:
                m_specularCheckBox->setChecked( m_parameters->getSpecular() );
                break;

            case OptimalViewpointParameters::SpecularPower:
                m_specularPowerDoubleSpinBox->setValue( m_parameters->getSpecularPower() );
                break;

            case OptimalViewpointParameters::UpdatePlane:
                m_updatePlaneSpinBox->setValue( m_parameters->getUpdatePlane() );
                break;

            case OptimalViewpointParameters::Compute:
                m_computeCheckBox->setChecked( m_parameters->getCompute() );
                break;

            case OptimalViewpointParameters::NumberOfClusters:
                m_numberOfClustersLabel->setText( QString("<b>%1 clusters</b>").arg( (short) m_parameters->getNumberOfClusters() ) );
                std::cout << "nclusters = " << (short) m_parameters->getNumberOfClusters() << std::endl;
                break;

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
        }
    }
}

/**
 * Escriu tots els valors de paràmetres que té actualment al Parameters
 * associat.
 */
void OptimalViewpointInputParametersForm::writeAllParameters()
{
    if( !m_parameters )
    {
        std::cerr << "OptimalViewpointInputParametersForm: No hi ha paràmetres establerts" << std::endl;
    }
    else
    {
        this->setTransferFunction( m_editorByValues->getTransferFunction() );

        // actualitzem els valors dels paràmetres
//         m_parameters->setVolumeId( m_volumeId );
        m_parameters->setSegmentationFileName( m_segmentationFileLabel->text() );
        m_parameters->setSegmentationNumberOfIterations( m_spinBoxSegmentationIterations->value() );
        m_parameters->setSegmentationBlockLength( m_spinBoxSegmentationBlockLength->value() );
        m_parameters->setSegmentationNumberOfClusters( m_spinBoxSegmentationNumberOfClusters->value() );
        m_parameters->setSegmentationNoise( m_doubleSpinBoxSegmentationNoise->value() );
        m_parameters->setSegmentationImageSampleDistance( m_doubleSpinBoxSegmentationImageSampleDistance->value() );
        m_parameters->setSegmentationSampleDistance( m_doubleSpinBoxSegmentationSampleDistance->value() );
        m_parameters->setNumberOfPlanes( m_comboNumberOfPlanes->currentText().toUShort() );
        m_parameters->setShade( m_checkBoxShade->isChecked() );
        m_parameters->setVisualizationImageSampleDistance( m_doubleSpinBoxVisualizationImageSampleDistance->value() );
        m_parameters->setVisualizationSampleDistance( m_doubleSpinBoxVisualizationSampleDistance->value() );
        m_parameters->setVisualizationBlockLength( m_spinBoxVisualizationBlockLength->value() );
        m_parameters->setTransferFunctionObject( m_transferFunction );


        // nous paràmetres
        m_parameters->setComputeWithOpacity( m_computeWithOpacityCheckBox->isChecked() );
        if ( m_interpolationComboBox->currentIndex() < 0 )
            m_interpolationComboBox->setCurrentIndex( 0 );
        m_parameters->setInterpolation( m_interpolationComboBox->currentIndex() );
        m_parameters->setSpecular( m_specularCheckBox->isChecked() );
        m_parameters->setSpecularPower( m_specularPowerDoubleSpinBox->value() );
        m_parameters->setUpdatePlane( m_updatePlaneSpinBox->value() );
        m_parameters->setCompute( m_computeCheckBox->isChecked() );
        m_parameters->setSimilarityThreshold( m_similarityThresholdDoubleSpinBox->value() );

        m_parameters->setCluster( m_clusterCheckBox->isChecked() );
        m_parameters->setClusterFirst( m_clusterFirstSpinBox->value() );
        m_parameters->setClusterLast( m_clusterLastSpinBox->value() );

        m_parameters->setReadExtentFromFile( m_readExtentFromFileCheckBox->isChecked() );
    }
}


void OptimalViewpointInputParametersForm::writeSegmentationParameters()
{
    if( !m_parameters )
    {
        std::cerr << "OptimalViewpointInputParametersForm: No hi ha paràmetres establerts" << std::endl;
        return;
    }

    m_parameters->setSegmentationFileName( m_segmentationFileLabel->text() );
    m_parameters->setSegmentationNumberOfIterations( m_spinBoxSegmentationIterations->value() );
    m_parameters->setSegmentationBlockLength( m_spinBoxSegmentationBlockLength->value() );
    m_parameters->setSegmentationNumberOfClusters( m_spinBoxSegmentationNumberOfClusters->value() );
    m_parameters->setSegmentationNoise( m_doubleSpinBoxSegmentationNoise->value() );
    m_parameters->setSegmentationImageSampleDistance( m_doubleSpinBoxSegmentationImageSampleDistance->value() );
    m_parameters->setSegmentationSampleDistance( m_doubleSpinBoxSegmentationSampleDistance->value() );
}



/// Assigna la funció de transferència actual.
void OptimalViewpointInputParametersForm::setTransferFunction( const TransferFunction & transferFunction )
{
    m_transferFunction = transferFunction;

    m_gradientEditor->setTransferFunction( transferFunction );
//     m_gradientEditor->pointsUpdated();

    m_editorByValues->setTransferFunction( m_transferFunction );
}

/**
 * Mètode reimplementat per inicialitzar la funció de transferència quan es
 * mostra el widget per primer cop. Si no es fa així hi ha problemes amb el
 * GradientEditor.
 */
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
    std::cout << "OVIPF::satf" << std::endl;
    adjustedTransferFunction.print();
    m_gradientEditor->setTransferFunction( adjustedTransferFunction );
//     m_gradientEditor->pointsUpdated();

    m_transferFunction = adjustedTransferFunction;

    m_editorByValues->setTransferFunction( m_transferFunction );

    m_parameters->setTransferFunctionObject( m_transferFunction );

//     m_groupBoxSegmentation->setDisabled( true );
//     m_segmentationFileLabel->setDisabled( true );
//     m_openSegmentationFilePushButton->setDisabled( true );
    m_loadSegmentationRadioButton->setDisabled( true );
    m_loadSegmentationWidget->setDisabled( true );
    m_automaticSegmentationRadioButton->setDisabled( true );
    m_automaticSegmentationWidget->setDisabled( true );





//     m_segmentationOkPushButton->setDisabled( true );
    toggleSegmentationParameters();

    disconnect( m_segmentationOkPushButton, SIGNAL( clicked() ), this, SLOT( writeSegmentationParameters() ) );
    disconnect( m_segmentationOkPushButton, SIGNAL( clicked() ), this, SLOT( requestSegmentation() ) );
    connect( m_segmentationOkPushButton, SIGNAL( clicked() ), SLOT( toggleSegmentationParameters() ) );






    m_applyPushButton->setEnabled( true );
}


void OptimalViewpointInputParametersForm::toggleSegmentationParameters()
{
    if ( m_segmentationLine->isVisible() )
    {
        m_segmentationOkPushButton->setText( tr("Show parameters") );
        m_loadSegmentationRadioButton->hide();
        m_loadSegmentationWidget->hide();
        m_automaticSegmentationRadioButton->hide();
        m_automaticSegmentationWidget->hide();
        m_segmentationLine->hide();
    }
    else
    {
        m_segmentationOkPushButton->setText( tr("Hide parameters") );
        m_loadSegmentationRadioButton->show();
        if ( m_loadSegmentationRadioButton->isChecked() ) m_loadSegmentationWidget->show();
        m_automaticSegmentationRadioButton->show();
        if ( m_automaticSegmentationRadioButton->isChecked() ) m_automaticSegmentationWidget->show();
        m_segmentationLine->show();
    }
}


void OptimalViewpointInputParametersForm::setNumberOfPlanes( const QString & numberOfPlanes )
{
    m_updatePlaneSpinBox->setMaximum( numberOfPlanes.toInt() );
}

void OptimalViewpointInputParametersForm::openSegmentationFile()
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



void OptimalViewpointInputParametersForm::requestSegmentation()
{
    if ( m_loadSegmentationRadioButton->isChecked() )
    {
        m_parameters->setSegmentation( OptimalViewpointParameters::LoadSegmentation );
        if ( m_segmentationFileChosen ) emit segmentationRequested();
        else QMessageBox::warning( this, tr("No segmentation file chosen"),
                                   tr("Please, choose a segmentation file or do an automatic segmentation.") );
    }
    else
    {
        m_parameters->setSegmentation( OptimalViewpointParameters::AutomaticSegmentation );
        emit segmentationRequested();
    }
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


};
