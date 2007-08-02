/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rightmenuitem.h"
#include "QPalette"
#include "QListWidgetItem"
#include "QLabel"
#include "QPixmap"
#include "logging.h"
#include "QVBoxLayout"

namespace udg {

RightMenuItem::RightMenuItem( QWidget *parent )
: QFrame(parent)
{
    setupUi( this );
    auxiliar = new QWidget( 0, Qt::Widget );
    auxiliar->setWindowFlags(Qt::Popup);
    auxiliar->hide();

    QVBoxLayout * verticalLayout = new QVBoxLayout( auxiliar );
    QLabel * icon = new QLabel( auxiliar );
    verticalLayout->addWidget( icon );
    icon->setPixmap( QPixmap( "/home/ester/starviewer/src/main/images/axial.png" ) );
    QLabel * text = new QLabel( auxiliar );
    verticalLayout->addWidget( text );
    text->setText( "info_auxiliar ");
    text->show();
    icon->show();
}


void RightMenuItem::enterEvent( QEvent * event )
{
//     QPalette palette = this->palette();
//     QBrush selected(QColor(0, 0, 0 , 255));
//     selected.setStyle(Qt::SolidPattern);
//     palette.setBrush(QPalette::Active, QPalette::Window, selected);
//     palette.setBrush(QPalette::Disabled, QPalette::Window, selected);
//     palette.setBrush(QPalette::Inactive, QPalette::Window, selected);
//     setPalette(palette);
//     show();
// 
//     QList<QObject*> QObjectList = children();
//     for ( int i = 0; i < QObjectList.size(); i++ )
//     {
//         if ( QObjectList.value(i)->isWidgetType() )
//         {
//             ( (QWidget *) QObjectList.value(i) )->setPalette( palette );
//             ( (QWidget *) QObjectList.value(i) )->show();
//         }
//     }
// 
//     show();

    setFrameShape( QFrame::StyledPanel );
    setFrameShadow( QFrame::Raised );

    emit isActive( this->geometry().y(), auxiliar );

    DEBUG_LOG( QString( "%1").arg(this->geometry().y() ) );
    DEBUG_LOG( QString( this->y() ) );

    auxiliar->show();

}

void RightMenuItem::leaveEvent( QEvent * event )
{
//     QPalette palette = this->palette();
//     QBrush selected(QColor(239, 243, 247, 255));
//     selected.setStyle(Qt::SolidPattern);
//     palette.setBrush(QPalette::Active, QPalette::Window, selected);
//     setPalette(palette);
    
    setFrameShape( QFrame::NoFrame );
    auxiliar->hide();
}


}
