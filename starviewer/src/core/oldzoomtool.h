/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGOLDZOOMTOOL_H
#define UDGOLDZOOMTOOL_H

#include "oldtool.h"

namespace udg {

class QViewer;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class OldZoomTool : public OldTool
{
Q_OBJECT
public:
    enum { NONE , ZOOMING };
    OldZoomTool( QViewer *viewer, QObject *parent = 0 );

    ~OldZoomTool();

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
