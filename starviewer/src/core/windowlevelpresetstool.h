/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGWINDOWLEVELPRESETSTOOL_H
#define UDGWINDOWLEVELPRESETSTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
/**
Tool per canviar window levels predefinits

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class WindowLevelPresetsTool : public Tool
{
Q_OBJECT
public:
    WindowLevelPresetsTool( QViewer *viewer, QObject *parent = 0 );

    ~WindowLevelPresetsTool();

    void handleEvent( unsigned long eventID );

private:
    /**
     * Aplica el window level predeterminat per la tecla donada
     * @param key caràcter rebut
     */
    void applyPreset(char key);

private:
    Q2DViewer *m_2DViewer;
};

}

#endif
