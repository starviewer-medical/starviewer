#include "qexperimental3dextension.h"

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
    connect( m_vmiPushButton, SIGNAL( clicked() ), SLOT( computeVmi() ) );
    connect( m_viewpointUnstabilitiesPushButton, SIGNAL( clicked() ), SLOT( computeViewpointUnstabilities() ) );
    connect( m_loadVomiPushButton, SIGNAL( clicked() ), SLOT( loadVomi() ) );
    connect( m_saveVomiPushButton, SIGNAL( clicked() ), SLOT( saveVomi() ) );
    connect( m_loadVoxelSalienciesPushButton, SIGNAL( clicked() ), SLOT( loadVoxelSaliencies() ) );
    connect( m_saveVoxelSalienciesPushButton, SIGNAL( clicked() ), SLOT( saveVoxelSaliencies() ) );
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
        TransferFunction *transferFunction;

        if ( transferFunctionFileName.endsWith( ".xml" ) ) transferFunction = TransferFunctionIO::fromXmlFile( transferFunctionFileName );
        else transferFunction = TransferFunctionIO::fromFile( transferFunctionFileName );

        m_transferFunctionEditor->setTransferFunction( *transferFunction );
        delete transferFunction;

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
    bool computeVomi = m_computeVomiCheckBox->isChecked();
    bool computeVoxelSaliencies = m_computeVoxelSalienciesCheckBox->isChecked();

    if ( !computeVomi && !computeVoxelSaliencies ) return;

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

    // Inicialitzar progrés
    int nSteps = 4; // ray casting (p(O|V)), p(V), p(O), VoMI + voxel saliencies
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

    // VoMI + voxel saliencies (inicialització)
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
    // VoMI + voxel saliencies (càlcul)
    {
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

                        float saliency = 0.0f;
                        int nNeighbours = 0;

                        // iterem pels veïns
                        for ( int j = 0; j < 26; j++ )
                        {
                            if ( !validNeighbours[j] ) continue;

                            nNeighbours++;

                            float poj = objectProbabilities.at( neighbours[j] );    // p(oj)
                            Q_ASSERT( poj == poj );
                            float poij = poi + poj; // p(ô)

                            if ( poij == 0.0 ) continue;

                            // p(V|oj)
                            if ( poj == 0.0 ) pVoj.fill( 0.0f );    // si p(oj) == 0 vol dir que el vòxel no es veu des d'enlloc --> p(V|oj) ha de ser tot zeros
                            else for ( int k = 0; k < nViewpoints; k++ ) pVoj[k] = viewProbabilities.at( k ) * pOV[k][neighbours[j] - pOvShift] / poj;

                            float s = InformationTheory<float>::jensenShannonDivergence( poi / poij, poj / poij, pVoi, pVoj );
                            Q_ASSERT( s == s );
                            saliency += s;
                        }

                        saliency /= nNeighbours;
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


void QExperimental3DExtension::computeVmi()
{
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;

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

    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();

    m_vmiProgressBar->setValue( 0 );
    m_volume->startVmiMode();

    QVector<float> viewProbabilities( nViewpoints );    // vector de p(v), inicialitzat a 0
    {
        float totalViewedVolume = 0.0;

        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiFirstPass();
            setViewpoint( viewpoints.at( i ) ); // render
            float viewedVolume = m_volume->finishVmiFirstPass();
            viewProbabilities[i] = viewedVolume;
            totalViewedVolume += viewedVolume;
            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / ( 3 * nViewpoints ) );
        }

        if ( totalViewedVolume > 0.0f )
        {
            for ( int i = 0; i < nViewpoints; i++ )
            {
                viewProbabilities[i] /= totalViewedVolume;
                DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i + 1 ).arg( viewProbabilities.at( i ) ) );
            }
        }
    }

    unsigned int nObjects = m_volume->getSize();
    QVector<float> objectProbabilities( nObjects ); // vector de p(o), inicialitzat a 0
    {
        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiSecondPass();
            setViewpoint( viewpoints.at( i ) ); // render
            QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass();
            for ( unsigned int j = 0; j < nObjects; j++ )
                objectProbabilities[j] += viewProbabilities.at( i ) * objectProbabilitiesInView.at( j );
            m_vmiProgressBar->setValue( 100 * ( nViewpoints + i + 1 ) / ( 3 * nViewpoints ) );
        }

        //for ( unsigned int i = 0; i < nObjects; i++ ) DEBUG_LOG( QString( "p(o%1) = %2" ).arg( i ).arg( objectProbabilities.at( i ) ) );
    }

    QVector<float> vmi( nViewpoints );
    {
        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiSecondPass();
            setViewpoint( viewpoints.at( i ) ); // render
            QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass();
            vmi[i] = InformationTheory<float>::kullbackLeiblerDivergence( objectProbabilitiesInView, objectProbabilities );
            m_vmiProgressBar->setValue( 100 * ( 2 * nViewpoints + i + 1 ) / ( 3 * nViewpoints ) );
        }

        // Printar resultats i guardar-los en un fitxer
        QFile outFile( QDir::tempPath().append( "/vmi.txt" ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFile );
            for ( int i = 0; i < nViewpoints; i++ )
            {
                DEBUG_LOG( QString( "VMI(v%1) = %2" ).arg( i + 1 ).arg( vmi.at( i ) ) );
                out << "VMI(v" << i + 1 << ") = " << vmi.at( i ) << "\n";
            }
            outFile.close();
        }
    }

    doVisualization();
    m_viewer->setCamera( position, focus, up );
}


