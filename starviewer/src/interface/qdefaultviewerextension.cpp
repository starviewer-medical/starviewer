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
    // \TODO ara ho fem "a saco" però s'hauria de millorar
    m_2DView->setInput( m_mainVolume );
    m_2DView2_1->setInput( m_mainVolume );
    m_2DView2_2->setInput( m_mainVolume );
    changeViewToAxial();
    
}

void QDefaultViewerExtension::changeViewToAxial()
{
    m_currentView = Axial;
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( 0 );
        m_spinBox->setMaximum( m_mainVolume->getDimensions()[2] );
        m_slider->setMaximum(  m_mainVolume->getDimensions()[2] );
        m_slider->setValue( m_2DView->getSlice() );
        m_viewText->setText( tr("XY : Axial") );
        m_2DView->setViewToAxial();
        m_2DView->render();
    break;

    case 1:
        m_spinBox2_1->setMinimum( 0 );
        m_spinBox2_1->setMaximum( m_mainVolume->getDimensions()[2] );
        m_slider2_1->setMaximum(  m_mainVolume->getDimensions()[2] );
        m_slider2_1->setValue( m_2DView2_1->getSlice() );
        m_viewText2_1->setText( tr("XY : Axial") );
        m_2DView2_1->setViewToAxial();
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( 0 );
        m_spinBox2_2->setMaximum( m_mainVolume->getDimensions()[2] );
        m_slider2_2->setMaximum(  m_mainVolume->getDimensions()[2] );
        m_slider2_2->setValue( m_2DView2_2->getSlice() );
        m_viewText2_2->setText( tr("XY : Axial") );
        m_2DView2_2->setViewToAxial();
        m_2DView2_2->render();
    break;
    
    }

}

void QDefaultViewerExtension::changeViewToSagital()
{
    m_currentView = Sagital;
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( 0 );
        m_spinBox->setMaximum( m_mainVolume->getDimensions()[0] );
        m_slider->setMaximum(  m_mainVolume->getDimensions()[0] );
        m_slider->setValue( m_mainVolume->getDimensions()[0]/2 );
        m_viewText->setText( tr("YZ : Sagital") );
        m_2DView->setViewToSagittal();
        m_2DView->render();
    break;

    case 1:
        m_spinBox2_1->setMinimum( 0 );
        m_spinBox2_1->setMaximum( m_mainVolume->getDimensions()[0] );
        m_slider2_1->setMaximum(  m_mainVolume->getDimensions()[0] );
        m_slider2_1->setValue( m_2DView2_1->getSlice() );
        m_viewText2_1->setText( tr("YZ : Sagital") );
        m_2DView2_1->setViewToSagittal();
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( 0 );
        m_spinBox2_2->setMaximum( m_mainVolume->getDimensions()[0] );
        m_slider2_2->setMaximum(  m_mainVolume->getDimensions()[0] );
        m_slider2_2->setValue( m_2DView2_2->getSlice() );
        m_viewText2_2->setText( tr("YZ : Sagital") );
        m_2DView2_2->setViewToSagittal();
        m_2DView2_2->render();
    break;
    }
}

void QDefaultViewerExtension::changeViewToCoronal()
{
    m_currentView = Coronal;
    switch( m_stackedWidget->currentIndex() )
    {
    case 0:
        m_spinBox->setMinimum( 0 );
        m_spinBox->setMaximum( m_mainVolume->getDimensions()[1] );
        m_slider->setMaximum(  m_mainVolume->getDimensions()[1] );
        m_slider->setValue( m_mainVolume->getDimensions()[1]/2 );
        m_viewText->setText( tr("XZ : Coronal") );
        m_2DView->setViewToCoronal();
        m_2DView->render();
    break;
    
    case 1:
        m_spinBox2_1->setMinimum( 0 );
        m_spinBox2_1->setMaximum( m_mainVolume->getDimensions()[1] );
        m_slider2_1->setMaximum(  m_mainVolume->getDimensions()[1] );
        m_slider2_1->setValue( m_2DView2_1->getSlice() );
        m_viewText2_1->setText( tr("XZ : Coronal") );
        m_2DView2_1->setViewToCoronal();
        m_2DView2_1->render();

        m_spinBox2_2->setMinimum( 0 );
        m_spinBox2_2->setMaximum( m_mainVolume->getDimensions()[1] );
        m_slider2_2->setMaximum(  m_mainVolume->getDimensions()[1] );
        m_slider2_2->setValue( m_2DView2_2->getSlice() );
        m_viewText2_2->setText( tr("XZ : Coronal") );
        m_2DView2_2->setViewToCoronal();
        m_2DView2_2->render();
    break;
    }
}

