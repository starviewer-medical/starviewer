/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGZOOMTOOL_H
#define UDGZOOMTOOL_H

#include "oldtool.h"

namespace udg {

class QViewer;
class Q2DViewer;
class Q3DViewer;
class Q3DMPRViewer;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ZoomTool : public OldTool
{
Q_OBJECT
public:
    enum { NONE , ZOOMING };
    ZoomTool( Q2DViewer *viewer, QObject *parent = 0 );
    ZoomTool( Q3DViewer *viewer, QObject *parent = 0 );
    ZoomTool( Q3DMPRViewer *viewer, QObject *parent = 0 );

    ~ZoomTool();

    void handleEvent( unsigned long eventID );

private:
    /// Viewer sobre el qual aplicarem el zoom
    QViewer *m_viewer;

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença el zoom
    void startZoom();

    /// Calcula el nou zoom
    void doZoom();

    /// Atura l'estat de zoom
    void endZoom();
};

}

#endif