void QExperimental3DExtension::computeViewpointUnstabilities()
{
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;

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

    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();

    // calculem el nombre de renders que caldran, per fer el progrés més real
    int nRenders = 2 * nViewpoints;
    for ( int i = 0; i < nViewpoints; i++ ) nRenders += viewpointGenerator.neighbours( i ).size();
    int renderCount = 0;

    m_vmiProgressBar->setValue( 0 );
    m_volume->startVmiMode();

    QVector<float> viewProbabilities( nViewpoints );    // vector de p(v), inicialitzat a 0
    {
        float totalViewedVolume = 0.0;

        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiFirstPass();
            setViewpoint( viewpoints.at( i ) ); // render
            float viewedVolume = m_volume->finishVmiFirstPass();
            viewProbabilities[i] = viewedVolume;
            totalViewedVolume += viewedVolume;
            m_vmiProgressBar->setValue( 100 * ++renderCount / nRenders );
        }

        if ( totalViewedVolume > 0.0f )
        {
            for ( int i = 0; i < nViewpoints; i++ )
            {
                viewProbabilities[i] /= totalViewedVolume;
                DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i + 1 ).arg( viewProbabilities.at( i ) ) );
            }
        }
    }

    QVector<float> viewpointUnstabilities( nViewpoints );
    {
        for ( int i = 0; i < nViewpoints; i++ )
        {
            DEBUG_LOG( QString( "v%1" ).arg( i + 1 ) );

            float pvi = viewProbabilities.at( i );

            m_volume->startVmiSecondPass();
            setViewpoint( viewpoints.at( i ) ); // render
            QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass(); // vector de p(o|v) = distribució p(O|v)
            m_vmiProgressBar->setValue( 100 * ++renderCount / nRenders );

            QVector<int> neighbours = viewpointGenerator.neighbours( i );
            int nNeighbours = neighbours.size();
            float viewpointUnstability = 0.0;

            for ( int j = 0; j < nNeighbours; j++ )
            {
                DEBUG_LOG( QString( "-v%1" ).arg( neighbours.at( j ) + 1 ) );

                float pvj = viewProbabilities.at( neighbours.at( j ) );
                float pvij = pvi + pvj;

                if ( pvij == 0.0 )
                {
                    m_vmiProgressBar->setValue( 100 * ++renderCount / nRenders );
                    continue;
                }

                m_volume->startVmiSecondPass();
                setViewpoint( viewpoints.at( neighbours.at( j ) ) );    // render
                QVector<float> objectProbabilitiesInNeighbour = m_volume->finishVmiSecondPass();    // vector de p(o|v) = distribució p(O|v)

                float viewpointDissimilarity = InformationTheory<float>::jensenShannonDivergence( pvi / pvij, pvj / pvij, objectProbabilitiesInView, objectProbabilitiesInNeighbour );
                viewpointUnstability += viewpointDissimilarity;

                m_vmiProgressBar->setValue( 100 * ++renderCount / nRenders );
            }

            viewpointUnstability /= nNeighbours;
            viewpointUnstabilities[i] = viewpointUnstability;
        }

        // Printar resultats i guardar-los en un fitxer
        QFile outFile( QDir::tempPath().append( "/viewpointUnstabilities.txt" ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFile );
            for ( int i = 0; i < nViewpoints; i++ )
            {
                DEBUG_LOG( QString( "U(v%1) = %2" ).arg( i + 1 ).arg( viewpointUnstabilities.at( i ) ) );
                out << "U(v" << i + 1 << ") = " << viewpointUnstabilities.at( i ) << "\n";
            }
            outFile.close();
        }
    }

    doVisualization();
    m_viewer->setCamera( position, focus, up );
}


