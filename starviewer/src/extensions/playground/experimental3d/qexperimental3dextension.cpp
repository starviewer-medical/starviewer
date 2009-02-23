#include "qexperimental3dextension.h"

#include <QButtonGroup>
#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTemporaryFile>
#include <QTextStream>

#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"
#include "mathtools.h"
#include "obscurancemainthread.h"
#include "transferfunctionio.h"
#include "vector3.h"
#include "viewpointgenerator.h"
#include "volumereslicer.h"


namespace udg {


QExperimental3DExtension::QExperimental3DExtension( QWidget *parent )
 : QWidget( parent ),
   m_volume( 0 ),
   m_computingObscurance( false ), m_obscuranceMainThread( 0 ), m_obscurance( 0 )
{
    setupUi( this );

    createConnections();

    QButtonGroup *bestViewpointsRadioButtons = new QButtonGroup( this );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsNRadioButton );
    bestViewpointsRadioButtons->addButton( m_computeBestViewsThresholdRadioButton );
}


QExperimental3DExtension::~QExperimental3DExtension()
{
    if ( m_computingObscurance )
    {
        m_obscuranceMainThread->stop();
        m_obscuranceMainThread->wait();
    }

    delete m_obscuranceMainThread;
    delete m_obscurance;
}


void QExperimental3DExtension::setInput( Volume *input )
{
    m_viewer->setInput( input );
    m_volume = m_viewer->getVolume();

    unsigned short max = m_volume->getRangeMax();
    m_transferFunctionEditor->setRange( 0.0, max );
    TransferFunction defaultTransferFunction;
    defaultTransferFunction.addPoint( 0, QColor( 0, 0, 0, 0 ) );
    defaultTransferFunction.addPoint( max, QColor( 255, 255, 255, 255 ) );
    m_transferFunctionEditor->setTransferFunction( defaultTransferFunction );

    doVisualization();
}


void QExperimental3DExtension::createConnections()
{
    // visualització
    connect( m_backgroundColorPushButton, SIGNAL( clicked() ), SLOT( chooseBackgroundColor() ) );
    connect( m_diffuseCheckBox, SIGNAL( toggled(bool) ), SLOT( enableSpecularLighting(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_specularPowerLabel, SLOT( setEnabled(bool) ) );
    connect( m_specularCheckBox, SIGNAL( toggled(bool) ), m_specularPowerDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_contourCheckBox, SIGNAL( toggled(bool) ), m_contourDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_loadTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( loadTransferFunction() ) );
    connect( m_saveTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( saveTransferFunction() ) );
    connect( m_visualizationOkPushButton, SIGNAL( clicked() ), SLOT( doVisualization() ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterLowLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterLowDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterHighLabel, SLOT( setEnabled(bool) ) );
    connect( m_obscuranceCheckBox, SIGNAL( toggled(bool) ), m_obscuranceFilterHighDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), m_vomiFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), m_vomiFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_vomiCheckBox, SIGNAL( toggled(bool) ), SLOT( vomiChecked(bool) ) );
    connect( m_voxelSalienciesCheckBox, SIGNAL( toggled(bool) ), m_voxelSalienciesFactorLabel, SLOT( setEnabled(bool) ) );
    connect( m_voxelSalienciesCheckBox, SIGNAL( toggled(bool) ), m_voxelSalienciesFactorDoubleSpinBox, SLOT( setEnabled(bool) ) );
    connect( m_voxelSalienciesCheckBox, SIGNAL( toggled(bool) ), SLOT( voxelSalienciesChecked(bool) ) );

    // càmera
    connect( m_cameraGetPushButton, SIGNAL( clicked() ), SLOT( getCamera() ) );
    connect( m_cameraSetPushButton, SIGNAL( clicked() ), SLOT( setCamera() ) );
    connect( m_cameraLoadPushButton, SIGNAL( clicked() ), SLOT( loadCamera() ) );
    connect( m_cameraSavePushButton, SIGNAL( clicked() ), SLOT( saveCamera() ) );
    connect( m_cameraViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setNumberOfViewpoints(int) ) );
    connect( m_viewpointPushButton, SIGNAL( clicked() ), SLOT( setViewpoint() ) );

    // obscurances
    connect( m_obscurancePushButton, SIGNAL( clicked() ), SLOT( computeCancelObscurance() ) );
    connect( m_obscuranceLoadPushButton, SIGNAL( clicked() ), SLOT( loadObscurance() ) );
    connect( m_obscuranceSavePushButton, SIGNAL( clicked() ), SLOT( saveObscurance() ) );

    // SMI
    connect( m_smiViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setNumberOfSmiViewpoints(int) ) );
    connect( m_smiDefaultAxisCheckBox, SIGNAL( toggled(bool) ), m_smiViewpointDistributionWidget, SLOT( setDisabled(bool) ) );
    connect( m_smiDefaultAxisCheckBox, SIGNAL( toggled(bool) ), m_smiViewpointLabel, SLOT( setDisabled(bool) ) );
    connect( m_smiDefaultAxisCheckBox, SIGNAL( toggled(bool) ), m_smiViewpointSpinBox, SLOT( setDisabled(bool) ) );
    connect( m_smiPushButton, SIGNAL( clicked() ), SLOT( computeSmi() ) );
    connect( m_sliceUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( computeSliceUnstabilities() ) );
    connect( m_pmiPushButton, SIGNAL( clicked() ), SLOT( computePmi() ) );
    connect( m_propertySalienciesPushButton, SIGNAL( clicked() ), SLOT( computePropertySaliencies() ) );

    // VMI
    connect( m_computeVmiPushButton, SIGNAL( clicked() ), SLOT( computeSelectedVmi() ) );
    connect( m_loadVmiPushButton, SIGNAL( clicked() ), SLOT( loadVmi() ) );
    connect( m_saveVmiPushButton, SIGNAL( clicked() ), SLOT( saveVmi() ) );
    connect( m_loadViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( loadViewpointUnstabilities() ) );
    connect( m_saveViewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( saveViewpointUnstabilities() ) );
    connect( m_loadBestViewsPushButton, SIGNAL( clicked() ), SLOT( loadBestViews() ) );
    connect( m_saveBestViewsPushButton, SIGNAL( clicked() ), SLOT( saveBestViews() ) );
    connect( m_loadVomiPushButton, SIGNAL( clicked() ), SLOT( loadVomi() ) );
    connect( m_saveVomiPushButton, SIGNAL( clicked() ), SLOT( saveVomi() ) );
    connect( m_loadVoxelSalienciesPushButton, SIGNAL( clicked() ), SLOT( loadVoxelSaliencies() ) );
    connect( m_saveVoxelSalienciesPushButton, SIGNAL( clicked() ), SLOT( saveVoxelSaliencies() ) );

    // Program
    connect( m_loadAndRunProgramPushButton, SIGNAL( clicked() ), SLOT( loadAndRunProgram() ) );
}


void QExperimental3DExtension::loadTransferFunction( const QString &fileName )
{
    TransferFunction *transferFunction;

    if ( fileName.endsWith( ".xml" ) ) transferFunction = TransferFunctionIO::fromXmlFile( fileName );
    else transferFunction = TransferFunctionIO::fromFile( fileName );

    m_transferFunctionEditor->setTransferFunction( *transferFunction );
    delete transferFunction;
}


