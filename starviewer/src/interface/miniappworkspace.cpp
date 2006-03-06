/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extensionworkspace.h"
// #include <qworkspace.h>
//#include "qmutualinformationtab.h"
// #include "qtabaxisview.h"
#include <qpushbutton.h>
#include <iostream>
namespace udg {

ExtensionWorkspace::ExtensionWorkspace(QWidget *parent, const char *name)
 : QTabWidget(parent, name)
{
    m_closeTabButton = 0;
    
    this->setTabPosition( QTabWidget::Bottom );
//      QTabAxisView *axisView = new QTabAxisView( 0 );
//      this->addApplication( axisView );
//\TODO:fer que el botó nom surti quan hi hagi alguna pestanya
// l'estratègia potser seria que quan hi ha un sol widget no hi ha cap pestanya ( és a dir és un sol widget, tal qual ) i quan tenim +1 llavors creem el tabwidget
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
//        std::cout << "Comptatge :: " << this->count() << std::endl;
        if( m_closeTabButton == 0 )
        {
            std::cout << "Creant per primer cop el botó de tancar :: " <<  std::endl;
            m_closeTabButton = new QPushButton( tr("X") , this );
        }
        //this->setCornerWidget( m_closeTabButton , Qt::BottomRight );
        m_closeTabButton->show();
    }
        
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
//     m_rightTab->removePage(w);
    removeApplication( w );
    delete w;
//     std::cout << "Comptatge :: " << this->count() << std::endl;
//     if (m_rightTab->count()==0)
//         delete m_rightTab;
}

};  // end namespace udg {
