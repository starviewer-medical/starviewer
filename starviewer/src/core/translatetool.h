/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGTRANSLATETOOL_H
#define UDGTRANSLATETOOL_H

#include "tool.h"

class vtkInteractorStyle;

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

class Q2DViewer;
class Q3DViewer;
class Q3DMPRViewer;

class TranslateTool : public Tool
{
Q_OBJECT
public:
    enum { NONE , TRANSLATING };
    TranslateTool( Q2DViewer *viewer , QObject *parent = 0, const char *name = 0 );
    TranslateTool( Q3DViewer *viewer , QObject *parent = 0, const char *name = 0 );
    TranslateTool( Q3DMPRViewer *viewer , QObject *parent = 0, const char *name = 0 );

    ~TranslateTool();

    void handleEvent( unsigned long eventID );

private:
    /// interactor style que omplirem en el constructor depenent del visor
    vtkInteractorStyle *m_interactorStyle;

    /// realitza la feina de desplaçament
    void pan();

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença el translate
    void startTranslate();

    /// Calcula el nou translate
    void doTranslate();

    /// Atura l'estat de translate
    void endTranslate();
};

}

#endif
