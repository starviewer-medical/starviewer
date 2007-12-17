/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGINTERACTIVEWIDGET_H
#define UDGINTERACTIVEWIDGET_H

#include <QTreeView>

namespace udg {

class PatientBrowserMenu;
class Patient;

/**
Classe que representa una vista de model/view programming de les qt's per tal de representar les dades dels estudis dels pacients a partir d'un widget més interactiu, que s'amaga automàticament.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Volume;


class InteractiveWidget : public QTreeView
{
    Q_OBJECT
public:
    InteractiveWidget( QWidget *parent = 0 );

    ~InteractiveWidget();

    void setPatient(Patient * patient);

    PatientBrowserMenu * buttonMenu;

protected:
    /// Sobrecàrrega de l'event que s'emet quan el mouse entra dins l'àmbit de l'objecte
    void enterEvent( QEvent * event );
     /// Sobrecàrrega de l'event que s'emet quan el mouse entra dins l'àmbit de l'objecte
    void leaveEvent( QEvent * event );
};

}

#endif
