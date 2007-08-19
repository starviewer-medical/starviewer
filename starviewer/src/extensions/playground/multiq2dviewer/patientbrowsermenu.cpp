/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "patientbrowsermenu.h"

#include <QDesktopWidget>
#include <QApplication>

#include "patient.h"
#include "patientbrowsermenuextendeditem.h"
#include "patientbrowsermenulist.h"
#include "series.h"
#include "logging.h"

namespace udg {

PatientBrowserMenu::PatientBrowserMenu( )
{
    m_patientExtendedWidget = new PatientBrowserMenuExtendedItem();
    m_patientExtendedWidget->hide();
    m_patientBasicList = 0;
}

void PatientBrowserMenu::setPatient( Patient * patient )
{
    // 1.-Eliminar la informació actual
    if( m_patientBasicList != 0 ) delete m_patientBasicList;
    m_patientBasicList = new PatientBrowserMenuList();
    m_patientExtendedWidget->hide();

    // 2.- Crear el menu amb el nou pacient
    m_patientBasicList->setPatient( patient );
    m_patientBasicList->hide();

    // 3.- Creem les connexions
    connect( m_patientBasicList , SIGNAL( isActive( int, Series * ) ) , m_patientExtendedWidget , SLOT( showSerie( int , Series * ) ) );

    connect( m_patientBasicList , SIGNAL( isNotActive() ) , m_patientExtendedWidget , SLOT( hide() ) );

    connect( m_patientExtendedWidget , SIGNAL( setPosition( PatientBrowserMenuExtendedItem * , int ) ) , this, SLOT ( showInformation( PatientBrowserMenuExtendedItem * , int ) ) );

    connect( m_patientBasicList , SIGNAL( selectedSerie( Series * ) ) , this , SLOT ( emitSelected( Series * ) ) );

}

void PatientBrowserMenu::showInformation( PatientBrowserMenuExtendedItem * extendedWidget , int y )
{

    int x;
    int screen_x= qApp->desktop()->availableGeometry().width();

    extendedWidget->show();// Si no fem el show en aquest punt, les mides que s'obtenen no són correctes.

    // Calcular si hi cap a la dreta, altrament el mostrarem a l'esquerre del menu
    if( (m_patientBasicList->x() + m_patientBasicList->width() + extendedWidget->width() ) > screen_x )
    {
        x = ( m_patientBasicList->geometry().x() ) - ( extendedWidget->width() );
    }
    else
    {
        x =  m_patientBasicList->x() + m_patientBasicList->width();
    }
    extendedWidget->move( x, m_patientBasicList->y() );
}

void PatientBrowserMenu::setPosition( const QPoint &point )
{
    // Calcular si el menu hi cap a la pantalla
    int x = point.x();
    int y = point.y();
    
    int screen_x = qApp->desktop()->availableGeometry().width();
    int screen_y = qApp->desktop()->availableGeometry().height();

    m_patientBasicList->show();// Si no fem el show en aquest punt, les mides obtingudes no son correctes.

    if ( ( x + m_patientBasicList->width() ) > screen_x )
    {
        x = screen_x - m_patientBasicList->width() - 5;
    }

    if ( ( y + m_patientBasicList->height() ) > screen_y )
    {
        y = screen_y - m_patientBasicList->height() - 5;
    }

    // moure la finestra del menu al punt que toca
    m_patientBasicList->move( QPoint(x,y) );
    m_patientBasicList->show();

}

void PatientBrowserMenu::emitSelected( Series * serie )
{
    m_patientBasicList->hide();
    m_patientExtendedWidget->hide();
    emit selectedSeries( serie );
}

}
