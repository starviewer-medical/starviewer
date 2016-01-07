/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGDISPLAYABLE_H
#define UDGDISPLAYABLE_H

#include <QList>
#include <QString>

namespace udg {

/**
    Identificador que es fa servir en llocs on es necessita un identificador que l'usuari podrà veure per pantalla. Aquest té una part fixa, invariable, que és la
    que realment es fa servir com a identificador, internament. També té una part que serveix per poder-la mostrar a l'interfície i que es pugui traduir.
    Per exemple, els noms de les Extensions.

    No té "setters" expressament.
    Exemple d'assignacions i creació d'id's nuls:
    @code
    DisplayableID id(QString::null);

    DisplayableID id2("test", tr("test"));

    id = DisplayableID("nouValor",tr("nouValor"));

    id = id2;
    @endcode

  */
class DisplayableID {

public:
    DisplayableID(const QString &id, const QString &name = QString::null);

    ~DisplayableID();

    QString getID() const
    {
        return m_id;
    }
    QString getLabel() const
    {
        return m_name;
    }

    friend inline bool operator==(const DisplayableID &, const DisplayableID &);
    friend inline bool operator!=(const DisplayableID &, const DisplayableID &);
    friend inline bool operator<(const DisplayableID &, const DisplayableID &);
    friend inline bool operator>(const DisplayableID &, const DisplayableID &);

private:
    QString m_id;
    QString m_name;

};

typedef QList<DisplayableID> DisplayableIDList;

}

#endif
