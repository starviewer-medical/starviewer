/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWERTOOLMANAGER_H
#define UDGQ2DVIEWERTOOLMANAGER_H

#include "toolmanager.h"

namespace udg {

class Q2DViewer;

/**
Tool Manager per al visor Q2DViewer

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class Q2DViewerToolManager : public ToolManager
{
Q_OBJECT
public:
    Q2DViewerToolManager(QObject *parent = 0);
    Q2DViewerToolManager( Q2DViewer *viewer, QObject *parent = 0);

    ~Q2DViewerToolManager();

    /// Li assignem el visor
    void setViewer( Q2DViewer *viewer );
    
    /// Factory method heretat. Crearà la tool demanada si existeix, 0 altrament.
    Tool *createTool( QString toolName );
    
public slots:
    virtual void forwardEvent( unsigned long eventID );

    virtual bool setCurrentTool( QString toolName );

private:
    /// Instància del visor sobre el qual estem treballant
    Q2DViewer *m_viewer;

    void initToolRegistration();
};

}

#endif
