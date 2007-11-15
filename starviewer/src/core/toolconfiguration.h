/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTOOLCONFIGURATION_H
#define UDGTOOLCONFIGURATION_H

#include <QObject>

#include <QMap>
#include <QVariant>

namespace udg {

/**
Classe encarregada de definir els atributs que configuren una tool. Els guarda de forma genèrica.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ToolConfiguration : public QObject
{
Q_OBJECT
public:
    ToolConfiguration(QObject *parent = 0);

    ~ToolConfiguration();

    /**
    * Afegeix un atribut a la tool
    **/
    void addAttribute( QString attributeName, QVariant value );

    /**
    * Posa el valor a un atribut
    **/
    void setValue( QString attributeName, QVariant value );

    /**
    * Obté el valor d'un atribut
    **/
    QVariant getValue( QString attributeName );

private:

    /**
    * Map per guardar els atributs amb els seus corresponents valors
    **/
    QMap<QString, QVariant> attributeMap;
};

}

#endif