void QExperimental3DExtension::computeVomi()
{
    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;

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

    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();

    m_vmiProgressBar->setValue( 0 );
    m_volume->startVmiMode();

    QVector<float> viewProbabilities( nViewpoints );    // vector de p(v), inicialitzat a 0
    {
        float totalViewedVolume = 0.0;

        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiFirstPass();
            setViewpoint( viewpoints.at( i ) ); // render
            float viewedVolume = m_volume->finishVmiFirstPass();
            viewProbabilities[i] = viewedVolume;
            totalViewedVolume += viewedVolume;
            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / ( 3 * nViewpoints ) );
        }

        if ( totalViewedVolume > 0.0f )
        {
            for ( int i = 0; i < nViewpoints; i++ )
            {
                viewProbabilities[i] /= totalViewedVolume;
                Q_ASSERT( viewProbabilities.at( i ) == viewProbabilities.at( i ) );
                DEBUG_LOG( QString( "p(v%1) = %2" ).arg( i + 1 ).arg( viewProbabilities.at( i ) ) );
            }
        }
    }

    unsigned int nObjects = m_volume->getSize();
    QVector<float> objectProbabilities( nObjects ); // vector de p(o), inicialitzat a 0
    {
        for ( int i = 0; i < nViewpoints; i++ )
        {
            m_volume->startVmiSecondPass();
            setViewpoint( viewpoints.at( i ) ); // render
            QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass();
            for ( unsigned int j = 0; j < nObjects; j++ )
                objectProbabilities[j] += viewProbabilities.at( i ) * objectProbabilitiesInView.at( j );
            m_vmiProgressBar->setValue( 100 * ( nViewpoints + i + 1 ) / ( 3 * nViewpoints ) );
        }

        for ( unsigned int i = 0; i < nObjects; i++ )
        {
            Q_ASSERT( objectProbabilities.at( i ) == objectProbabilities.at( i ) );
            //DEBUG_LOG( QString( "p(o%1) = %2" ).arg( i ).arg( objectProbabilities.at( i ) ) );
        }
    }

    m_vomi.resize( nObjects );  // vector de VoMI, inicialitzat a 0
    m_maximumVomi = 0.0f;
    {
        for ( int i = 0; i < nViewpoints; i++ )
        {
            float pv = viewProbabilities.at( i );
            Q_ASSERT( pv == pv );

            m_volume->startVmiSecondPass();
            setViewpoint( viewpoints.at( i ) ); // render
            QVector<float> objectProbabilitiesInView = m_volume->finishVmiSecondPass();

            for ( unsigned int j = 0; j < nObjects; j++ )
            {
                float po = objectProbabilities.at( j );

                if ( po > 0.0f )
                {
                    float pov = objectProbabilitiesInView.at( j );
                    Q_ASSERT( pov == pov );
                    float pvo = pv * pov / po;
                    if ( pvo > 0.0f ) m_vomi[j] += pvo * MathTools::logTwo( pvo / pv );
                    if ( m_vomi.at( j ) > m_maximumVomi ) m_maximumVomi = m_vomi.at( j );
                }
            }

            m_vmiProgressBar->setValue( 100 * ( 2 * nViewpoints + i + 1 ) / ( 3 * nViewpoints ) );
        }

//        // Printar resultats i guardar-los en un fitxer
//        QFile outFile( QDir::tempPath().append( "/vomi.txt" ) );
//        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
//        {
//            QTextStream out( &outFile );
//            for ( unsigned int i = 0; i < nObjects; i++ )
//            {
//                //DEBUG_LOG( QString( "VoMI(o%1) = %2" ).arg( i ).arg( m_vomi.at( i ) ) );
//                out << "VoMI(o" << i << ") = " << m_vomi.at( i ) << "\n";
//            }
//            outFile.close();
//        }

        m_vomiCheckBox->setEnabled( true );
        m_saveVomiPushButton->setEnabled( true );
    }

    doVisualization();
    m_viewer->setCamera( position, focus, up );
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


