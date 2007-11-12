/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOL_H
#define UDGTOOL_H

#include <QObject>

class QAction;

namespace udg {

class ToolConfiguration;
class ToolData;

/**
Classe base per a totes les tools

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Tool : public QObject
{
Q_OBJECT
public:
    Tool(QObject *parent = 0);
    ~Tool();

    /**
     * Li assignem una configuracio, si la que te per defecte no ens val
     * @param configuration Configuracio que li volem assignar
     */
    void setConfiguration( ToolConfiguration *configuration );

    /**
     * Retorna la configuracio actual de la tool
     * TODO hauria de ser const o no?
     */
    ToolConfiguration *getConfiguration() const;

protected:
    /// Configuracio de la tool
    ToolConfiguration *m_toolConfiguration;

    /// Dades de la tool
    ToolData *m_toolData;

    /// Indica si les seves dades hauran de ser o no compartides
    bool m_hasSharedData;

    /// Nom de la tool TODO podem fer servir QMetaObject::className()? i ens estalviem aquesta variable?
    QString m_toolName;

};

}

#endif
