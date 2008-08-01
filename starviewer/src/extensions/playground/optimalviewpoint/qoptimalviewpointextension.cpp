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

#include "vector3.h"
#include <QTextStream>
#include "optimalviewpointvolume.h"
#include <QTime>


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


    m_automaticSegmentationWidget->hide();
    m_regularSegmentationWidget->hide();

    m_segmentationFileChosen = false;


    connect( m_parameters, SIGNAL( changed(int) ), SLOT( readParameter(int) ) );

    connect( m_visualizationOkPushButton, SIGNAL( clicked() ), SLOT( doVisualization() ) );

    createConnections();

    QAction * obscurancesAction0 = new QAction( tr("Load from file..."), m_obscurancesPushButton );
    m_obscurancesPushButton->addAction( obscurancesAction0 );
    connect( obscurancesAction0, SIGNAL( triggered() ), this, SLOT( loadObscurances() ) );

    connect( m_obscurancesPushButton, SIGNAL( clicked() ), this, SLOT( computeObscurances() ) );
    connect( m_saliencyPushButton, SIGNAL( clicked() ), this, SLOT( computeSaliency() ) );

    connect( m_comboNumberOfPlanes, SIGNAL( currentIndexChanged(const QString &) ), SLOT( setNumberOfPlanes(const QString &) ) );
    connect( m_viewpointSelectionOkPushButton, SIGNAL( clicked() ), SLOT( doViewpointSelection() ) );
    connect( m_updatePlaneRenderPushButton, SIGNAL( clicked() ), SLOT( renderPlane() ) );

    m_parameters->setUpdatePlane( -1 ); // de moment ho poso aquí, perquè s'ha d'inicialitzar a algun lloc

    connect( m_viewpointEntropiesOkPushButton, SIGNAL( clicked() ), SLOT( computeViewpointEntropies() ) );

    connect( m_cameraGetPushButton, SIGNAL( clicked() ), SLOT( getCameraParameters() ) );
    connect( m_cameraSetPushButton, SIGNAL( clicked() ), SLOT( setCameraParameters() ) );
    connect( m_cameraLoadPushButton, SIGNAL( clicked() ), SLOT( loadCameraParameters() ) );
    connect( m_cameraSavePushButton, SIGNAL( clicked() ), SLOT( saveCameraParameters() ) );


    connect( m_viewpointSaliencyPushButton, SIGNAL( clicked() ), this, SLOT( computeViewpointSaliency() ) );





    //m_segmentationWidget->setChecked( false );
    //m_visualizationWidget->setChecked( true );
}


QOptimalViewpointExtension::~QOptimalViewpointExtension()
{
//     delete m_method;
}


