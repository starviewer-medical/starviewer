/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "itemmenu.h"

namespace udg {

ItemMenu::ItemMenu( QWidget * parent )
 : QWidget( parent )
{
    setAutoFillBackground( true );
}

ItemMenu::~ItemMenu()
{

}

bool ItemMenu::event( QEvent * event )
{

    if ( event->type() == QEvent::Enter )
    {
        QPalette palette = this->palette();
        QBrush selected( QColor( 85, 160, 255, 128 ) );
        selected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, selected );
        setPalette( palette );
        return true;
    }
    else if ( event->type() == QEvent::MouseButtonPress )
    {
        QPalette palette = this->palette();
        QBrush selected( QColor( 255, 0, 0, 255 ) );
        selected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, selected );
        setPalette( palette );
        return true;
    }
    else if ( event->type() == QEvent::Leave )
    {
        QPalette palette = this->palette();
        QBrush selected( QColor(239, 243, 247, 255) );
        selected.setStyle( Qt::SolidPattern );
        palette.setBrush( QPalette::Active, QPalette::Window, selected );
        setPalette( palette );
        return true;
    }
    else
    {
        return QWidget::event( event );
    }
}


}