void QExperimental3DExtension::computeVoxelSaliencies()
{
    setCursor( QCursor( Qt::WaitCursor ) );

    Vector3 position, focus, up;
    m_viewer->getCamera( position, focus, up );

    float distance = ( position - focus ).length();

    ViewpointGenerator viewpointGenerator;

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

    QVector<Vector3> viewpoints = viewpointGenerator.viewpoints();
    int nViewpoints = viewpoints.size();
    unsigned int nObjects = m_volume->getSize();

    m_vmiProgressBar->setValue( 0 );
    m_vmiProgressBar->repaint();
    m_vmiTotalProgressBar->setMaximum( 4 );
    m_vmiTotalProgressBar->setValue( 0 );
    m_vmiTotalProgressBar->repaint();
    m_volume->startVmiMode();

    QVector<float> viewProbabilities( nViewpoints );    // vector de p(v), inicialitzat a 0
    QVector<float> objectProbabilities( nObjects );     // vector de p(o), inicialitzat a 0
    QVector<QTemporaryFile*> pOvFiles( nViewpoints );

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

    // pOvFiles guardarà la matriu p(O|V)
    // cada fitxer una fila -> p(O|v1), p(O|v2)...

    // p(V) i p(O)
    {
        m_vmiProgressBar->setValue( 0 );
        m_vmiProgressBar->repaint();

        float totalViewedVolume = 0.0;

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

        m_vmiTotalProgressBar->setValue( 1 );
        m_vmiTotalProgressBar->repaint();

        // p(V)
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

        m_vmiTotalProgressBar->setValue( 2 );
        m_vmiTotalProgressBar->repaint();

        // p(O)
        m_vmiProgressBar->setValue( 0 );
        float *objectProbabilitiesInView = new float[nObjects];
        for ( int i = 0; i < nViewpoints; i++ )
        {
            pOvFiles[i]->reset();
            pOvFiles[i]->read( reinterpret_cast<char*>( objectProbabilitiesInView ), nObjects * sizeof(float) );    // llegim...
            for ( unsigned int j = 0; j < nObjects; j++ )
            {
                objectProbabilities[j] += viewProbabilities.at( i ) * objectProbabilitiesInView[j];
            }
            pOvFiles[i]->reset();   // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)
            m_vmiProgressBar->setValue( 100 * ( i + 1 ) / nViewpoints );
            m_vmiProgressBar->repaint();
        }
        m_vmiTotalProgressBar->setValue( 3 );
        m_vmiTotalProgressBar->repaint();
        for ( unsigned int i = 0; i < nObjects; i++ )
        {
                Q_ASSERT( objectProbabilities.at( i ) == objectProbabilities.at( i ) );
                //DEBUG_LOG( QString( "p(o%1) = %2" ).arg( i ).arg( objectProbabilities.at( i ) ) );
        }

        delete[] objectProbabilitiesInView;
    }

    /*
     * Fer un peek per llegir el fitxer sencer és costós. Podem aprofitar que sabem com estan distribuïts els veïns per acotar el tros de fitxer que cal llegir de manera que el que valor que busquem sigui a dins.
     * N'hi haurà prou llegint la llesca actual, l'anterior i la següent.
     */
    m_voxelSaliencies.resize( nObjects );
    m_maximumSaliency = 0.0f;
    {
        int *dimensions = m_volume->getImage()->GetDimensions();
        int dimX = dimensions[0], dimY = dimensions[1], dimZ = dimensions[2], dimXY = dimX * dimY, dimXY3 = dimXY * 3;
        qint64 sizeToRead = dimXY3 * sizeof(float), sizeToReadOnEdge = dimXY * 2 * sizeof(float);
        float **pOV = new float*[nViewpoints];
        for ( int i = 0; i < nViewpoints; i++ ) pOV[i] = new float[dimXY3];
        QVector<float> pVoi( nViewpoints );
        QVector<float> pVoj( nViewpoints );

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

                    float poi = objectProbabilities.at( i );
                    Q_ASSERT( poi == poi );

                    // pVoi
                    if ( poi == 0.0 ) pVoi.fill( 0.0f );    // si poi == 0 vol dir que el vòxel no es veu des d'enlloc --> pVoi ha de ser tot zeros
                    else for ( int k = 0; k < nViewpoints; k++ ) pVoi[k] = pOV[k][i - pOvShift];

                    int neighbours[6] = { x - 1 + y * dimX + z * dimXY, x + 1 + y * dimX + z * dimXY,
                                          x + ( y - 1 ) * dimX + z * dimXY, x + ( y + 1 ) * dimX + z * dimXY,
                                          x + y * dimX + ( z - 1 ) * dimXY, x + y * dimX + ( z + 1 ) * dimXY };
                    bool validNeighbours[6] = { x > 0, x + 1 < dimX,
                                                y > 0, y + 1 < dimY,
                                                z > 0, z + 1 < dimZ };

                    // iterem pels veïns
                    for ( int j = 0; j < 6; j++ )
                    {
                        if ( !validNeighbours[j] ) continue;

                        float poj = objectProbabilities.at( neighbours[j] );
                        Q_ASSERT( poj == poj );
                        float poij = poi + poj;

                        if ( poij == 0.0 ) continue;

                        // pVoj
                        if ( poj == 0.0 ) pVoj.fill( 0.0f );    // si poj == 0 vol dir que el vòxel no es veu des d'enlloc --> pVoj ha de ser tot zeros
                        else for ( int k = 0; k < nViewpoints; k++ ) pVoj[k] = pOV[k][neighbours[j] - pOvShift];

                        float saliency = InformationTheory<float>::jensenShannonDivergence( poi / poij, poj / poij, pVoi, pVoj );
                        Q_ASSERT( saliency == saliency );
                        m_voxelSaliencies[i] += saliency;
                    }

                    m_voxelSaliencies[i] /= 6.0f;
                    if ( m_voxelSaliencies.at( i ) > m_maximumSaliency ) m_maximumSaliency = m_voxelSaliencies.at( i );
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

        m_vmiTotalProgressBar->setValue( 4 );
        m_vmiTotalProgressBar->repaint();

        DEBUG_LOG( "esborrem pOV" );
        for ( int i = 0; i < nViewpoints; i++ ) delete[] pOV[i];
        delete[] pOV;

        /*DEBUG_LOG( "printem" );
        // Printar resultats i guardar-los en un fitxer
        QFile outFile( QDir::tempPath().append( "/voxelSaliencies.txt" ) );
        if ( outFile.open( QFile::WriteOnly | QFile::Truncate ) )
        {
            QTextStream out( &outFile );
            for ( unsigned int i = 0; i < nObjects; i++ )
            {
                //DEBUG_LOG( QString( "S(o%1) = %2" ).arg( i ).arg( m_voxelSaliencies.at( i ) ) );
                out << "S(o" << i << ") = " << m_voxelSaliencies.at( i ) << "\n";
            }
            outFile.close();
        }*/

        m_voxelSalienciesCheckBox->setEnabled( true );
        m_saveVoxelSalienciesPushButton->setEnabled( true );
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


}
