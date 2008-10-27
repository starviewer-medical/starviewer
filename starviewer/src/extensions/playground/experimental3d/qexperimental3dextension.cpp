#include "qexperimental3dextension.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QSettings>
#include <QTextStream>

#include "experimental3dvolume.h"
#include "logging.h"
#include "transferfunctionio.h"
#include "vector3.h"
#include "viewpointgenerator.h"


namespace udg {


QExperimental3DExtension::QExperimental3DExtension( QWidget *parent )
 : QWidget( parent ), m_volume( 0 )
{
    setupUi( this );

    createConnections();
}


QExperimental3DExtension::~QExperimental3DExtension()
{
}


void QExperimental3DExtension::setInput( Volume *input )
{
    m_viewer->setInput( input );
    m_volume = m_viewer->getVolume();

    unsigned char max = m_volume->getRangeMax();
    m_transferFunctionEditor->setMaximum( max );
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

    // càmera
    connect( m_cameraGetPushButton, SIGNAL( clicked() ), SLOT( getCamera() ) );
    connect( m_cameraSetPushButton, SIGNAL( clicked() ), SLOT( setCamera() ) );
    connect( m_cameraLoadPushButton, SIGNAL( clicked() ), SLOT( loadCamera() ) );
    connect( m_cameraSavePushButton, SIGNAL( clicked() ), SLOT( saveCamera() ) );
    connect( m_cameraViewpointDistributionWidget, SIGNAL( numberOfViewpointsChanged(int) ), SLOT( setNumberOfViewpoints(int) ) );
    connect( m_viewpointPushButton, SIGNAL( clicked() ), SLOT( setViewpoint() ) );
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
            TransferFunctionIO::toXmlFile( transferFunctionFileName, m_transferFunctionEditor->getTransferFunction() );
        else
            TransferFunctionIO::toFile( transferFunctionFileName, m_transferFunctionEditor->getTransferFunction() );

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
    m_volume->setLighting( m_diffuseCheckBox->isChecked(), m_specularCheckBox->isChecked(), m_specularPowerDoubleSpinBox->value() );
    m_volume->setContour( m_contourCheckBox->isChecked(), m_contourDoubleSpinBox->value() );
    m_volume->setTransferFunction( m_transferFunctionEditor->getTransferFunction() );
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

    float distance = position.length();

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

    position = viewpointGenerator.viewpoint( m_viewpointSpinBox->value() - 1 );
    focus = Vector3( 0.0, 0.0, 0.0 );
    up = Vector3( 0.0, 1.0, 0.0 );

    Vector3 position2 = position;
    if ( qAbs( position2.normalize() * up ) > 0.9 ) up = Vector3( 0.0, 0.0, 1.0 );

    m_viewer->setCamera( position, focus, up );
}


}
