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

ExtensionWorkspace::ExtensionWorkspace(QWidget *parent, const char *name)
 : QTabWidget( parent )
{
    this->setObjectName( name );
    m_closeTabButton = 0;
    
    this->setTabPosition( QTabWidget::South );
    m_closeTabButton = new QToolButton( this );
    m_closeTabButton->setIcon( QIcon(":/images/fileclose.png") );
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
    INFO_LOG( qPrintable( "Afegint extensió: " + caption ) );
    this->addTab( application , caption );
    this->setCurrentIndex( this->indexOf( application ) );
    m_lastIndex = this->currentIndex();
}

void ExtensionWorkspace::removeApplication( QWidget *application )
{
    INFO_LOG( qPrintable( "Tancant extensió: " + application->objectName() ) );
    this->removeTab( this->indexOf( application ) );
    if( this->count() < 1 )
        m_closeTabButton->hide();
    m_lastIndex = this->currentIndex();
}

void ExtensionWorkspace::closeCurrentApplication()
{
    QWidget *w = this->currentWidget();
    removeApplication( w );
    delete w;
}

};  // end namespace udg 
