/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensionworkspace.h"
#include <qpushbutton.h>
#include <iostream>
namespace udg {

ExtensionWorkspace::ExtensionWorkspace(QWidget *parent, const char *name)
 : QTabWidget(parent, name)
{
    m_closeTabButton = 0;
    
    this->setTabPosition( QTabWidget::Bottom );
    m_closeTabButton = new QPushButton( tr("X") , this );
    this->setCornerWidget( m_closeTabButton , Qt::BottomRight );
    connect( m_closeTabButton , SIGNAL( clicked() ), this , SLOT( closeCurrentApplication() ) );
    m_closeTabButton->hide();
}


ExtensionWorkspace::~ExtensionWorkspace()
{
}


void ExtensionWorkspace::addApplication( QWidget *application , QString caption )
{
    if( this->count() == 0 )
    {
        if( m_closeTabButton == 0 )
        {
            std::cout << "Creant per primer cop el botó de tancar :: " <<  std::endl;
            m_closeTabButton = new QPushButton( tr("X") , this );
        }
        m_closeTabButton->show();
    }
    std::cout << "Adding application" << std::endl;        
    this->addTab( application , caption );
    this->showPage( application );
}

void ExtensionWorkspace::removeApplication( QWidget *application )
{
    this->removePage( application );
    if( this->count() < 1 )
        m_closeTabButton->hide();
}

void ExtensionWorkspace::closeCurrentApplication()
{
    QWidget *w = this->currentPage();
    removeApplication( w );
    delete w;

}

};  // end namespace udg 
