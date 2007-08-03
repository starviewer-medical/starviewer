/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRIGHTMENUITEM_H
#define UDGRIGHTMENUITEM_H

#include <ui_rightmenuitembase.h>
#include <series.h>
#include <QObject>

namespace udg {

/**
Widget per utilitzar com a item de menú. Pot representar diversos objectes i te la funcionalitat de canviar l'aparença al situar-se el mouse sobre l'element.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RightMenuItem : public QFrame, public Ui::RightMenuItemBase
{
Q_OBJECT
public:

    RightMenuItem( QWidget *parent = 0 );

    ~RightMenuItem(){}

    void setSerie( Series * serie );

protected:

    /// Sobrecàrrega de l'event que s'emet quan el mouse entra dins l'àmbit de l'objecte
    void enterEvent( QEvent * event );

     /// Sobrecàrrega de l'event que s'emet quan el mouse entra dins l'àmbit de l'objecte
    void leaveEvent( QEvent * event );

    /// sobrecàrrega de l'event que esdevé quan es clica amb el mouse
    void mousePressEvent( QMouseEvent * event );

    QWidget * m_auxiliar;

    Series * m_serie;

signals:

    /// Aquest senyal s'emetrà quan el mouse entri al widget
    void isActive( int, QWidget * );

    /// Aquest senyal s'emet quan s'escull una serie de l'item
    void selectedSerie( Series * );
};

}

#endif
