/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRESTORETOOL_H
#define UDGRESTORETOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;

/**
Tool de restaurar a l'estat inicial un visualitzador.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class RestoreTool : public Tool
{
Q_OBJECT
public:

    RestoreTool( QViewer *viewer, QObject *parent = 0 );

    ~RestoreTool();

    void handleEvent( unsigned long eventID );

private:
    /// Visualitzador 2d al que pertany la tool
    Q2DViewer *m_2DViewer;
};

}

#endif
