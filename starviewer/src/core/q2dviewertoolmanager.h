/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERTOOLMANAGER_H
#define UDGQ2DVIEWERTOOLMANAGER_H

#include "oldtoolmanager.h"

namespace udg {

class Q2DViewer;

/**
Tool Manager per al visor Q2DViewer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerToolManager : public OldToolManager
{
Q_OBJECT
public:
    Q2DViewerToolManager(QObject *parent = 0);
    Q2DViewerToolManager( Q2DViewer *viewer, QObject *parent = 0 );

    ~Q2DViewerToolManager();

    /// Li assignem el visor \TODO aquest mètode podria quedar \deprecated obligant a proporcionar el visor en el moment de la construcció i prou, eliminant el constructor per defecte
    void setViewer( Q2DViewer *viewer );

private:
    /// Instància del visor sobre el qual estem treballant
    Q2DViewer *m_viewer;

    void initToolRegistration();
};

}

#endif
