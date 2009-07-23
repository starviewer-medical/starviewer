#include "qgputestingextension.h"
#include "gputestingsettings.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QMessageBox>
#include <QTextStream>
#include <vtkImageData.h>

#include "transferfunctionio.h"
#include "volume.h"

namespace udg {


QGpuTestingExtension::QGpuTestingExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    GPUTestingSettings().init();

    createConnections();
}


QGpuTestingExtension::~QGpuTestingExtension()
{
}


void QGpuTestingExtension::setInput( Volume *input )
{
    m_viewer->setVolume( input );

    vtkImageData *imageData = input->getVtkData();
    double *range = imageData->GetScalarRange();
    unsigned short max = static_cast<unsigned short>( range[1] - range[0] );
    m_transferFunctionEditor->setMaximum( max );

    TransferFunction defaultTransferFunction;
    defaultTransferFunction.addPoint( 0, QColor( 0, 0, 0, 0 ) );
    defaultTransferFunction.addPoint( max, QColor( 255, 255, 255, 255 ) );
    m_transferFunctionEditor->setTransferFunction( defaultTransferFunction );

    doVisualization();
}


void QGpuTestingExtension::createConnections()
{
    // visualització
    connect( m_backgroundColorPushButton, SIGNAL( clicked() ), SLOT( chooseBackgroundColor() ) );
    connect( m_loadTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( loadTransferFunction() ) );
    connect( m_saveTransferFunctionPushButton, SIGNAL( clicked() ), SLOT( saveTransferFunction() ) );
    connect( m_visualizationOkPushButton, SIGNAL( clicked() ), SLOT( doVisualization() ) );

    // càmera
    connect( m_fieldOfViewSpinBox, SIGNAL( valueChanged(int) ), m_viewer, SLOT( setFieldOfView(int) ) );
    connect( m_cameraGetPushButton, SIGNAL( clicked() ), SLOT( getCamera() ) );
    connect( m_cameraSetPushButton, SIGNAL( clicked() ), SLOT( setCamera() ) );
    connect( m_cameraLoadPushButton, SIGNAL( clicked() ), SLOT( loadCamera() ) );
    connect( m_cameraSavePushButton, SIGNAL( clicked() ), SLOT( saveCamera() ) );
}


void QGpuTestingExtension::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor( m_viewer->backgroundColor(), this );
    if ( color.isValid() ) m_viewer->setBackgroundColor( color );
}


void QGpuTestingExtension::loadTransferFunction()
{
    Settings settings;

    QString transferFunctionDir = settings.getValue( GPUTestingSettings::TransferFunctionFilesPath ).toString();
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
        settings.setValue( GPUTestingSettings::TransferFunctionFilesPath, transferFunctionFileInfo.absolutePath() );
    }
}


void QGpuTestingExtension::saveTransferFunction()
{
    Settings settings;

    QString transferFunctionDir = settings.getValue( GPUTestingSettings::TransferFunctionFilesPath ).toString();
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
        settings.setValue( GPUTestingSettings::TransferFunctionFilesPath, transferFunctionFileInfo.absolutePath() );
    }
}


void QGpuTestingExtension::doVisualization()
{
    m_viewer->setRayStep( m_rayStepDoubleSpinBox->value() );
    m_viewer->setLighting( m_diffuseLightingCheckBox->isChecked(), false, 64.0f );
    m_viewer->setTransferFunction( m_transferFunctionEditor->getTransferFunction().to01( 0.0, m_transferFunctionEditor->maximum() ) );
    m_viewer->updateGL();
}


void QGpuTestingExtension::getCamera()
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


void QGpuTestingExtension::setCamera()
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


void QGpuTestingExtension::loadCamera()
{
    Settings settings;

    QString cameraDir = settings.getValue( GPUTestingSettings::CameraFilesPath ).toString();
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
        settings.setValue( GPUTestingSettings::CameraFilesPath, cameraFileInfo.absolutePath() );

        setCamera();
    }
}


void QGpuTestingExtension::saveCamera()
{
    Settings settings;

    QString cameraDir = settings.getValue( GPUTestingSettings::CameraFilesPath ).toString();
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
        settings.setValue( GPUTestingSettings::CameraFilesPath, cameraFileInfo.absolutePath() );
    }
}


}
