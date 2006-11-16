/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextension.h"
#include "q3dmprviewer.h"
#include "logging.h"
#include <QToolButton>
#include <QMessageBox>

namespace udg {

QMPR3DExtension::QMPR3DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );

    createConnections();
    
    m_axialViewEnabledButton->setChecked( true );
    m_sagitalViewEnabledButton->setChecked( true );
    m_coronalViewEnabledButton->setChecked( true );
}


QMPR3DExtension::~QMPR3DExtension()
{
}

void QMPR3DExtension::createConnections()
{
    connect( m_axialViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setAxialVisibility(bool) ) );
    connect( m_sagitalViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setSagitalVisibility(bool) ) );
    connect( m_coronalViewEnabledButton , SIGNAL( toggled(bool) ) , m_mpr3DView , SLOT( setCoronalVisibility(bool) ) );
    
    connect( m_sagitalOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToSagital() ) );
    connect( m_coronalOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToCoronal() ) );
    connect( m_axialOrientationButton , SIGNAL( clicked() ) , m_mpr3DView , SLOT( resetViewToAxial() ) );

    connect( m_windowLevelComboBox , SIGNAL( windowLevel(double,double) ) , m_mpr3DView , SLOT( setWindowLevel(double,double) ) );
    connect( m_windowLevelComboBox , SIGNAL( defaultValue() ) , m_mpr3DView , SLOT( resetWindowLevelToDefault() ) );
}

void QMPR3DExtension::setInput( Volume *input )
{ 
    m_volume = input; 
    m_mpr3DView->setInput( m_volume );
    double wl[2];
    m_mpr3DView->getWindowLevel( wl );
    m_windowLevelComboBox->updateWindowLevel( wl[0] , wl[1] );
    INFO_LOG("QMPR3DExtension:: Donem Input ");
}

};  // end namespace udg 