void QExperimental3DExtension::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor( m_viewer->getBackgroundColor(), this );
    if ( color.isValid() ) m_viewer->setBackgroundColor( color );
}


void QExperimental3DExtension::enableSpecularLighting( bool on )
{
    m_specularCheckBox->setEnabled( on );

    if ( on )
    {
        m_specularPowerLabel->setEnabled( m_specularCheckBox->isChecked() );
        m_specularPowerDoubleSpinBox->setEnabled( m_specularCheckBox->isChecked() );
    }
    else
    {
        m_specularPowerLabel->setEnabled( false );
        m_specularPowerDoubleSpinBox->setEnabled( false );
    }
}


void QExperimental3DExtension::loadTransferFunction()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString transferFunctionDir = settings.value( "transferFunctionDir", QString() ).toString();
    QString transferFunctionFileName = QFileDialog::getOpenFileName( this, tr("Load transfer function"), transferFunctionDir,
                                                                     tr("Transfer function files (*.tf);;XML files (*.xml);;All files (*)") );

    if ( !transferFunctionFileName.isNull() )
    {
        loadTransferFunction( transferFunctionFileName );

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "transferFunctionDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveTransferFunction()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString transferFunctionDir = settings.value( "transferFunctionDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save transfer function"), transferFunctionDir,
                            tr("Transfer function files (*.tf);;XML files (*.xml);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "tf" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString transferFunctionFileName = saveDialog.selectedFiles().first();

        if ( transferFunctionFileName.endsWith( ".xml" ) )
            TransferFunctionIO::toXmlFile( transferFunctionFileName, m_transferFunctionEditor->transferFunction() );
        else
            TransferFunctionIO::toFile( transferFunctionFileName, m_transferFunctionEditor->transferFunction() );

        QFileInfo transferFunctionFileInfo( transferFunctionFileName );
        settings.setValue( "transferFunctionDir", transferFunctionFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::doVisualization()
{
    m_volume->setInterpolation( static_cast<Experimental3DVolume::Interpolation>( m_interpolationComboBox->currentIndex() ) );
    m_volume->setGradientEstimator( static_cast<Experimental3DVolume::GradientEstimator>( m_gradientEstimatorComboBox->currentIndex() ) );
    if ( m_diffuseCheckBox->isChecked() ) m_viewer->updateShadingTable();
    if ( m_vomiCheckBox->isChecked() )
    {
        m_volume->renderVomi( m_vomi, m_maximumVomi, m_vomiFactorDoubleSpinBox->value(), m_diffuseCheckBox->isChecked() );
    }
    else if ( m_voxelSalienciesCheckBox->isChecked() )
    {
        m_volume->renderVoxelSaliencies( m_voxelSaliencies, m_maximumSaliency, m_voxelSalienciesFactorDoubleSpinBox->value(), m_diffuseCheckBox->isChecked() );
    }
    else
    {
        m_volume->setLighting( m_diffuseCheckBox->isChecked(), m_specularCheckBox->isChecked(), m_specularPowerDoubleSpinBox->value() );
        m_volume->setContour( m_contourCheckBox->isChecked(), m_contourDoubleSpinBox->value() );
        m_volume->setObscurance( m_obscuranceCheckBox->isChecked(), m_obscurance, m_obscuranceFactorDoubleSpinBox->value(),
                                 m_obscuranceFilterLowDoubleSpinBox->value(), m_obscuranceFilterHighDoubleSpinBox->value() );
    }
    m_volume->setTransferFunction( m_transferFunctionEditor->transferFunction() );
    m_viewer->render();
}


void QExperimental3DExtension::getCamera()
{
    Vector3 position, focus, up;

    m_viewer->getCamera( position, focus, up );

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


void QExperimental3DExtension::setCamera()
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

    m_viewer->setCamera( position, focus, up );
}


void QExperimental3DExtension::loadCamera()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString cameraDir = settings.value( "cameraDir", QString() ).toString();
    QString cameraFileName = QFileDialog::getOpenFileName( this, tr("Load camera parameters"), cameraDir,
                                                           tr("Camera files (*.cam);;All files (*)") );

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

        setCamera();
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveCamera()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

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


void QExperimental3DExtension::setNumberOfViewpoints( int numberOfViewpoints )
{
    m_viewpointSpinBox->setMaximum( numberOfViewpoints );
}


void QExperimental3DExtension::setViewpoint()
{
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;

    if ( m_cameraViewpointDistributionWidget->isUniform() )
    {
        switch ( m_cameraViewpointDistributionWidget->numberOfViewpoints() )
        {
            case 4: viewpointGenerator.setToUniform4( distance ); break;
            case 6: viewpointGenerator.setToUniform6( distance ); break;
            case 8: viewpointGenerator.setToUniform8( distance ); break;
            case 12: viewpointGenerator.setToUniform12( distance ); break;
            case 20: viewpointGenerator.setToUniform20( distance ); break;
            default: Q_ASSERT_X( false, "setViewpoint", qPrintable( QString( "Nombre de punts de vista uniformes incorrecte: %1" ).arg( m_cameraViewpointDistributionWidget->numberOfViewpoints() ) ) );
        }
    }
    else viewpointGenerator.setToQuasiUniform( m_cameraViewpointDistributionWidget->recursionLevel(), distance );

    setViewpoint( viewpointGenerator.viewpoint( m_viewpointSpinBox->value() - 1 ) );
}


void QExperimental3DExtension::setViewpoint( const Vector3 &viewpoint )
{
    Vector3 up( 0.0, 1.0, 0.0 );
    Vector3 position = viewpoint;
    if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );

    m_viewer->setCamera( viewpoint, Vector3(), up );
}


void QExperimental3DExtension::computeCancelObscurance()
{
    if ( !m_computingObscurance )
    {
        m_computingObscurance = true;

        if ( m_obscuranceCheckBox->isChecked() )
        {
            m_obscuranceCheckBox->setChecked( false );
            m_volume->setObscurance( false, 0, 1.0, 0.0, 1.0 );
            m_viewer->render();
        }

        m_obscuranceCheckBox->setEnabled( false );

        delete m_obscuranceMainThread;          // esborrem el thread d'abans
        delete m_obscurance; m_obscurance = 0;  // esborrem l'obscurança d'abans

        int numberOfDirections;
        if ( m_obscuranceViewpointDistributionWidget->isUniform() )
            numberOfDirections = -1 * m_obscuranceViewpointDistributionWidget->numberOfViewpoints();
        else
            numberOfDirections = m_obscuranceViewpointDistributionWidget->recursionLevel();

        m_obscuranceMainThread = new ObscuranceMainThread( numberOfDirections,
                                                           m_obscuranceMaximumDistanceDoubleSpinBox->value(),
                                                           static_cast<ObscuranceMainThread::Function>( m_obscuranceFunctionComboBox->currentIndex() ),
                                                           static_cast<ObscuranceMainThread::Variant>( m_obscuranceVariantComboBox->currentIndex() ),
                                                           m_obscuranceDoublePrecisionRadioButton->isChecked(),
                                                           this );
        m_obscuranceMainThread->setVolume( m_volume->getVolume() );
        m_obscuranceMainThread->setTransferFunction( m_transferFunctionEditor->transferFunction() );

        m_obscurancePushButton->setText( tr("Cancel obscurance") );
        m_obscuranceProgressBar->setValue( 0 );
        connect( m_obscuranceMainThread, SIGNAL( progress(int) ), m_obscuranceProgressBar, SLOT( setValue(int) ) );
        connect( m_obscuranceMainThread, SIGNAL( computed() ), SLOT( endComputeObscurance() ) );
        m_obscuranceLoadPushButton->setEnabled( false );
        m_obscuranceSavePushButton->setEnabled( false );

        m_obscuranceMainThread->start();
    }
    else
    {
        m_obscuranceMainThread->stop();
        connect( m_obscuranceMainThread, SIGNAL( finished() ), SLOT( endCancelObscurance() ) );

        m_obscurancePushButton->setText( tr("Cancelling obscurance...") );
        m_obscurancePushButton->setEnabled( false );
    }
}


void QExperimental3DExtension::endComputeObscurance()
{
    m_computingObscurance = false;

    m_obscurance = m_obscuranceMainThread->getObscurance();
    m_obscurancePushButton->setText( tr("Compute obscurance") );
    m_obscuranceLoadPushButton->setEnabled( true );
    m_obscuranceSavePushButton->setEnabled( true );
    m_obscuranceCheckBox->setEnabled( true );
}


void QExperimental3DExtension::endCancelObscurance()
{
    m_computingObscurance = false;

    m_obscurancePushButton->setText( tr("Compute obscurance") );
    m_obscurancePushButton->setEnabled( true );
    m_obscuranceLoadPushButton->setEnabled( true );
}


void QExperimental3DExtension::loadObscurance()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString obscuranceDir = settings.value( "obscuranceDir", QString() ).toString();
    QString obscuranceFileName = QFileDialog::getOpenFileName( this, tr("Load obscurance"), obscuranceDir,
                                                               tr("Data files (*.dat);;All files (*)") );

    if ( !obscuranceFileName.isNull() )
    {
        if ( m_obscuranceCheckBox->isChecked() )
        {
            m_obscuranceCheckBox->setChecked( false );
            m_volume->setObscurance( false, 0, 1.0, 0.0, 1.0 );
            m_viewer->render();
        }

        m_obscuranceCheckBox->setEnabled( false );

        delete m_obscurance;

        m_obscurance = new Obscurance( m_volume->getSize(), ObscuranceMainThread::hasColor( static_cast<ObscuranceMainThread::Variant>( m_obscuranceVariantComboBox->currentIndex() ) ), m_obscuranceDoublePrecisionRadioButton->isChecked() );
        bool ok = m_obscurance->load( obscuranceFileName );

        if ( ok )
        {
            m_obscuranceSavePushButton->setEnabled( true );
            m_obscuranceCheckBox->setEnabled( true );
        }
        else
        {
            m_obscuranceSavePushButton->setEnabled( false );
            QMessageBox::warning( this, tr("Can't load obscurance"), QString( tr("Can't load obscurance from file ") ) + obscuranceFileName );
        }

        QFileInfo obscuranceFileInfo( obscuranceFileName );
        settings.setValue( "obscuranceDir", obscuranceFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveObscurance()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString obscuranceDir = settings.value( "obscuranceDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save obscurance"), obscuranceDir, tr("Data files (*.dat);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "dat" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString obscuranceFileName = saveDialog.selectedFiles().first();

        if ( !m_obscurance->save( obscuranceFileName ) )
            QMessageBox::warning( this, tr("Can't save obscurance"), QString( tr("Can't save obscurance to file ") ) + obscuranceFileName );

        QFileInfo obscuranceFileInfo( obscuranceFileName );
        settings.setValue( "obscuranceDir", obscuranceFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::setNumberOfSmiViewpoints( int numberOfViewpoints )
{
    m_smiViewpointSpinBox->setMaximum( numberOfViewpoints );
}


void QExperimental3DExtension::computeSmi()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computeSmi();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computeSmi();
        }
    }
}


void QExperimental3DExtension::computeSliceUnstabilities()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computeSliceUnstabilities();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computeSliceUnstabilities();
        }
    }
}


