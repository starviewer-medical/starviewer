/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGWINDOWLEVELTOOL_H
#define UDGWINDOWLEVELTOOL_H

#include "tool.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/

//class Q2DViewer;
class QViewer;

class WindowLevelTool : public Tool
{
Q_OBJECT
public:
    /// estats de la tool
    enum { NONE , WINDOWLEVELING };

    WindowLevelTool( QViewer *viewer, QObject *parent = 0 );

    ~WindowLevelTool();

    void handleEvent( unsigned long eventID );

/// \TODO potser aquests mètodes slots passen a ser públics
private slots:
    /// Comença el window level
    void startWindowLevel();

    /// Calcula el nou window level
    void doWindowLevel();

    /// Atura l'estat de window level
    void endWindowLevel();

private:
    /// valors per controlar el mapeig del window level
    double m_initialWindow, m_initialLevel, m_currentWindow, m_currentLevel;
    int m_windowLevelStartPosition[2], m_windowLevelCurrentPosition[2];
    
    /// estats d'execució de la tool
    int m_state;
};

}

#endif
