/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROTATE3DTOOL_H
#define UDGROTATE3DTOOL_H

#include "oldtool.h"

class vtkInteractorStyle;

namespace udg {

class Q3DViewer;
class Q3DMPRViewer;

/**
Eina per rotacions tridimensionals ( pensat per visors 3D )

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Rotate3DTool : public OldTool
{
Q_OBJECT
public:

    enum { NONE , ROTATING };
    Rotate3DTool( Q3DViewer *viewer, QObject *parent = 0 );
    Rotate3DTool( Q3DMPRViewer *viewer, QObject *parent = 0 );

    ~Rotate3DTool();

    void handleEvent( unsigned long eventID );

private:
    /// interactor style que omplirem en el constructor depenent del visor
    vtkInteractorStyle *m_interactorStyle;

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença el translate
    void startRotate3D();

    /// Calcula el nou translate
    void doRotate3D();

    /// Atura l'estat de translate
    void endRotate3D();
};

}

#endif
