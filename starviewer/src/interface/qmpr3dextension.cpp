/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qmpr3dextension.h"
#include "q3dmprviewer.h"
#include "qcustomwindowleveldialog.h"
#include "logging.h"
#include <QToolButton>
#include <QMessageBox>

namespace udg {

QMPR3DExtension::QMPR3DExtension( QWidget *parent )
 : QWidget( parent )
{
    setupUi( this );
    
    m_customWindowLevelDialog = new QCustomWindowLevelDialog;

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
    connect( m_customWindowLevelDialog , SIGNAL( windowLevel( double,double) ) , m_mpr3DView , SLOT( setWindowLevel( double , double ) ) );
}

void QMPR3DExtension::setInput( Volume *input )
{ 
    m_volume = input; 
    m_mpr3DView->setInput( m_volume );
    INFO_LOG("QMPR3DExtension:: Donem Input ");
}

void QMPR3DExtension::changeDefaultWindowLevel( int which )
{
    switch( which )
    {
    case 0:
        m_mpr3DView->resetWindowLevelToDefault();
        INFO_LOG("QMPR3DExtension:: Canviem window level a DEFAULT");
    break;

    case 1:
        m_mpr3DView->resetWindowLevelToBone();
        INFO_LOG("QMPR3DExtension:: Canviem window level a Bone");
    break;

    case 2:
        m_mpr3DView->resetWindowLevelToLung();
        INFO_LOG("QMPR3DExtension:: Canviem window level a Lung");
    break;

    case 3:
        m_mpr3DView->resetWindowLevelToSoftTissuesNonContrast();
        INFO_LOG("QMPR3DExtension:: Canviem window level a SoftTissuesNC");
    break;

    case 4:
        m_mpr3DView->resetWindowLevelToLiverNonContrast();
        INFO_LOG("QMPR3DExtension:: Canviem window level a LiverNC");
    break;

    case 5:
        m_mpr3DView->resetWindowLevelToSoftTissuesContrastMedium();
        INFO_LOG("QMPR3DExtension:: Canviem window level a SoftTissuesCM");
    break;

    case 6:
        m_mpr3DView->resetWindowLevelToLiverContrastMedium();
        INFO_LOG("QMPR3DExtension:: Canviem window level a LiverCM");
    break;

    case 7:
        m_mpr3DView->resetWindowLevelToNeckContrastMedium();
        INFO_LOG("QMPR3DExtension:: Canviem window level a NeckCM");
    break;

    case 8:
        m_mpr3DView->resetWindowLevelToAngiography();
        INFO_LOG("QMPR3DExtension:: Canviem window level a Angiography");
    break;

    case 9:
        m_mpr3DView->resetWindowLevelToOsteoporosis();
        INFO_LOG("QMPR3DExtension:: Canviem window level a Osteoporosis");
    break;

    case 10:
        m_mpr3DView->resetWindowLevelToEmphysema();
        INFO_LOG("QMPR3DExtension:: Canviem window level a Emphysema");
    break;

    case 11:
        m_mpr3DView->resetWindowLevelToPetrousBone();
        INFO_LOG("QMPR3DExtension:: Canviem window level a PetrousBone");
    break;

    case 12:
        // custom
        m_customWindowLevelDialog->exec();
        INFO_LOG("QMPR3DExtension:: Canviem window level a custpom");
    break;

    default:
        m_mpr3DView->resetWindowLevelToDefault();
        INFO_LOG("QMPR3DExtension:: Canviem window level a DEFAULT");
    break;
    
    }
}

};  // end namespace udg 
