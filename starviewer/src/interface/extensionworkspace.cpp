/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensionworkspace.h"

#include <QToolButton>
#include <iostream>
#include "logging.h"

namespace udg {

ExtensionWorkspace::ExtensionWorkspace(QWidget *parent, QString name)
 : QTabWidget( parent )
{
    this->setObjectName( name );
    m_closeTabButton = 0;

    this->setTabPosition( QTabWidget::South );
    m_closeTabButton = new QToolButton( this );
    m_closeTabButton->setIcon( QIcon(":/images/tabRemove.png") );
    m_closeTabButton->setAutoRaise( true );
    m_closeTabButton->setIconSize( QSize(24,24) );
    this->setCornerWidget( m_closeTabButton , Qt::BottomRightCorner );
    createConnections();
    m_closeTabButton->hide();
}

ExtensionWorkspace::~ExtensionWorkspace()
{
}

void ExtensionWorkspace::createConnections()
{
    connect( m_closeTabButton , SIGNAL( clicked() ), this , SLOT( closeCurrentApplication() ) );
}

void ExtensionWorkspace::addApplication( QWidget *application , QString caption )
{
    if( this->count() == 0 )
    {
        m_closeTabButton->show();
    }
    STAT_LOG( "Obrim extensió: " + caption );
    this->addTab( application , caption );
    this->setCurrentIndex( this->indexOf( application ) );
}

void ExtensionWorkspace::removeApplication( QWidget *application )
{
    if( application )
    {
        INFO_LOG( "Tancant extensió: " + application->objectName() );
        this->removeTab( this->indexOf( application ) );
        if( this->count() < 1 )
            m_closeTabButton->hide();
    }
    else
    {
        DEBUG_LOG( "S'ha donat una widget nul per eliminar" );
    }
}

void ExtensionWorkspace::killThemAll()
{
    int numberOfExtensions = this->count();
    for( int i = numberOfExtensions-1; i >=0; i-- )
    {
        QWidget *currentExtension = this->widget(i);
        removeApplication( currentExtension );
        delete currentExtension;
    }
}

void ExtensionWorkspace::closeCurrentApplication()
{
    QWidget *w = this->currentWidget();
    removeApplication( w );
    delete w;
}

};  // end namespace udg
