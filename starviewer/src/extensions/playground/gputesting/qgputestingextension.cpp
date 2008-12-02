#include "qgputestingextension.h"


namespace udg {


QGpuTestingExtension::QGpuTestingExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
}


QGpuTestingExtension::~QGpuTestingExtension()
{
}


void QGpuTestingExtension::setInput( Volume *input )
{
    m_viewer->setVolume( input );
}


}