void QExperimental3DExtension::computePmi()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computePmi();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computePmi();
        }
    }
}


void QExperimental3DExtension::computePropertySaliencies()
{
    if ( m_smiDefaultAxisCheckBox->isChecked() )
    {
        VolumeReslicer volumeReslicer;
        volumeReslicer.setInput( m_volume->getImage() );
        volumeReslicer.noReslice();
        volumeReslicer.computePropertySaliencies();
    }
    else
    {
        QVector<Vector3> viewpoints = m_smiViewpointDistributionWidget->viewpoints();
        int i0, i1;

        if ( m_smiViewpointSpinBox->value() == 0 )  // tots
        {
            i0 = 0; i1 = viewpoints.size();
        }
        else    // un en concret
        {
            i0 = m_smiViewpointSpinBox->value() - 1; i1 = i0 + 1;
        }

        for ( int i = i0; i < i1; i++ )
        {
            const Vector3 &viewpoint = viewpoints.at( i );

            VolumeReslicer volumeReslicer( i + 1 );
            volumeReslicer.setInput( m_volume->getImage() );

            Vector3 position( viewpoint );
            Vector3 up( 0.0, 1.0, 0.0 );
            if ( qAbs( position.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );
            volumeReslicer.setViewpoint( viewpoint, up );

            volumeReslicer.setSpacing( 1.0, 1.0, 1.0 );
            volumeReslicer.reslice();

            volumeReslicer.computePropertySaliencies();
        }
    }
}


void QExperimental3DExtension::computeSelectedVmi()
{
    // Què ha demanat l'usuari
    bool computeVmi = m_computeVmiCheckBox->isChecked();
    bool computeViewpointUnstabilities = m_computeViewpointUnstabilitiesCheckBox->isChecked();
    bool computeBestViews = m_computeBestViewsCheckBox->isChecked();
    bool computeVomi = m_computeVomiCheckBox->isChecked();
    bool computeVoxelSaliencies = m_computeVoxelSalienciesCheckBox->isChecked();

    // Si no hi ha res a calcular marxem
    if ( !computeVmi && !computeViewpointUnstabilities && !computeBestViews && !computeVomi && !computeVoxelSaliencies ) return;

    setCursor( QCursor( Qt::WaitCursor ) );

    // Obtenir direccions
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;
    {
        if ( m_vmiViewpointDistributionWidget->isUniform() )
        {
            switch ( m_vmiViewpointDistributionWidget->numberOfViewpoints() )
            {
                case 4: viewpointGenerator.setToUniform4( distance ); break;
                case 6: viewpointGenerator.setToUniform6( distance ); break;
                case 8: viewpointGenerator.setToUniform8( distance ); break;
                case 12: viewpointGenerator.setToUniform12( distance ); break;
                case 20: viewpointGenerator.setToUniform20( distance ); break;
                default: Q_ASSERT_X( false, "setViewpoint", qPrintable( QString( "Nombre de punts de vista uniformes incorrecte: %1" ).arg( m_vmiViewpointDistributionWidget->numberOfViewpoints() ) ) );
            }
        }
        else viewpointGenerator.setToQuasiUniform( m_vmiViewpointDistributionWidget->recursionLevel(), distance );
    }

    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();
    unsigned int nObjects = m_volume->getSize();

    // Dependències
    if ( computeBestViews && m_vmi.size() != nViewpoints ) computeVmi = true;

    // Inicialitzar progrés
    int nSteps = 3; // ray casting (p(O|V)), p(V), p(O)
    if ( computeVmi || computeViewpointUnstabilities ) nSteps++;    // VMI + viewpoint unstabilities
    if ( computeBestViews ) nSteps++;   // best views
    if ( computeVomi || computeVoxelSaliencies ) nSteps ++; //VoMI + voxel saliencies
    int step = 0;
    {
        m_vmiProgressBar->setValue( 0 );
        m_vmiProgressBar->repaint();
        m_vmiTotalProgressBar->setMaximum( nSteps );
        m_vmiTotalProgressBar->setValue( step );
        m_vmiTotalProgressBar->repaint();
    }

    m_volume->startVmiMode();

    QVector<float> viewProbabilities( nViewpoints );    // vector p(V), inicialitzat a 0
    QVector<float> objectProbabilities( nObjects );     // vector p(O), inicialitzat a 0
    QVector<QTemporaryFile*> pOvFiles( nViewpoints );   // matriu p(O|V) (cada fitxer una fila p(O|v))
    {
        for ( int i = 0; i < nViewpoints; i++ )
        {
            pOvFiles[i] = new QTemporaryFile( "pOvXXXXXX.tmp" );    // els fitxers temporals es creen al directori de treball

            if ( !pOvFiles[i]->open() )
            {
                DEBUG_LOG( QString( "No s'ha pogut obrir el fitxer: error %1" ).arg( pOvFiles[i]->errorString() ) );
                for ( int j = 0; j < i; j++ ) pOvFiles[j]->close();
                return;
            }
        }
    }

    float totalViewedVolume = 0.0f;

    // p(O|V) (i acumulació de p(V))
    {
        m_vmiProgressBar->setValue( 0 );
        m_vmiProgressBar->repaint();

        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiSecondPass();
            setViewpoint( viewpoints.at( i ) ); // render
            QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass();

            // p(V)
            float viewedVolume = m_volume->viewedVolumeInVmiSecondPass();
            viewProbabilities[i] = viewedVolume;
            totalViewedVolume += viewedVolume;

            // p(O|V)
            pOvFiles[i]->write( reinterpret_cast<const char*>( objectProbabilitiesInView.data() ), objectProbabilitiesInView.size() * sizeof(float) );

            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
            m_vmiProgressBar->repaint();
        }

        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // p(V)
    {
        if ( totalViewedVolume > 0.0f )
        {
            m_vmiProgressBar->setValue( 0 );

            for ( int i = 0; i < nViewpoints; i++ )
            {
                viewProbabilities[i] /= totalViewedVolume;
                Q_ASSERT( viewProbabilities.at( i ) == viewProbabilities.at( i ) );
                DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i + 1 ).arg( viewProbabilities.at( i ) ) );
                m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
                m_vmiProgressBar->repaint();
            }
        }

        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();
    }

    // p(O)
    {
        m_vmiProgressBar->setValue( 0 );

        float *objectProbabilitiesInView = new float[nObjects]; // vector p(O|v)

        for ( int i = 0; i < nViewpoints; i++ )
        {
            pOvFiles[i]->reset();   // reset per tornar al principi
            pOvFiles[i]->read( reinterpret_cast<char*>( objectProbabilitiesInView ), nObjects * sizeof(float) );    // llegim...
            pOvFiles[i]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

            for ( unsigned int j = 0; j < nObjects; j++ )
            {
                objectProbabilities[j] += viewProbabilities.at( i ) * objectProbabilitiesInView[j];
            }

            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
            m_vmiProgressBar->repaint();
        }

        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();

        for ( unsigned int i = 0; i < nObjects; i++ )
        {
            Q_ASSERT( objectProbabilities.at( i ) == objectProbabilities.at( i ) );
            //DEBUG_LOG( QString( "p(o%1) = %2" ).arg( i ).arg( objectProbabilities.at( i ) ) );
        }

        delete[] objectProbabilitiesInView;
    }

    // VMI
    if ( computeVmi || computeViewpointUnstabilities )
    {
        if ( computeVmi ) m_vmi.resize( nViewpoints );
        if ( computeViewpointUnstabilities ) m_viewpointUnstabilities.resize( nViewpoints );

        m_vmiProgressBar->setValue( 0 );

        QVector<float> objectProbabilitiesInView( nObjects );   // vector p(O|vi)

        for ( int i = 0; i < nViewpoints; i++ )
        {
            pOvFiles[i]->reset();   // reset per tornar al principi
            pOvFiles[i]->read( reinterpret_cast<char*>( objectProbabilitiesInView.data() ), nObjects * sizeof(float) ); // llegim...
            pOvFiles[i]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

            if ( computeVmi )
            {
                float vmi = InformationTheory<float>::kullbackLeiblerDivergence( objectProbabilitiesInView, objectProbabilities );
                Q_ASSERT( vmi == vmi );
                m_vmi[i] = vmi;
                DEBUG_LOG( QString( "VMI(v%1) = %2" ).arg( i + 1 ).arg( vmi ) );
            }

            if ( computeViewpointUnstabilities )
            {
                float pvi = viewProbabilities.at( i );  // p(vi)

                QVector<int> neighbours = viewpointGenerator.neighbours( i );
                int nNeighbours = neighbours.size();
                float viewpointUnstability = 0.0f;
                QVector<float> objectProbabilitiesInNeighbour( nObjects );  // vector p(O|vj)

                for ( int j = 0; j < nNeighbours; j++ )
                {
                    int neighbour = neighbours.at( j );
                    float pvj = viewProbabilities.at( neighbour );  // p(vj)
                    float pvij = pvi + pvj; // p(v̂)

                    if ( pvij == 0.0f ) continue;

                    pOvFiles[neighbour]->reset();   // reset per tornar al principi
                    pOvFiles[neighbour]->read( reinterpret_cast<char*>( objectProbabilitiesInNeighbour.data() ), nObjects * sizeof(float) );    // llegim...
                    pOvFiles[neighbour]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

                    float viewpointDissimilarity = InformationTheory<float>::jensenShannonDivergence( pvi / pvij, pvj / pvij, objectProbabilitiesInView, objectProbabilitiesInNeighbour );
                    viewpointUnstability += viewpointDissimilarity;
                }

                viewpointUnstability /= nNeighbours;
                m_viewpointUnstabilities[i] = viewpointUnstability;
            }

            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
            m_vmiProgressBar->repaint();
        }

        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();

        if ( computeVmi ) m_saveVmiPushButton->setEnabled( true );
        if ( computeViewpointUnstabilities ) m_saveViewpointUnstabilitiesPushButton->setEnabled( true );
    }

    if ( computeBestViews )
    {
        m_bestViews.clear();

        // millor vista
        float minVmi = m_vmi.at( 0 );
        int minVmiIndex = 0;

        for ( int i = 1; i < nViewpoints; i++ )
        {
            float vmi = m_vmi.at( i );

            if ( vmi < minVmi )
            {
                minVmi = vmi;
                minVmiIndex = i;
            }
        }

        m_bestViews << qMakePair( minVmiIndex, viewpoints.at( minVmiIndex ) );

        QList<int> viewpointIndexList;
        for ( int i = 0; i < nViewpoints; i++ ) viewpointIndexList << i;
        viewpointIndexList.removeAt( minVmiIndex );

        float pvv = viewProbabilities.at( minVmiIndex );    // p(v̂)
        QVector<float> pOvv( nObjects );    // vector p(O|v̂)
        pOvFiles[minVmiIndex]->reset();     // reset per tornar al principi
        pOvFiles[minVmiIndex]->read( reinterpret_cast<char*>( pOvv.data() ), nObjects * sizeof(float) );    // llegim...
        pOvFiles[minVmiIndex]->reset();     // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

        // límits
        bool limitN = m_computeBestViewsNRadioButton->isChecked();
        int n = qMin( m_computeBestViewsNSpinBox->value(), nViewpoints );
        bool limitThreshold = m_computeBestViewsThresholdRadioButton->isChecked();
        float threshold = m_computeBestViewsThresholdDoubleSpinBox->value();
        float IVO = 0.0f;       // I(V,O)
        for ( int i = 0; i < nViewpoints; i++ ) IVO += viewProbabilities.at( i ) * m_vmi.at( i );   // calcular I(V,O)
        float IvvO = minVmi;    // I(v̂,O)

        if ( limitN )
        {
            DEBUG_LOG( QString( "límit %1 vistes" ).arg( n ) );
        }
        if ( limitThreshold )
        {
            DEBUG_LOG( QString( "límit llindar %1" ).arg( threshold ) );
        }

        DEBUG_LOG( QString( "I(V,O) = %1" ).arg( IVO ) );
        DEBUG_LOG( "Millors vistes:" );
        DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂,O) = %4; I(v̂,O)/I(V,O) = %5" ).arg( 0 ).arg( minVmiIndex + 1 ).arg( viewpoints.at( minVmiIndex ).toString() ).arg( IvvO ).arg( IvvO / IVO ) );



        if ( limitN ) m_vmiProgressBar->setValue( 100 / n );

        if ( limitThreshold )
        {
            m_vmiProgressBar->setValue( 0 );
            m_vmiProgressBar->setMaximum( 0 );
        }

        m_vmiProgressBar->repaint();

        float *pOvi = new float[nObjects];

        while ( ( limitN && m_bestViews.size() < n ) || ( limitThreshold && IvvO / IVO > threshold ) )
        {
            int nRemainingViews = viewpointIndexList.size();
            float pvvMin = 0.0f;
            QVector<float> pOvvMin;
            float IvvOMin = 0.0f;
            int iMin = 0;

            for ( int i = 0; i < nRemainingViews; i++ )
            {
                int viewIndex = viewpointIndexList.at( i );
                float pvi = viewProbabilities.at( viewIndex );
                pOvFiles[viewIndex]->reset();   // reset per tornar al principi
                pOvFiles[viewIndex]->read( reinterpret_cast<char*>( pOvi ), nObjects * sizeof(float) ); // llegim...
                pOvFiles[viewIndex]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)

                float pvvi = pvv + pvi;         // p(v̂) afegint aquesta vista
                QVector<float> pOvvi( pOvv );   // vector p(O|v̂) afegint aquesta vista
                for ( unsigned int j = 0; j < nObjects; j++ ) pOvvi[j] = ( pvv * pOvv.at( j ) + pvi * pOvi[j] ) / pvvi;
                float IvviO = InformationTheory<float>::kullbackLeiblerDivergence( pOvvi, objectProbabilities );    // I(v̂,O) afegint aquesta vista

                if ( i == 0 || IvviO < IvvOMin )
                {
                    pvvMin = pvvi;
                    pOvvMin = pOvvi;
                    IvvOMin = IvviO;
                    iMin = i;
                }
            }

            pvv = pvvMin;
            pOvv = pOvvMin;
            IvvO = IvvOMin;
            int viewIndex = viewpointIndexList.takeAt( iMin );
            m_bestViews << qMakePair( viewIndex, viewpoints.at( viewIndex ) );
            DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂,O) = %4; I(v̂,O)/I(V,O) = %5" ).arg( m_bestViews.size() - 1 ).arg( viewIndex + 1 ).arg( viewpoints.at( viewIndex ).toString() ).arg( IvvO ).arg( IvvO / IVO ) );

            if ( limitN ) m_vmiProgressBar->setValue( 100 * m_bestViews.size() / n );
            m_vmiProgressBar->repaint();
        }

        delete[] pOvi;

        if ( limitThreshold )
        {
            m_vmiProgressBar->setMaximum( 100 );
            m_vmiProgressBar->setValue( 100 );
            m_vmiProgressBar->repaint();
        }

        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();

        m_saveBestViewsPushButton->setEnabled( true );
    }

    // VoMI + voxel saliencies
    if ( computeVomi || computeVoxelSaliencies )
    {
        if ( computeVomi )
        {
            m_vomi.resize( nObjects );
            m_maximumVomi = 0.0f;
        }
        if ( computeVoxelSaliencies )
        {
            m_voxelSaliencies.resize( nObjects );
            m_maximumSaliency = 0.0f;
        }

        /*
         * Fer un peek per llegir el fitxer sencer és costós. Podem aprofitar que sabem com estan distribuïts els veïns per acotar el tros de fitxer que cal llegir de manera que el que valor que busquem sigui a dins.
         * N'hi haurà prou llegint la llesca actual, l'anterior i la següent.
         */
        int *dimensions = m_volume->getImage()->GetDimensions();
        int dimX = dimensions[0], dimY = dimensions[1], dimZ = dimensions[2], dimXY = dimX * dimY, dimXY3 = dimXY * 3;
        qint64 sizeToRead = dimXY3 * sizeof(float), sizeToReadOnEdge = dimXY * 2 * sizeof(float);
        float **pOV = new float*[nViewpoints];  // p(O|V) (un tros de la matriu cada vegada)
        for ( int i = 0; i < nViewpoints; i++ ) pOV[i] = new float[dimXY3];
        QVector<float> pVoi( nViewpoints ); // p(V|oi)
        QVector<float> pVoj( nViewpoints ); // p(V|oj)

        m_vmiProgressBar->setValue( 0 );

        // iterem pel volum en l'ordre dels vòxels
        for ( int z = 0, i = 0; z < dimZ; z++ )
        {
            DEBUG_LOG( QString( "llesca %1/%2" ).arg( z + 1 ).arg( dimZ ) );

            // actualitzem pOV
            for ( int k = 0; k < nViewpoints; k++ )
            {
                // quan z == 0, es salta la primera llesca, que seria z == -1
                if ( z == 0 ) pOvFiles[k]->peek( reinterpret_cast<char*>( &(pOV[k][dimXY]) ), sizeToReadOnEdge );
                else if ( z == dimZ - 1 ) pOvFiles[k]->peek( reinterpret_cast<char*>( pOV[k] ), sizeToReadOnEdge );
                else pOvFiles[k]->peek( reinterpret_cast<char*>( pOV[k] ), sizeToRead );
            }

            int pOvShift = ( z - 1 ) * dimXY;

            for ( int y = 0; y < dimY; y++ )
            {
                for ( int x = 0; x < dimX; x++, i++ )
                {
                    Q_ASSERT( i == x + y * dimX + z * dimXY );

                    float poi = objectProbabilities.at( i );    // p(oi)
                    Q_ASSERT( poi == poi );

                    // p(V|oi)
                    if ( poi == 0.0 ) pVoi.fill( 0.0f );    // si p(oi) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oi) ha de ser tot zeros
                    else for ( int k = 0; k < nViewpoints; k++ ) pVoi[k] = viewProbabilities.at( k ) * pOV[k][i - pOvShift] / poi;

                    if ( computeVomi )
                    {
                        float vomi = InformationTheory<float>::kullbackLeiblerDivergence( pVoi, viewProbabilities );
                        Q_ASSERT( vomi == vomi );
                        m_vomi[i] = vomi;
                        if ( vomi > m_maximumVomi ) m_maximumVomi = vomi;
                    }

                    if ( computeVoxelSaliencies )
                    {
//                        int neighbours[6] = { x - 1 + y * dimX + z * dimXY, x + 1 + y * dimX + z * dimXY,
//                                              x + ( y - 1 ) * dimX + z * dimXY, x + ( y + 1 ) * dimX + z * dimXY,
//                                              x + y * dimX + ( z - 1 ) * dimXY, x + y * dimX + ( z + 1 ) * dimXY };
//                        bool validNeighbours[6] = { x > 0, x + 1 < dimX,
//                                                    y > 0, y + 1 < dimY,
//                                                    z > 0, z + 1 < dimZ };
                        int neighbours[26] = { x-1 + (y-1) * dimX + (z-1) * dimXY, x-1 + (y-1) * dimX +  z    * dimXY, x-1 + (y-1) * dimX + (z+1) * dimXY,
                                               x-1 +  y    * dimX + (z-1) * dimXY, x-1 +  y    * dimX +  z    * dimXY, x-1 +  y    * dimX + (z+1) * dimXY,
                                               x-1 + (y+1) * dimX + (z-1) * dimXY, x-1 + (y+1) * dimX +  z    * dimXY, x-1 + (y+1) * dimX + (z+1) * dimXY,
                                               x   + (y-1) * dimX + (z-1) * dimXY, x   + (y-1) * dimX +  z    * dimXY, x   + (y-1) * dimX + (z+1) * dimXY,
                                               x   +  y    * dimX + (z-1) * dimXY,                                     x   +  y    * dimX + (z+1) * dimXY,
                                               x   + (y+1) * dimX + (z-1) * dimXY, x   + (y+1) * dimX +  z    * dimXY, x   + (y+1) * dimX + (z+1) * dimXY,
                                               x+1 + (y-1) * dimX + (z-1) * dimXY, x+1 + (y-1) * dimX +  z    * dimXY, x+1 + (y-1) * dimX + (z+1) * dimXY,
                                               x+1 +  y    * dimX + (z-1) * dimXY, x+1 +  y    * dimX +  z    * dimXY, x+1 +  y    * dimX + (z+1) * dimXY,
                                               x+1 + (y+1) * dimX + (z-1) * dimXY, x+1 + (y+1) * dimX +  z    * dimXY, x+1 + (y+1) * dimX + (z+1) * dimXY };
                        bool validNeighbours[26] = {   x > 0    &&   y > 0    &&   z > 0   ,   x > 0    &&   y > 0                 ,   x > 0    &&   y > 0    && z+1 < dimZ,
                                                       x > 0                  &&   z > 0   ,   x > 0                               ,   x > 0                  && z+1 < dimZ,
                                                       x > 0    && y+1 < dimY &&   z > 0   ,   x > 0    && y+1 < dimY              ,   x > 0    && y+1 < dimY && z+1 < dimZ,
                                                                     y > 0    &&   z > 0   ,                 y > 0                 ,                 y > 0    && z+1 < dimZ,
                                                                                   z > 0   ,                                                                     z+1 < dimZ,
                                                                   y+1 < dimY &&   z > 0   ,               y+1 < dimY              ,               y+1 < dimY && z+1 < dimZ,
                                                     x+1 < dimX &&   y > 0    &&   z > 0   , x+1 < dimX &&   y > 0                 , x+1 < dimX &&   y > 0    && z+1 < dimZ,
                                                     x+1 < dimX               &&   z > 0   , x+1 < dimX                            , x+1 < dimX               && z+1 < dimZ,
                                                     x+1 < dimX && y+1 < dimY &&   z > 0   , x+1 < dimX && y+1 < dimY              , x+1 < dimX && y+1 < dimY && z+1 < dimZ };
                        const float SQRT_1_2 = 1.0f / sqrt( 2.0f ), SQRT_1_3 = 1.0f / sqrt( 3.0f );
                        float weights[26] = { SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                              SQRT_1_2,   1.0f  , SQRT_1_2,
                                              SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                              SQRT_1_2,   1.0f  , SQRT_1_2,
                                                1.0f  ,             1.0f  ,
                                              SQRT_1_2,   1.0f  , SQRT_1_2,
                                              SQRT_1_3, SQRT_1_2, SQRT_1_3,
                                              SQRT_1_2,   1.0f  , SQRT_1_2,
                                              SQRT_1_3, SQRT_1_2, SQRT_1_3 };

                        float saliency = 0.0f;
                        float totalWeight = 0.0f;

                        // iterem pels veïns
                        for ( int j = 0; j < 26; j++ )
                        {
                            if ( !validNeighbours[j] ) continue;

                            totalWeight += weights[j];

                            float poj = objectProbabilities.at( neighbours[j] );    // p(oj)
                            Q_ASSERT( poj == poj );
                            float poij = poi + poj; // p(ô)

                            if ( poij == 0.0f ) continue;

                            // p(V|oj)
                            if ( poj == 0.0f ) pVoj.fill( 0.0f );   // si p(oj) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oj) ha de ser tot zeros
                            else for ( int k = 0; k < nViewpoints; k++ ) pVoj[k] = viewProbabilities.at( k ) * pOV[k][neighbours[j] - pOvShift] / poj;

                            float s = weights[j] * InformationTheory<float>::jensenShannonDivergence( poi / poij, poj / poij, pVoi, pVoj );
                            Q_ASSERT( s == s );
                            saliency += s;
                        }

                        saliency /= totalWeight;
                        m_voxelSaliencies[i] = saliency;
                        if ( saliency > m_maximumSaliency ) m_maximumSaliency = saliency;
                    }
                }
            }

            if ( z > 0 )
            {
                // avancem una llesca en tots els fitxers
                for ( int k = 0; k < nViewpoints; k++ ) pOvFiles[k]->read( reinterpret_cast<char*>( pOV[k] ), dimXY * sizeof(float) );
            }

            m_vmiProgressBar->setValue( 100 * ( z + 1 ) / dimZ );
            m_vmiProgressBar->repaint();
        }

        m_vmiTotalProgressBar->setValue( ++step );
        m_vmiTotalProgressBar->repaint();

        DEBUG_LOG( "esborrem pOV" );
        for ( int i = 0; i < nViewpoints; i++ ) delete[] pOV[i];
        delete[] pOV;

        if ( computeVomi )
        {
            m_vomiCheckBox->setEnabled( true );
            m_saveVomiPushButton->setEnabled( true );
        }

        if ( computeVoxelSaliencies )
        {
            m_voxelSalienciesCheckBox->setEnabled( true );
            m_saveVoxelSalienciesPushButton->setEnabled( true );
        }
    }

    DEBUG_LOG( "destruïm els fitxers temporals" );
    for ( int i = 0; i < nViewpoints; i++ )
    {
        pOvFiles[i]->close();
        delete pOvFiles[i];
    }

    doVisualization();
    m_viewer->setCamera( position, focus, up );

    DEBUG_LOG( "fi" );

    setCursor( QCursor( Qt::ArrowCursor ) );
}


