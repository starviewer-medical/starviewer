#include "qgputestingextension.h"

#include <QColorDialog>

#include <vtkImageData.h>

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
    connect( m_visualizationOkPushButton, SIGNAL( clicked() ), SLOT( doVisualization() ) );
}


void QGpuTestingExtension::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor( m_viewer->backgroundColor(), this );
    if ( color.isValid() ) m_viewer->setBackgroundColor( color );
}


void QGpuTestingExtension::doVisualization()
{
    m_viewer->setTransferFunction( m_transferFunctionEditor->getTransferFunction().to01( 0.0, m_transferFunctionEditor->maximum() ) );
    m_viewer->updateGL();
}


}
