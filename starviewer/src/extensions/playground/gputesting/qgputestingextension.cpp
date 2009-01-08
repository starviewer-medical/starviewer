#include "qgputestingextension.h"

#include <QColorDialog>


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
}


void QGpuTestingExtension::createConnections()
{
    // visualització
    connect( m_backgroundColorPushButton, SIGNAL( clicked() ), SLOT( chooseBackgroundColor() ) );
}


void QGpuTestingExtension::chooseBackgroundColor()
{
    QColor color = QColorDialog::getColor( m_viewer->backgroundColor(), this );
    if ( color.isValid() ) m_viewer->setBackgroundColor( color );
}


}