void QExperimental3DExtension::loadVmi()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString vmiDir = settings.value( "vmiDir", QString() ).toString();
    QString vmiFileName = QFileDialog::getOpenFileName( this, tr("Load VMI"), vmiDir, tr("Data files (*.dat);;All files (*)") );

    if ( !vmiFileName.isNull() )
    {
        QFile vmiFile( vmiFileName );

        if ( !vmiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + vmiFileName );
            QMessageBox::warning( this, tr("Can't load VMI"), QString( tr("Can't load VMI from file ") ) + vmiFileName );
            return;
        }

        m_vmi.clear();

        QDataStream in( &vmiFile );

        while ( !in.atEnd() )
        {
            float vmi;
            in >> vmi;
            m_vmi << vmi;
        }

        vmiFile.close();

        QFileInfo vmiFileInfo( vmiFileName );
        settings.setValue( "vmiDir", vmiFileInfo.absolutePath() );

        m_saveVmiPushButton->setEnabled( true );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveVmi()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString vmiDir = settings.value( "vmiDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save VMI"), vmiDir, tr("Data files (*.dat);;Text files (*.txt);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "dat" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString vmiFileName = saveDialog.selectedFiles().first();
        bool saveAsText = vmiFileName.endsWith( ".txt" );
        QFile vmiFile( vmiFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !vmiFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + vmiFileName );
            QMessageBox::warning( this, tr("Can't save VMI"), QString( tr("Can't save VMI to file ") ) + vmiFileName );
            return;
        }

        int nViewpoints = m_vmi.size();

        if ( saveAsText )
        {
            QTextStream out( &vmiFile );
            for ( int i = 0; i < nViewpoints; i++ ) out << "VMI(v" << i + 1 << ") = " << m_vmi.at( i ) << "\n";
        }
        else
        {
            QDataStream out( &vmiFile );
            for ( int i = 0; i < nViewpoints; i++ ) out << m_vmi.at( i );
        }

        vmiFile.close();

        QFileInfo vmiFileInfo( vmiFileName );
        settings.setValue( "vmiDir", vmiFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::loadViewpointUnstabilities()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString viewpointUnstabilitiesDir = settings.value( "viewpointUnstabilitiesDir", QString() ).toString();
    QString viewpointUnstabilitiesFileName = QFileDialog::getOpenFileName( this, tr("Load viewpoint unstabilities"), viewpointUnstabilitiesDir, tr("Data files (*.dat);;All files (*)") );

    if ( !viewpointUnstabilitiesFileName.isNull() )
    {
        QFile viewpointUnstabilitiesFile( viewpointUnstabilitiesFileName );

        if ( !viewpointUnstabilitiesFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + viewpointUnstabilitiesFileName );
            QMessageBox::warning( this, tr("Can't load viewpoint unstabilities"), QString( tr("Can't load viewpoint unstabilities from file ") ) + viewpointUnstabilitiesFileName );
            return;
        }

        m_viewpointUnstabilities.clear();

        QDataStream in( &viewpointUnstabilitiesFile );

        while ( !in.atEnd() )
        {
            float viewpointUnstabilities;
            in >> viewpointUnstabilities;
            m_viewpointUnstabilities << viewpointUnstabilities;
        }

        viewpointUnstabilitiesFile.close();

        QFileInfo viewpointUnstabilitiesFileInfo( viewpointUnstabilitiesFileName );
        settings.setValue( "viewpointUnstabilitiesDir", viewpointUnstabilitiesFileInfo.absolutePath() );

        m_saveViewpointUnstabilitiesPushButton->setEnabled( true );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveViewpointUnstabilities()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString viewpointUnstabilitiesDir = settings.value( "viewpointUnstabilitiesDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save viewpoint unstabilities"), viewpointUnstabilitiesDir, tr("Data files (*.dat);;Text files (*.txt);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "dat" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString viewpointUnstabilitiesFileName = saveDialog.selectedFiles().first();
        bool saveAsText = viewpointUnstabilitiesFileName.endsWith( ".txt" );
        QFile viewpointUnstabilitiesFile( viewpointUnstabilitiesFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !viewpointUnstabilitiesFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + viewpointUnstabilitiesFileName );
            QMessageBox::warning( this, tr("Can't save viewpoint unstabilities"), QString( tr("Can't save viewpoint unstabilities to file ") ) + viewpointUnstabilitiesFileName );
            return;
        }

        int nViewpoints = m_viewpointUnstabilities.size();

        if ( saveAsText )
        {
            QTextStream out( &viewpointUnstabilitiesFile );
            for ( int i = 0; i < nViewpoints; i++ ) out << "U(v" << i + 1 << ") = " << m_viewpointUnstabilities.at( i ) << "\n";
        }
        else
        {
            QDataStream out( &viewpointUnstabilitiesFile );
            for ( int i = 0; i < nViewpoints; i++ ) out << m_viewpointUnstabilities.at( i );
        }

        viewpointUnstabilitiesFile.close();

        QFileInfo viewpointUnstabilitiesFileInfo( viewpointUnstabilitiesFileName );
        settings.setValue( "viewpointUnstabilitiesDir", viewpointUnstabilitiesFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::loadBestViews()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString bestViewsDir = settings.value( "bestViewsDir", QString() ).toString();
    QString bestViewsFileName = QFileDialog::getOpenFileName( this, tr("Load best views"), bestViewsDir, tr("Data files (*.dat);;All files (*)") );

    if ( !bestViewsFileName.isNull() )
    {
        QFile bestViewsFile( bestViewsFileName );

        if ( !bestViewsFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + bestViewsFileName );
            QMessageBox::warning( this, tr("Can't load best views"), QString( tr("Can't load best views from file ") ) + bestViewsFileName );
            return;
        }

        m_bestViews.clear();

        QDataStream in( &bestViewsFile );

        while ( !in.atEnd() )
        {
            int i;
            Vector3 v;
            in >> i;
            in >> v.x >> v.y >> v.z;
            m_bestViews << qMakePair( i, v );
        }

        bestViewsFile.close();

        QFileInfo bestViewsFileInfo( bestViewsFileName );
        settings.setValue( "bestViewsDir", bestViewsFileInfo.absolutePath() );

        m_saveBestViewsPushButton->setEnabled( true );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveBestViews()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString bestViewsDir = settings.value( "bestViewsDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save best views"), bestViewsDir, tr("Data files (*.dat);;Text files (*.txt);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "dat" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString bestViewsFileName = saveDialog.selectedFiles().first();
        bool saveAsText = bestViewsFileName.endsWith( ".txt" );
        QFile bestViewsFile( bestViewsFileName );
        QIODevice::OpenMode mode = QIODevice::WriteOnly | QIODevice::Truncate;
        if ( saveAsText ) mode = mode | QIODevice::Text;

        if ( !bestViewsFile.open( mode ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + bestViewsFileName );
            QMessageBox::warning( this, tr("Can't save best views"), QString( tr("Can't save best views to file ") ) + bestViewsFileName );
            return;
        }

        int nBestViews = m_bestViews.size();

        if ( saveAsText )
        {
            QTextStream out( &bestViewsFile );
            for ( int i = 0; i < nBestViews; i++ ) out << i << ": v" << m_bestViews.at( i ).first + 1 << " " << m_bestViews.at( i ).second.toString() << "\n";
        }
        else
        {
            QDataStream out( &bestViewsFile );
            for ( int i = 0; i < nBestViews; i++ )
            {
                const Vector3 &v = m_bestViews.at( i ).second;
                out << m_bestViews.at( i ).first << v.x << v.y << v.z;
            }
        }

        bestViewsFile.close();

        QFileInfo bestViewsFileInfo( bestViewsFileName );
        settings.setValue( "bestViewsDir", bestViewsFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::loadVomi()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString vomiDir = settings.value( "vomiDir", QString() ).toString();
    QString vomiFileName = QFileDialog::getOpenFileName( this, tr("Load VoMI"), vomiDir, tr("Data files (*.dat);;All files (*)") );

    if ( !vomiFileName.isNull() )
    {
        QFile vomiFile( vomiFileName );

        if ( !vomiFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + vomiFileName );
            QMessageBox::warning( this, tr("Can't load VoMI"), QString( tr("Can't load VoMI from file ") ) + vomiFileName );
            return;
        }

        unsigned int nObjects = m_volume->getSize();
        m_vomi.resize( nObjects );
        m_maximumVomi = 0.0f;

        QDataStream in( &vomiFile );

        for ( unsigned int i = 0; i < nObjects && !in.atEnd(); i++ )
        {
            in >> m_vomi[i];
            if ( m_vomi.at( i ) > m_maximumVomi ) m_maximumVomi = m_vomi.at( i );
        }

        vomiFile.close();

        QFileInfo vomiFileInfo( vomiFileName );
        settings.setValue( "vomiDir", vomiFileInfo.absolutePath() );

        m_vomiCheckBox->setEnabled( true );
        m_saveVomiPushButton->setEnabled( true );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveVomi()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString vomiDir = settings.value( "vomiDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save VoMI"), vomiDir, tr("Data files (*.dat);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "dat" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString vomiFileName = saveDialog.selectedFiles().first();
        QFile vomiFile( vomiFileName );

        if ( !vomiFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + vomiFileName );
            QMessageBox::warning( this, tr("Can't save VoMI"), QString( tr("Can't save VoMI to file ") ) + vomiFileName );
            return;
        }

        QDataStream out( &vomiFile );

        unsigned int nObjects = m_volume->getSize();

        for ( unsigned int i = 0; i < nObjects; i++ ) out << m_vomi.at( i );

        vomiFile.close();

        QFileInfo vomiFileInfo( vomiFileName );
        settings.setValue( "vomiDir", vomiFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::vomiChecked( bool checked )
{
    if ( checked ) m_voxelSalienciesCheckBox->setChecked( false );
}


void QExperimental3DExtension::loadVoxelSaliencies()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString voxelSalienciesDir = settings.value( "voxelSalienciesDir", QString() ).toString();
    QString voxelSalienciesFileName = QFileDialog::getOpenFileName( this, tr("Load voxel saliencies"), voxelSalienciesDir, tr("Data files (*.dat);;All files (*)") );

    if ( !voxelSalienciesFileName.isNull() )
    {
        QFile voxelSalienciesFile( voxelSalienciesFileName );

        if ( !voxelSalienciesFile.open( QFile::ReadOnly ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + voxelSalienciesFileName );
            QMessageBox::warning( this, tr("Can't load voxel saliencies"), QString( tr("Can't load voxel saliencies from file ") ) + voxelSalienciesFileName );
            return;
        }

        unsigned int nObjects = m_volume->getSize();
        m_voxelSaliencies.resize( nObjects );
        m_maximumSaliency = 0.0f;

        QDataStream in( &voxelSalienciesFile );

        for ( unsigned int i = 0; i < nObjects && !in.atEnd(); i++ )
        {
            in >> m_voxelSaliencies[i];
            if ( m_voxelSaliencies.at( i ) > m_maximumSaliency ) m_maximumSaliency = m_voxelSaliencies.at( i );
        }

        voxelSalienciesFile.close();

        QFileInfo voxelSalienciesFileInfo( voxelSalienciesFileName );
        settings.setValue( "voxelSalienciesDir", voxelSalienciesFileInfo.absolutePath() );

        m_voxelSalienciesCheckBox->setEnabled( true );
        m_saveVoxelSalienciesPushButton->setEnabled( true );
    }

    settings.endGroup();
}


void QExperimental3DExtension::saveVoxelSaliencies()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString voxelSalienciesDir = settings.value( "voxelSalienciesDir", QString() ).toString();
    QFileDialog saveDialog( this, tr("Save voxel saliencies"), voxelSalienciesDir, tr("Data files (*.dat);;All files (*)") );
    saveDialog.setAcceptMode( QFileDialog::AcceptSave );
    saveDialog.setDefaultSuffix( "dat" );

    if ( saveDialog.exec() == QDialog::Accepted )
    {
        QString voxelSalienciesFileName = saveDialog.selectedFiles().first();
        QFile voxelSalienciesFile( voxelSalienciesFileName );

        if ( !voxelSalienciesFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            DEBUG_LOG( QString( "No es pot escriure al fitxer " ) + voxelSalienciesFileName );
            QMessageBox::warning( this, tr("Can't save voxel saliencies"), QString( tr("Can't save voxel saliencies to file ") ) + voxelSalienciesFileName );
            return;
        }

        QDataStream out( &voxelSalienciesFile );

        unsigned int nObjects = m_volume->getSize();

        for ( unsigned int i = 0; i < nObjects; i++ ) out << m_voxelSaliencies.at( i );

        voxelSalienciesFile.close();

        QFileInfo voxelSalienciesFileInfo( voxelSalienciesFileName );
        settings.setValue( "voxelSalienciesDir", voxelSalienciesFileInfo.absolutePath() );
    }

    settings.endGroup();
}


void QExperimental3DExtension::voxelSalienciesChecked( bool checked )
{
    if ( checked ) m_vomiCheckBox->setChecked( false );
}


void QExperimental3DExtension::loadAndRunProgram()
{
    QSettings settings;
    settings.beginGroup( "Experimental3D" );

    QString programDir = settings.value( "programDir", QString() ).toString();
    QString programFileName = QFileDialog::getOpenFileName( this, tr("Load program"), programDir, tr("Text files (*.txt);;All files (*)") );

    if ( !programFileName.isNull() )
    {
        QFile programFile( programFileName );

        if ( !programFile.open( QFile::ReadOnly | QFile::Text ) )
        {
            DEBUG_LOG( QString( "No es pot llegir el fitxer " ) + programFileName );
            QMessageBox::warning( this, tr("Can't load program"), QString( tr("Can't load program from file ") ) + programFileName );
            return;
        }

        QTextStream in( &programFile );

        while ( !in.atEnd() )
        {
            QString line = in.readLine();
            QStringList words = line.split( ' ', QString::SkipEmptyParts );

            if ( words.isEmpty() ) continue;

            QString command = words.at( 0 );

            if ( command == "tf-load" )
            {
                if ( words.size() >= 2 ) loadTransferFunction( words.at( 1 ) );
                else
                {
                    DEBUG_LOG( "[E3DP] Falta el nom del fitxer: " + line );
                    ERROR_LOG( "[E3DP] Falta el nom del fitxer: " + line );
                }
            }
            else if ( command == "visualization-ok" )
            {
                doVisualization();
            }
            else
            {
                DEBUG_LOG( "[E3DP] Ordre desconeguda: " + line );
                ERROR_LOG( "[E3DP] Ordre desconeguda: " + line );
            }
        }

        programFile.close();

        QFileInfo programFileInfo( programFileName );
        settings.setValue( "programDir", programFileInfo.absolutePath() );
    }

    settings.endGroup();
}


}
