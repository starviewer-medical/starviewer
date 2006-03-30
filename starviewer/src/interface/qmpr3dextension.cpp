/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextension.h"
#include "q3dmprviewer.h"
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

    connect( m_windowLevelAdjustmentComboBox , SIGNAL( activated(int) ) , this , SLOT( changeDefaultWindowLevel( int ) ) );
}

void QMPR3DExtension::setInput( Volume *input )
{ 
    m_volume = input; 
    m_mpr3DView->setInput( m_volume );
}

void QMPR3DExtension::changeDefaultWindowLevel( int which )
{
    switch( which )
    {
    case 0:
        m_mpr3DView->resetWindowLevelToDefault();
    break;

    case 1:
        m_mpr3DView->resetWindowLevelToBone();
    break;

    case 2:
        m_mpr3DView->resetWindowLevelToLung();
    break;

    case 3:
        m_mpr3DView->resetWindowLevelToSoftTissuesNonContrast();
    break;

    case 4:
        m_mpr3DView->resetWindowLevelToLiverNonContrast();
    break;

    case 5:
        m_mpr3DView->resetWindowLevelToSoftTissuesContrastMedium();
    break;

    case 6:
        m_mpr3DView->resetWindowLevelToLiverContrastMedium();
    break;

    case 7:
        m_mpr3DView->resetWindowLevelToNeckContrastMedium();
    break;

    case 8:
        m_mpr3DView->resetWindowLevelToAngiography();
    break;

    case 9:
        m_mpr3DView->resetWindowLevelToOsteoporosis();
    break;

    case 10:
        m_mpr3DView->resetWindowLevelToEmphysema();
    break;

    case 11:
        m_mpr3DView->resetWindowLevelToPetrousBone();
    break;

    case 12:
        // custom
        QMessageBox::information( m_mpr3DView , tr("Information") , tr("Custom Window/Level Functions are not yet available") , QMessageBox::Ok );
    break;

    default:
        m_mpr3DView->resetWindowLevelToDefault();
    break;
    
    }
}

};  // end namespace udg 