void QOptimalViewpointExtension::setInput( Volume * input )
{
    Q_ASSERT( input );

    Volume * volume = input;

    if ( volume->getNumberOfPhases() > 1 ) volume = volume->getPhaseVolume( 14 );

    m_method->setImage( volume->getVtkData() );
    int dims[3];
    DEBUG_LOG( "input->getDimensions" );
    volume->getDimensions( dims );
    DEBUG_LOG( "m_inputParametersWidget->setNumberOfSlices" );
    m_inputParametersWidget->setNumberOfSlices( dims[2] );
    DEBUG_LOG( "end setInput" );
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

    m_segmentationWidget->setChecked( false );
    m_visualizationOkPushButton->setEnabled( true );
    m_visualizationWidget->setChecked( true );
    m_viewpointSelectionOkPushButton->setEnabled( true );
    m_viewpointEntropiesOkPushButton->setEnabled( true );
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
    writeAllParameters();
    // nous paràmetres

    m_method->setOpacityForComputing( m_parameters->getComputeWithOpacity() );
    m_method->setUpdatePlane( m_parameters->getUpdatePlane() );
    m_method->setSimilarityThreshold( m_parameters->getSimilarityThreshold() );

    bool renderCluster = m_parameters->getCluster();
    if ( renderCluster ) m_method->setClusterLimits( m_parameters->getClusterFirst(), m_parameters->getClusterLast() );
    m_method->setRenderCluster( renderCluster );

    m_method->setReadExtentFromFile( m_parameters->getReadExtentFromFile() );



    m_method->updatePlanes();
    std::cout << "OVD: update planes" << std::endl;
//     m_viewer->render();
    QTime t;
    t.start();
    m_viewerWidget->render();
    int elapsed = t.elapsed();
    DEBUG_LOG( QString( "Temps de render: %1 s" ).arg( elapsed / 1000.0 ) );
    INFO_LOG( QString( "Temps de render: %1 s" ).arg( elapsed / 1000.0 ) );

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


void QOptimalViewpointExtension::renderPlane()
{
    m_method->renderPlanes( m_updatePlaneSpinBox->value() );
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
            case OptimalViewpointParameters::Shade:
                m_shadeCheckBox->setChecked( m_parameters->getShade() );
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

            case OptimalViewpointParameters::Obscurances:
                m_obscurancesCheckBox->setChecked( m_parameters->getObscurances() );
                break;

            case OptimalViewpointParameters::ObscurancesFactor:
                m_obscurancesFactorDoubleSpinBox->setValue( m_parameters->getObscurancesFactor() );
                break;

            case OptimalViewpointParameters::ObscurancesFilterLow:
                m_obscurancesFilterLowDoubleSpinBox->setValue( m_parameters->getObscurancesFilterLow() );
                break;

            case OptimalViewpointParameters::ObscurancesFilterHigh:
                m_obscurancesFilterHighDoubleSpinBox->setValue( m_parameters->getObscurancesFilterHigh() );
                break;

            case OptimalViewpointParameters::NumberOfPlanes:
                m_comboNumberOfPlanes->setCurrentIndex( m_comboNumberOfPlanes->findText( QString::number( m_parameters->getNumberOfPlanes() ) ) );
                break;

            case OptimalViewpointParameters::UpdatePlane:
                m_updatePlaneSpinBox->setValue( m_parameters->getUpdatePlane() );
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

            case OptimalViewpointParameters::Fx:
                m_fxCheckBox->setChecked( m_parameters->getFx() );
                break;

            case OptimalViewpointParameters::FxContour:
                m_fxContourDoubleSpinBox->setValue( m_parameters->getFxContour() );
                break;

            case OptimalViewpointParameters::FxSaliency:
                m_fxSaliencyCheckBox->setChecked( m_parameters->getFxSaliency() );
                break;

            case OptimalViewpointParameters::FxSaliencyA:
                m_fxSaliencyADoubleSpinBox->setValue( m_parameters->getFxSaliencyA() );
                break;

            case OptimalViewpointParameters::FxSaliencyB:
                m_fxSaliencyBDoubleSpinBox->setValue( m_parameters->getFxSaliencyB() );
                break;

            case OptimalViewpointParameters::FxSaliencyLow:
                m_fxSaliencyLowDoubleSpinBox->setValue( m_parameters->getFxSaliencyLow() );
                break;

            case OptimalViewpointParameters::FxSaliencyHigh:
                m_fxSaliencyHighDoubleSpinBox->setValue( m_parameters->getFxSaliencyHigh() );
                break;
        }
    }
}

void QOptimalViewpointExtension::writeAllParameters()
{
    if( !m_parameters )
    {
        DEBUG_LOG("OptimalViewpointInputParametersForm: No hi ha paràmetres establerts");
    }
    else
    {
    }
}


void QOptimalViewpointExtension::doVisualization()
{
    m_parameters->setShade( m_shadeCheckBox->isChecked() );
    if ( m_interpolationComboBox->currentIndex() < 0 )
        m_interpolationComboBox->setCurrentIndex( 0 );
    m_parameters->setInterpolation( m_interpolationComboBox->currentIndex() );
    m_parameters->setSpecular( m_specularCheckBox->isChecked() );
    m_parameters->setSpecularPower( m_specularPowerDoubleSpinBox->value() );
    m_parameters->setObscurances( m_obscurancesCheckBox->isChecked() );
    m_parameters->setObscurancesFactor( m_obscurancesFactorDoubleSpinBox->value() );
    m_parameters->setObscurancesFilterLow( m_obscurancesFilterLowDoubleSpinBox->value() );
    m_parameters->setObscurancesFilterHigh( m_obscurancesFilterHighDoubleSpinBox->value() );
    m_parameters->setFx( m_fxCheckBox->isChecked() );
    m_parameters->setFxContour( m_fxContourDoubleSpinBox->value() );
    m_parameters->setFxSaliency( m_fxSaliencyCheckBox->isChecked() );
    m_parameters->setFxSaliencyA( m_fxSaliencyADoubleSpinBox->value() );
    m_parameters->setFxSaliencyB( m_fxSaliencyBDoubleSpinBox->value() );
    m_parameters->setFxSaliencyLow( m_fxSaliencyLowDoubleSpinBox->value() );
    m_parameters->setFxSaliencyHigh( m_fxSaliencyHighDoubleSpinBox->value() );

    m_viewerWidget->render();
}


