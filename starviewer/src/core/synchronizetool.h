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

    /// Assignem una configuracio
    void setConfiguration( ToolConfiguration *configuration );

    /// Per posar dades (compartides)
    virtual void setToolData( ToolData *data );

    /// Per obtenir les dades ( per compartir )
    virtual ToolData *getToolData() const;

private slots:

    /// Posa l'increment a les dades
    void setIncrement( int slice );

    /// Posa el window level a les dades
    void setWindowLevel( double window, double level );

    /// Posa el factor de zoom
    void setZoomFactor( double factor );

    /// Aplica els canvis de canvi de llesca
    void applySliceChanges();

    /// Aplica els canvis del window level
    void applyWindowLevelChanges();

    /// Aplica els canvis de zoom
    void applyZoomFactorChanges();

private:

    /// Llesca anterior per saber l'increment
    int m_lastSlice;

    /// Thickness de la serie
    int m_thickness;

    /// Thickness perdut per arrodoniment
    double m_roundLostThickness;

    /// Dades de la tool
    SynchronizeToolData *m_toolData;

    /// Visualitzador 2d al que pertany la tool
    Q2DViewer *m_q2dviewer;
};

}

#endif
