/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLREGISTRY_H
#define UDGTOOLREGISTRY_H

#include <QObject>
#include <QMap>

class QAction;

namespace udg {

class Tool;
class QViewer;

/**
Registre de Tools i elements associats

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolRegistry : public QObject
{
Q_OBJECT
public:
    ToolRegistry(QObject *parent = 0);

    ~ToolRegistry();

    /**
     * Ens crea la tool demanada i li assigna el viewer donat
     * @param toolName
     * @param viewer
     * @return
     */
    Tool *getTool( const QString &toolName, QViewer *viewer );

    /**
     * Crea una acció vàlida per aquella tool
     * @param toolName El nom de la tool de la qual volem l'acció
     * @return L'acció de la tool demanada, nul si la tool no existeix TODO o millor una QAction buida?
     */
    QAction *getToolAction( const QString &toolName );

};

}

#endif
