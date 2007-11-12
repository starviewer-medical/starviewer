/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ3DVIEWERTOOLMANAGER_H
#define UDGQ3DVIEWERTOOLMANAGER_H

#include "oldtoolmanager.h"

namespace udg {

class Q3DViewer;

/**
Tool Manager pel visor Q3DViewer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q3DViewerToolManager : public OldToolManager
{
Q_OBJECT
public:
    Q3DViewerToolManager(QObject *parent = 0);
    Q3DViewerToolManager( Q3DViewer *viewer, QObject *parent = 0);

    ~Q3DViewerToolManager();

    /// Li assignem el visor \TODO aquest mètode podria quedar \deprecated obligant a proporcionar el visor en el moment de la construcció i prou, eliminant el constructor per defecte
    void setViewer( Q3DViewer *viewer );

private:
    /// Instància del visor sobre el qual estem treballant
    Q3DViewer *m_viewer;

    void initToolRegistration();

};

}

#endif
