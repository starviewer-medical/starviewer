/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdefaultviewerextension.h"

#include "volume.h"

namespace udg {

QDefaultViewerExtension::QDefaultViewerExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    m_mainVolume = 0;
    createConnections();
}


QDefaultViewerExtension::~QDefaultViewerExtension()
{
}

void QDefaultViewerExtension::setInput( Volume *input )
{
    m_mainVolume = input;
    m_2DView->setInput( m_mainVolume );
    
    // refrescar el controls
    m_spinBox->setMinimum( 0 );
    m_spinBox->setMaximum( m_mainVolume->getVtkData()->GetDimensions()[2] );
    m_slider->setMaximum(  m_mainVolume->getVtkData()->GetDimensions()[2] );
    m_slider->setValue( m_2DView->getSlice() );
    m_2DView->render();
}

void QDefaultViewerExtension::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged(int) ) , m_spinBox , SLOT( setValue(int) ) );
    connect( m_spinBox , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );
}

}
