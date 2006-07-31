/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef EXTENSIONMEDIATOR_H
#define EXTENSIONMEDIATOR_H

#include <QObject>
#include "displayableid.h"

namespace udg{

/**
Classe abstracta que fa de pont entre l'starviewer i una extensió. D'aquesta classe hauran d'heredar tots 
els mediators de les diferents extensions. Hi ha d'haver un mediator per cada extensió.
La vida d'un ExtensionMediator és la mateixa que la de la seva extensió. Per això queda lligada a ell
mitjançants un parentiu.
S'instancia un objecte Mediator per cada objecte Extension.
Classe "mare" de l'Extensió. És l'única que enten a l'Extensió i sap on es troba, com tractar-la... Alhora
"totes les mares són iguals". I els fills no coneixen a les mares.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ExtensionMediator : public QObject
{
Q_OBJECT
public:
    ExtensionMediator(QObject *parent = 0);

    virtual ~ExtensionMediator();

    virtual bool setExtension(QWidget* extension) = 0;

    ///Retorna l'identificador de la classe Extension amb qui dialoga
    virtual DisplayableID getExtensionID() const = 0;
};

}

#endif
