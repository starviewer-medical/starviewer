/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "optimalviewpointinputparametersform.h"

#include <iostream>

#include "optimalviewpoint.h"
#include "optimalviewpointparameters.h"
// #include "magicmirrorsselectvolumesdialog.h"
#include <QFileDialog>
#include <QSettings>

namespace udg {

OptimalViewpointInputParametersForm::OptimalViewpointInputParametersForm( QWidget * parent )
    : QInputParameters( parent )
{
    setupUi( this );    // heredat d'Ui::OptimalViewpointInputParametersFormBase

    m_advancedControlsFrame->hide();
    m_groupBoxSegmentation->hide();

    m_parameters = 0;
    m_transferFunction << QGradientStop( 0, QColor( 0, 0, 0, 0 ) )
                       << QGradientStop( 1, QColor( 255, 255, 255, 255 ) );
    m_inited = false;

    this->disableIndividualSincronization();

//     connect( m_buttonSelect, SIGNAL( clicked() ), this, SLOT( selectVolume() ) );
//     connect( m_gradientEditor, SIGNAL( gradientStopsChanged(const QGradientStops &) ),
//              this, SLOT( setTransferFunction(const QGradientStops &) ) );
    connect( m_comboNumberOfPlanes, SIGNAL( currentIndexChanged(const QString &) ),
             this, SLOT( setNumberOfPlanes(const QString &) ) );
    connect( m_applyPushButton, SIGNAL( clicked() ), SLOT( writeAllParameters() ) );
    connect( m_applyPushButton, SIGNAL( clicked() ), SIGNAL( executionRequested() ) );
    connect( m_openSegmentationFilePushButton, SIGNAL( clicked() ), SLOT( openSegmentationFile() ) );

    connect( m_segmentationParametersPushButton, SIGNAL( toggled(bool) ), SLOT( toggleSegmentationParametersPushButtonText(bool) ) );
}

OptimalViewpointInputParametersForm::~OptimalViewpointInputParametersForm()
{
}

/// Assigna l'objecte que guardarà els paràmetres.
void OptimalViewpointInputParametersForm::setParameters( OptimalViewpointParameters * parameters )
{
    if ( m_parameters != parameters )
    {
        delete m_parameters;
        m_parameters = parameters;
        connect( m_parameters, SIGNAL( signalAdjustedTransferFunction(const OptimalViewpoint::TransferFunction&) ),
                 this, SLOT( setAdjustedTransferFunction(const OptimalViewpoint::TransferFunction&) ) );
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

            case OptimalViewpointParameters::SegmentationIterations:
                m_spinBoxSegmentationIterations->setValue( m_parameters->getSegmentationIterations() );
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

            case OptimalViewpointParameters::TransferFunction:
//                 m_transferFunction = m_parameters->getTransferFunction();
                this->setTransferFunction( m_parameters->getTransferFunction() );
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
        m_parameters->setSegmentationIterations( m_spinBoxSegmentationIterations->value() );
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
        m_parameters->setTransferFunction( m_transferFunction );


        // nous paràmetres
        m_parameters->setComputeWithOpacity( m_computeWithOpacityCheckBox->isChecked() );
        if ( m_interpolationComboBox->currentIndex() < 0 )
            m_interpolationComboBox->setCurrentIndex( 0 );
        m_parameters->setInterpolation( m_interpolationComboBox->currentIndex() );
        m_parameters->setSpecular( m_specularCheckBox->isChecked() );
        m_parameters->setSpecularPower( m_specularPowerDoubleSpinBox->value() );
        m_parameters->setUpdatePlane( m_updatePlaneSpinBox->value() );
        m_parameters->setCompute( m_computeCheckBox->isChecked() );
    }
}

/**
 * Mostra el quadre de diàleg que permet seleccionar els volums que es faran
 * servir.
 */
// void OptimalViewpointInputParametersForm::selectVolume()
// {
//     MagicMirrorsSelectVolumesDialog * selectVolumesDialog = new MagicMirrorsSelectVolumesDialog();
// 
//     if ( selectVolumesDialog->exec() == QDialog::Accepted )
//     {
//         std::list<int> * volumeIdList = selectVolumesDialog->getSelectedVolumes();
// 
//         if ( volumeIdList ) // si hi ha algun volum seleccionat
//         {
//             m_volumeId = volumeIdList->front();
//             m_labelVolume->setText( QString( tr("Volume %1") ).arg( volumeIdList->front() ) );
//             m_buttonSelect->setDisabled( true );
//             delete volumeIdList;
//         }
//     }
// 
//     delete selectVolumesDialog;
// }

/// Assigna la funció de transferència actual.
void OptimalViewpointInputParametersForm::setTransferFunction( const QGradientStops & stops )
{
    m_transferFunction = stops;

    m_gradientEditor->setGradientStops( stops );
    m_gradientEditor->pointsUpdated();

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

void OptimalViewpointInputParametersForm::setAdjustedTransferFunction( const OptimalViewpoint::TransferFunction & adjustedTransferFunction )
{
    std::cout << "OVIPF::satf" << std::endl;
    m_gradientEditor->setGradientStops( adjustedTransferFunction );
    m_gradientEditor->pointsUpdated();

    m_transferFunction = adjustedTransferFunction;

    m_editorByValues->setTransferFunction( m_transferFunction );

    m_parameters->setTransferFunction( m_transferFunction );

    m_groupBoxSegmentation->setDisabled( true );
    m_segmentationFileLabel->setDisabled( true );
    m_openSegmentationFilePushButton->setDisabled( true );
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

        QFileInfo segmentationFileInfo( segmentationFileName );
        settings.setValue( "segmentationFileDir", segmentationFileInfo.absolutePath() );
    }

    settings.endGroup();
}

void OptimalViewpointInputParametersForm::toggleSegmentationParametersPushButtonText( bool checked )
{
    if ( checked )
        m_segmentationParametersPushButton->setText( tr("Hide segmentation parameters") );
    else
        m_segmentationParametersPushButton->setText( tr("Show segmentation parameters") );
}


}; // end namespace udg
