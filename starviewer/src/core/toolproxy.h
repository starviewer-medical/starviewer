/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLPROXY_H
#define UDGTOOLPROXY_H

#include <QObject>
#include <QMap>

//forward declarations
class QString;

namespace udg {

//forward declarations space udg
class Tool;

/**
Classe encarregada de mantenir les tools actives per un visualitzador i de proporcionar-lis els events corresponents.
Les seves tasques són:
    - Rep tots els events d'un viewer
    - Coneix les tools actives per a un determinat viewer
    - Passa els events rebuts a totes les tools actives
    - Està associat a un viewer com atribut d'aquest
    - Guarda el nom de cadascuna de les tools

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolProxy : public QObject
{
Q_OBJECT
public:
    ToolProxy(QObject *parent = 0);

    ~ToolProxy();

    /**
     * Afegeix una tool al registre intern. Aquesta tool escoltarà els events
     * que se li comuniquin mitjançant forward event
     * @param tool
     */
    void addTool( Tool *tool );

    /**
     * Donat un nom de tool, l'elimina del registre.
     * @param toolName Nom de la tool a eliminar del proxy
     * @return Cert si la tenia, fals altrament
     */
    bool removeTool( QString toolName );

    /**
     * Buida i elimina totes les tools del proxy
     */
    void removeAllTools();

public slots:
    ///Avalua l'event que ha rebut del visualitzador al que està associat i l'envia a les tools actives
    void forwardEvent( unsigned long eventID );

private:
     /// map on hi guardem les tools associades a una clau determinada pel nom de la tool.
     /// en principi no es poden tenir dues tools amb el mateix nom
    QMap< QString, Tool* > m_toolsMap;
};

}

#endif
