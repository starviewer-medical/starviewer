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
#include "identifier.h"

namespace udg{

class ExtensionContext;

/**
Classe abstracta que fa de pont entre l'starviewer i una extensió. D'aquesta classe hauran d'heredar tots 
els mediators de les diferents extensions. Hi ha d'haver un mediator per cada extensió.
La vida d'un ExtensionMediator és la mateixa que la de la seva extensió. Per això queda lligada a ell
mitjançants un parentiu.
S'instancia un objecte Mediator per cada objecte Extension.
Classe "mare" de l'Extensió. És l'única que enten a l'Extensió i sap on es troba, com tractar-la... Alhora
"totes les mares són iguals". I els fills no coneixen a les mares.

\TODO Cal revisar tot aquest esquema. Ara és temporal per poder separar en directoris a l'espera del "Nou Disseny(tm)"

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ExtensionMediator : public QObject
{
Q_OBJECT
public:
    ExtensionMediator(QObject *parent = 0);

    virtual ~ExtensionMediator();

    /**
     * Mètode que ens serveix per, un cop creada l'extensió, inicialitzar-la amb els paràmetres necessàris a partir del seu contexte.
     * Per poder tractar l'extensió, el primer que caldrà serà realitzar un cast de QWidget a la classe concreta
     * del widget que se'ns passa.
     * @return Retorna false en el supòsit que hi hagi alguna cosa que impedeixi inicialitzar-la, true en la resta de casos
     */
    virtual bool initializeExtension(QWidget* extension, const ExtensionContext &extensionContext) = 0;

    ///Retorna l'identificador de la classe Extension amb qui dialoga
    virtual DisplayableID getExtensionID() const = 0;
};

}

#endif
