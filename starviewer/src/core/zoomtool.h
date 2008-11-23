/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGZOOMTOOL_H
#define UDGZOOMTOOL_H

#include "tool.h"

namespace udg {

class QViewer;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ZoomTool : public Tool
{
Q_OBJECT
public:
    enum { NONE , ZOOMING };
    ZoomTool( QViewer *viewer, QObject *parent = 0 );

    ~ZoomTool();

    void handleEvent( unsigned long eventID );

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença el zoom
    void startZoom();

    /// Calcula el nou zoom
    void doZoom();

    /// Atura l'estat de zoom
    void endZoom();

private:
    int m_state;
};

}

#endif
