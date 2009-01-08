#include "qgputestingextension.h"

#include <QColorDialog>
#include <QFileDialog>
#include <QSettings>

#include <vtkImageData.h>

#include "transferfunctionio.h"
#include "volume.h"


namespace udg {


QGpuTestingExtension::QGpuTestingExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

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
}


void QGpuTestingExtension::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor( m_viewer->backgroundColor(), this );
    if ( color.isValid() ) m_viewer->setBackgroundColor( color );
}


void QGpuTestingExtension::loadTransferFunction()
{
    QSettings settings;
    settings.beginGroup( "GpuTesting" );

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


void QGpuTestingExtension::saveTransferFunction()
{
    QSettings settings;
    settings.beginGroup( "GpuTesting" );

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


void QGpuTestingExtension::doVisualization()
{
    m_viewer->setTransferFunction( m_transferFunctionEditor->getTransferFunction().to01( 0.0, m_transferFunctionEditor->maximum() ) );
    m_viewer->updateGL();
}


}
