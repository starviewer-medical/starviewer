/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPATIENTBROWERMENU_H
#define UDGPATIENTBROWERMENU_H

#include "patient.h"
#include "patientbrowsermenuextendeditem.h"
#include "patientbrowsermenulist.h"
#include "study.h"
#include "series.h"
#include <QMenu>

namespace udg {

/**
Classe que correspon al widget que es mostra al pulsar el botó dret del ratolí.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PatientBrowserMenu : public QObject
{
Q_OBJECT
public:
    PatientBrowserMenu();
    ~PatientBrowserMenu(){}

    /// Assignem una posició al menú
    void setPosition( QPoint point );

    /// Assignem un pacient per representar al menu
    void setPatient( Patient * patient );

signals:
    /// senyal que envia la serie escollida per ser visualitzada
    void selectedSeries( Series *series );
    void selectedSeries( QString uid );

public slots:
    /// Slot per actualitzar la posició a on es mostrara la informació auxiliar
    void showInformation( PatientBrowserMenuExtendedItem * extendedWidget , int y );

protected:

    /// Atribut que guarda el punter al menú basic que representa al pacient
    PatientBrowserMenuList * m_patientBasicList;

    /// Atribut que guarda el punter al menú amb informació addicional de la serie seleccionada
    PatientBrowserMenuExtendedItem * m_patientExtendedWidget;

private slots:

    void emitSelected( Series * serie );

};

}

#endif