void QDefaultViewerExtension::createConnections()
{
    connect( m_slider , SIGNAL( valueChanged(int) ) , m_spinBox , SLOT( setValue(int) ) );
    connect( m_spinBox , SIGNAL( valueChanged(int) ) , m_2DView , SLOT( setSlice(int) ) );
    connect( m_2DView , SIGNAL( sliceChanged(int) ) , m_slider , SLOT( setValue(int) ) );

    // adicionals, \TODO ara es fa "a saco" però s'ha de millorar
    connect( m_slider2_1 , SIGNAL( valueChanged(int) ) , m_spinBox2_1 , SLOT( setValue(int) ) );
    connect( m_spinBox2_1 , SIGNAL( valueChanged(int) ) , m_2DView2_1 , SLOT( setSlice(int) ) );
    connect( m_2DView2_1 , SIGNAL( sliceChanged(int) ) , m_slider2_1 , SLOT( setValue(int) ) );

    connect( m_slider2_2 , SIGNAL( valueChanged(int) ) , m_spinBox2_2 , SLOT( setValue(int) ) );
    connect( m_spinBox2_2 , SIGNAL( valueChanged(int) ) , m_2DView2_2 , SLOT( setSlice(int) ) );
    connect( m_2DView2_2 , SIGNAL( sliceChanged(int) ) , m_slider2_2 , SLOT( setValue(int) ) );

    
    connect( m_axialViewToolButton , SIGNAL( clicked() ) , this , SLOT( changeViewToAxial() ) );
    connect( m_sagitalViewToolButton , SIGNAL( clicked() ) , this , SLOT( changeViewToSagital() ) );
    connect( m_coronalViewToolButton , SIGNAL( clicked() ) , this , SLOT( changeViewToCoronal() ) );

    connect( m_windowLevelComboBox , SIGNAL( activated(int) ) , this , SLOT( changeDefaultWindowLevel( int ) ) );

    connect( m_pageSelectorSpinBox , SIGNAL( valueChanged(int) ) , m_stackedWidget , SLOT( setCurrentIndex(int) ) );
    connect( m_stackedWidget , SIGNAL( currentChanged(int) ) , this , SLOT( pageChange(int) ) );
}

void QDefaultViewerExtension::changeDefaultWindowLevel( int which )
{
    // \TODO ara anem a saco però 'shauria de fer una manera perquè només es cridessin els que cal
    switch( which )
    {
    case 0:
        m_2DView->resetWindowLevelToDefault();
        m_2DView2_1->resetWindowLevelToDefault();
        m_2DView2_2->resetWindowLevelToDefault();
    break;

    case 1:
        m_2DView->resetWindowLevelToBone();
        m_2DView2_1->resetWindowLevelToBone();
        m_2DView2_2->resetWindowLevelToBone();
    break;

    case 2:
        m_2DView->resetWindowLevelToLung();
        m_2DView2_1->resetWindowLevelToLung();
        m_2DView2_2->resetWindowLevelToLung();
    break;

    case 3:
        m_2DView->resetWindowLevelToSoftTissuesNonContrast();
        m_2DView2_1->resetWindowLevelToSoftTissuesNonContrast();
        m_2DView2_2->resetWindowLevelToSoftTissuesNonContrast();
    break;

    case 4:
        m_2DView->resetWindowLevelToLiverNonContrast();
        m_2DView2_1->resetWindowLevelToLiverNonContrast();
        m_2DView2_2->resetWindowLevelToLiverNonContrast();
    break;

    case 5:
        m_2DView->resetWindowLevelToSoftTissuesContrastMedium();
        m_2DView2_1->resetWindowLevelToSoftTissuesContrastMedium();
        m_2DView2_2->resetWindowLevelToSoftTissuesContrastMedium();
    break;

    case 6:
        m_2DView->resetWindowLevelToLiverContrastMedium();
        m_2DView2_1->resetWindowLevelToLiverContrastMedium();
        m_2DView2_2->resetWindowLevelToLiverContrastMedium();
    break;

    case 7:
        m_2DView->resetWindowLevelToNeckContrastMedium();
        m_2DView2_1->resetWindowLevelToNeckContrastMedium();
        m_2DView2_2->resetWindowLevelToNeckContrastMedium();
    break;

    case 8:
        m_2DView->resetWindowLevelToAngiography();
        m_2DView2_1->resetWindowLevelToAngiography();
        m_2DView2_2->resetWindowLevelToAngiography();
    break;

    case 9:
        m_2DView->resetWindowLevelToOsteoporosis();
        m_2DView2_1->resetWindowLevelToOsteoporosis();
        m_2DView2_2->resetWindowLevelToOsteoporosis();
    break;

    case 10:
        m_2DView->resetWindowLevelToEmphysema();
        m_2DView2_1->resetWindowLevelToEmphysema();
        m_2DView2_2->resetWindowLevelToEmphysema();
    break;
    
    case 11:
        m_2DView->resetWindowLevelToPetrousBone();
        m_2DView2_1->resetWindowLevelToPetrousBone();
        m_2DView2_2->resetWindowLevelToPetrousBone();
    break;

    case 12:
        // custom
        QMessageBox::information( this , tr("Information") , tr("Custom Window/Level Functions are not yet available") , QMessageBox::Ok );
    break;

    default:
        m_2DView->resetWindowLevelToDefault();
        m_2DView2_1->resetWindowLevelToDefault();
        m_2DView2_2->resetWindowLevelToDefault();
    break;
    
    }
}

void QDefaultViewerExtension::setView( ViewType view )
{
    switch( view )
    {
    case Axial:
        changeViewToAxial();
    break;
    case Sagital:
        changeViewToSagital();
    break;
    case Coronal:
        changeViewToCoronal();
    break;
    }
}

void QDefaultViewerExtension::pageChange( int index )
{
    setView( m_currentView );
    switch( index )
    {
    case 0:
    break;

    case 1:
    break;
    }
}

}