void QOptimalViewpointExtension::createConnections()
{
//     connect( m_interpolationComboBox, SIGNAL( currentIndexChanged(int) ), m_parameters, SLOT( setInterpolation(int) ) );
//     connect( m_shadeCheckBox, SIGNAL( toggled(bool) ), m_parameters, SLOT( setShade(bool) ) );
}


void QOptimalViewpointExtension::computeObscurances()
{
    m_method->computeObscurances( m_obscuranceDirectionsSpinBox->value(),
                                  m_obscuranceMaximumDistanceDoubleSpinBox->value(),
                                  m_obscuranceFunctionComboBox->currentIndex(),
                                  m_obscuranceVariantComboBox->currentIndex() );
}


void QOptimalViewpointExtension::loadObscurances()
{
    QSettings settings;

    settings.beginGroup( "OptimalViewpoint" );

    QString obscurancesDir = settings.value( "obscurancesDir", QString() ).toString();

    QString obscurancesFileName =
            QFileDialog::getOpenFileName( this, tr("Load obscurances from file"),
                                          obscurancesDir, tr("Data files (*.dat);;All files (*)") );

    if ( !obscurancesFileName.isNull() )
    {
        bool color = m_obscuranceVariantComboBox->currentIndex() >= OptimalViewpointVolume::OpacityColorBleeding;

        if ( !m_method->loadObscurances( obscurancesFileName, color ) )
        {
            ERROR_LOG( QString( "No es pot llegir el fitxer " ) + obscurancesFileName );
            QMessageBox::warning( this, tr("Can't load"), QString( tr("Can't load from file ") ) + obscurancesFileName );
            return;
        }

        QFileInfo obscurancesFileInfo( obscurancesFileName );
        settings.setValue( "obscurancesDir", obscurancesFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QOptimalViewpointExtension::doViewpointSelection()
{
    m_parameters->setNumberOfPlanes( m_comboNumberOfPlanes->currentText().toUShort() );
    m_parameters->setUpdatePlane( m_updatePlaneSpinBox->value() );
}


void QOptimalViewpointExtension::setNumberOfPlanes( const QString & numberOfPlanes )
{
    m_updatePlaneSpinBox->setMaximum( numberOfPlanes.toInt() );
}


void QOptimalViewpointExtension::computeSaliency()
{
    m_method->computeSaliency();
}


void QOptimalViewpointExtension::computeViewpointEntropies()
{
    m_parameters->setVisualizationImageSampleDistance( m_doubleSpinBoxVisualizationImageSampleDistance->value() );
    m_parameters->setVisualizationSampleDistance( m_doubleSpinBoxVisualizationSampleDistance->value() );
    m_parameters->setVisualizationBlockLength( m_spinBoxVisualizationBlockLength->value() );
    m_method->computeViewpointEntropies();

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


void QOptimalViewpointExtension::getCameraParameters()
{
    Vector3 position, focus, up;

    m_method->getCameraParameters( position, focus, up );

    m_cameraPositionXDoubleSpinBox->setValue( position.x );
    m_cameraPositionYDoubleSpinBox->setValue( position.y );
    m_cameraPositionZDoubleSpinBox->setValue( position.z );

    m_cameraFocusXDoubleSpinBox->setValue( focus.x );
    m_cameraFocusYDoubleSpinBox->setValue( focus.y );
    m_cameraFocusZDoubleSpinBox->setValue( focus.z );

    m_cameraUpXDoubleSpinBox->setValue( up.x );
    m_cameraUpYDoubleSpinBox->setValue( up.y );
    m_cameraUpZDoubleSpinBox->setValue( up.z );
}


void QOptimalViewpointExtension::setCameraParameters()
{
    Vector3 position, focus, up;

    position.x = m_cameraPositionXDoubleSpinBox->value();
    position.y = m_cameraPositionYDoubleSpinBox->value();
    position.z = m_cameraPositionZDoubleSpinBox->value();

    focus.x = m_cameraFocusXDoubleSpinBox->value();
    focus.y = m_cameraFocusYDoubleSpinBox->value();
    focus.z = m_cameraFocusZDoubleSpinBox->value();

    up.x = m_cameraUpXDoubleSpinBox->value();
    up.y = m_cameraUpYDoubleSpinBox->value();
    up.z = m_cameraUpZDoubleSpinBox->value();

    m_method->setCameraParameters( position, focus, up );
}


void QOptimalViewpointExtension::loadCameraParameters()
{
    QSettings settings;

    settings.beginGroup( "OptimalViewpoint" );

    QString cameraDir = settings.value( "cameraDir", QString() ).toString();

    QString cameraFileName =
            QFileDialog::getOpenFileName( this, tr("Load camera parameters"),
                                          cameraDir, tr("Camera files (*.cam);;All files (*)") );

    if ( !cameraFileName.isNull() )
    {
        QFile cameraFile( cameraFileName );

        if ( !cameraFile.open( QFile::ReadOnly | QFile::Text ) )
        {
            ERROR_LOG( QString( "No es pot llegir el fitxer " ) + cameraFileName );
            QMessageBox::warning( this, tr("Can't load"), QString( tr("Can't load from file ") ) + cameraFileName );
            return;
        }

        QTextStream in( &cameraFile );

        Vector3 position, focus, up;

        if ( !in.atEnd() ) in >> position.x;
        if ( !in.atEnd() ) in >> position.y;
        if ( !in.atEnd() ) in >> position.z;

        if ( !in.atEnd() ) in >> focus.x;
        if ( !in.atEnd() ) in >> focus.y;
        if ( !in.atEnd() ) in >> focus.z;

        if ( !in.atEnd() ) in >> up.x;
        if ( !in.atEnd() ) in >> up.y;
        if ( !in.atEnd() ) in >> up.z;

        m_cameraPositionXDoubleSpinBox->setValue( position.x );
        m_cameraPositionYDoubleSpinBox->setValue( position.y );
        m_cameraPositionZDoubleSpinBox->setValue( position.z );

        m_cameraFocusXDoubleSpinBox->setValue( focus.x );
        m_cameraFocusYDoubleSpinBox->setValue( focus.y );
        m_cameraFocusZDoubleSpinBox->setValue( focus.z );

        m_cameraUpXDoubleSpinBox->setValue( up.x );
        m_cameraUpYDoubleSpinBox->setValue( up.y );
        m_cameraUpZDoubleSpinBox->setValue( up.z );

        cameraFile.close();

        QFileInfo cameraFileInfo( cameraFileName );
        settings.setValue( "cameraDir", cameraFileInfo.absolutePath() );

        setCameraParameters();
    }

    settings.endGroup();
}


void QOptimalViewpointExtension::saveCameraParameters()
{
    QSettings settings;

    settings.beginGroup( "OptimalViewpoint" );

    QString cameraDir = settings.value( "cameraDir", QString() ).toString();

    QFileDialog saveDialog( this, tr("Save camera parameters"), cameraDir, tr("Camera files (*.cam);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "cam" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString cameraFileName = saveDialog.selectedFiles().first();
        QFile cameraFile( cameraFileName );

        if ( !cameraFile.open( QFile::WriteOnly | QFile::Truncate | QFile::Text ) )
        {
            ERROR_LOG( QString( "No es pot escriure al fitxer " ) + cameraFileName );
            QMessageBox::warning( this, tr("Can't save"), QString( tr("Can't save to file ") ) + cameraFileName );
            return;
        }

        QTextStream out( &cameraFile );

        out << m_cameraPositionXDoubleSpinBox->value() << "\n";
        out << m_cameraPositionYDoubleSpinBox->value() << "\n";
        out << m_cameraPositionZDoubleSpinBox->value() << "\n";

        out << m_cameraFocusXDoubleSpinBox->value() << "\n";
        out << m_cameraFocusYDoubleSpinBox->value() << "\n";
        out << m_cameraFocusZDoubleSpinBox->value() << "\n";

        out << m_cameraUpXDoubleSpinBox->value() << "\n";
        out << m_cameraUpYDoubleSpinBox->value() << "\n";
        out << m_cameraUpZDoubleSpinBox->value() << "\n";

        out.flush();
        cameraFile.close();

        QFileInfo cameraFileInfo( cameraFileName );
        settings.setValue( "cameraDir", cameraFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QOptimalViewpointExtension::computeViewpointSaliency()
{
    m_method->computeViewpointSaliency( m_obscuranceDirectionsSpinBox->value(), m_viewpointSaliencyDivAreaCheckBox->isChecked() );
}


}
