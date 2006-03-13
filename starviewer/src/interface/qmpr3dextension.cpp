/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextension.h"
#include "q3dmprviewer.h"
#include <QToolButton>

namespace udg {

QMPR3DExtension::QMPR3DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    connect(m_sagitalViewEnabledButton, SIGNAL(toggled(bool)), m_mpr3DView, SLOT(setSagitalVisibility(bool)));
    connect(m_coronalViewEnabledButton, SIGNAL(toggled(bool)), m_mpr3DView, SLOT(setCoronalVisibility(bool)));
    connect(m_axialViewEnabledButton, SIGNAL(toggled(bool)), m_mpr3DView, SLOT(setAxialVisibility(bool)));

    connect(m_sagitalOrientationButton, SIGNAL(clicked()), m_mpr3DView, SLOT(resetViewToSagital()));
    connect(m_coronalOrientationButton, SIGNAL(clicked()), m_mpr3DView, SLOT(resetViewToCoronal()));
    connect(m_axialOrientationButton, SIGNAL(clicked()), m_mpr3DView, SLOT(resetViewToAxial()));    
}


QMPR3DExtension::~QMPR3DExtension()
{
}

void QMPR3DExtension::setInput( Volume *input )
{ 
    m_volume = input; 
    m_mpr3DView->setInput( m_volume );
}

};  // end namespace udg 
