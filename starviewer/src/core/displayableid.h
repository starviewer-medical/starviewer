/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGDISPLAYABLE_H
#define UDGVISIBLEID_H

#include <QList>
#include <QString>

namespace udg {

/**
Identificador que es fa servir en llocs on es necessita un identificador que l'usuari podrà veure per pantalla. Aquest té una part fixa, invariable, que és la que realment es fa servir com a identificador, internament. També té una part que serveix per poder-la mostrar a l'interfície i que es pugui traduir. Per exemple, els noms de les Extensions.

No té "setters" expressament.
Exemple d'assignacions i creació d'id's nuls:
@code
DisplayableID id(QString::null);

DisplayableID id2("test", tr("test"));

id = DisplayableID("nouValor",tr("nouValor"));

id = id2;
@endcode

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class DisplayableID {

public:

    DisplayableID(const QString & id, const QString & name = QString::null);

    ~DisplayableID();

    QString getID() const { return m_id; };
    QString getLabel() const { return m_name; };

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
