/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWEREXTENSIONCREATOR_H
#define UDGQ2DVIEWEREXTENSIONCREATOR_H

#include "extensioncreator.h"

namespace udg {

/**
Creador de l'extensió Q2dViewerExtension

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerExtensionCreator : public ExtensionCreator
{
Q_OBJECT
public:
    Q2DViewerExtensionCreator( QObject* parent = 0, const char* name = 0 );

    ~Q2DViewerExtensionCreator();

    /// creador d'extensions. és el mètode del facory encarregat de proporcionar l'extensió que demanem
    /// \TODO ara per ara retorna QWidget, però és possible que canviem a una altra classe que faci d'interfície per a les extensions [p.ex. ExtensionBase]
    /// aquest mètode és purament abstracte en el pare
    virtual QWidget *createExtension( QWidget *parent , QString name );
    /// inicialitzador d'extensions
    /// \TODO ídem anterior amb QWidget
    /// aquest mètode és purament abstracte en el pare
    virtual bool initializeExtension( QWidget *extension  /* , Resource *rsrc */);
};

}

#endif
