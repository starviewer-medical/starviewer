/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qdefaultviewerextension.h"

#include "volume.h"
#include <QMessageBox>

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
    changeViewToAxial();
    
}

void QDefaultViewerExtension::changeViewToAxial()
{
    m_spinBox->setMinimum( 0 );
    m_spinBox->setMaximum( m_mainVolume->getDimensions()[2] );
    m_slider->setMaximum(  m_mainVolume->getDimensions()[2] );
    m_slider->setValue( m_2DView->getSlice() );
    m_viewText->setText( tr("XY : Axial") );
    m_2DView->setViewToAxial();
    m_2DView->render();
}

void QDefaultViewerExtension::changeViewToSagital()
{
    m_spinBox->setMinimum( 0 );
    m_spinBox->setMaximum( m_mainVolume->getDimensions()[0] );
    m_slider->setMaximum(  m_mainVolume->getDimensions()[0] );
    m_slider->setValue( m_mainVolume->getDimensions()[0]/2 );
    m_viewText->setText( tr("YZ : Sagital") );
    m_2DView->setViewToSagittal();
    m_2DView->render();
}

void QDefaultViewerExtension::changeViewToCoronal()
{
    m_spinBox->setMinimum( 0 );
    m_spinBox->setMaximum( m_mainVolume->getDimensions()[1] );
    m_slider->setMaximum(  m_mainVolume->getDimensions()[1] );
    m_slider->setValue( m_mainVolume->getDimensions()[1]/2 );
    m_viewText->setText( tr("XZ : Coronal") );
    m_2DView->setViewToCoronal();
    m_2DView->render();
}

void QDefaultViewerExtension::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged(int) ) , m_spinBox , SLOT( setValue(int) ) );
    connect( m_spinBox , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );

    connect( m_axialViewToolButton , SIGNAL( clicked() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewToolButton , SIGNAL( clicked() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewToolButton , SIGNAL( clicked() ) , this , SLOT( changeViewToCoronal() ) );

    connect( m_windowLevelComboBox , SIGNAL( activated(int) ) , this , SLOT( changeDefaultWindowLevel( int ) ) );
}

void QDefaultViewerExtension::changeDefaultWindowLevel( int which )
{
    // amb una crida n'hi ha prou ja que els wW/WL estan sincronitzats
    switch( which )
    {
    case 0:
        m_2DView->resetWindowLevelToDefault();
    break;

    case 1:
        m_2DView->resetWindowLevelToBone();
    break;

    case 2:
        m_2DView->resetWindowLevelToLung();
    break;

    case 3:
        m_2DView->resetWindowLevelToSoftTissuesNonContrast();
    break;

    case 4:
        m_2DView->resetWindowLevelToLiverNonContrast();
    break;

    case 5:
        m_2DView->resetWindowLevelToSoftTissuesContrastMedium();
    break;

    case 6:
        m_2DView->resetWindowLevelToLiverContrastMedium();
    break;

    case 7:
        m_2DView->resetWindowLevelToNeckContrastMedium();
    break;

    case 8:
        m_2DView->resetWindowLevelToAngiography();
    break;

    case 9:
        m_2DView->resetWindowLevelToOsteoporosis();
    break;

    case 10:
        m_2DView->resetWindowLevelToEmphysema();
    break;
    
    case 11:
        m_2DView->resetWindowLevelToPetrousBone();
    break;

    case 12:
        // custom
        QMessageBox::information( this , tr("Information") , tr("Custom Window/Level Functions are not yet available") , QMessageBox::Ok );
    break;

    default:
        m_2DView->resetWindowLevelToDefault();
    break;
    
    }
}

}
