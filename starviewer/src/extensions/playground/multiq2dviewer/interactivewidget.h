/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGINTERACTIVEWIDGET_H
#define UDGINTERACTIVEWIDGET_H

#include <ui_interactivewidgetbase.h>
#include "patientitemmodel.h"
#include "rightbuttonmenu.h"
#include <QTreeView>
#include <QTableView>
#include <QListView>

#include <QStandardItemModel>

namespace udg {

/**
Classe que representa una vista de model/view programming de les qt's per tal de representar les dades dels estudis dels pacients a partir d'un widget més interactiu.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Volume;


class InteractiveWidget : public Ui::InteractiveWidgetBase, public QTreeView
{
public:
    InteractiveWidget( QWidget *parent = 0 );

    ~InteractiveWidget();

    void setVolume( Volume * volume);
//     void setModel( QStandardItemModel * model );

    RightButtonMenu * buttonMenu;

protected:
    /// Sobrecàrrega de l'event que s'emet quan el mouse entra dins l'àmbit de l'objecte
    void enterEvent( QEvent * event );
     /// Sobrecàrrega de l'event que s'emet quan el mouse entra dins l'àmbit de l'objecte
    void leaveEvent( QEvent * event );

    /// Mètode de proves
//     void mousePressEvent(QMouseEvent *event);
};

}

#endif
