/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSYNCHRONIZETOOL_H
#define UDGSYNCHRONIZETOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class SynchronizeToolData;

/**
Tool de sincronització entre visualitzadors.

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class SynchronizeTool : public Tool
{
Q_OBJECT
public:

    SynchronizeTool( QViewer *viewer, QObject *parent = 0 );

    ~SynchronizeTool();

    void handleEvent( unsigned long eventID );

    virtual void setToolData( ToolData *data );

    virtual ToolData *getToolData() const;

private slots:

    void setIncrement( int slice );

    void applyChanges( );

private:

    /// Llesca anterior per saber l'increment
    int m_lastSlice;

    /// Dades de la tool
    SynchronizeToolData *m_toolData;

    Q2DViewer *m_q2viewer;
};

}

#endif
