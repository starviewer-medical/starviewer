/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "windowlevelpresetstool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"

// vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

WindowLevelPresetsTool::WindowLevelPresetsTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent), m_myToolData(0)
{
    m_toolName = "WindowLevelPresetsTool";
//     m_hasSharedData = true;
//     m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
//     if( !m_2DViewer )
//         DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );

    setToolData( m_viewer->getWindowLevelData() );
}

WindowLevelPresetsTool::~WindowLevelPresetsTool()
{
}

void WindowLevelPresetsTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::KeyPressEvent:
        applyPreset( m_viewer->getInteractor()->GetKeyCode() );
    break;

    default:
    break;
    }
}

void WindowLevelPresetsTool::applyPreset(char key)
{
    if( m_standardPresets.isEmpty() )
        return;

    double window, level;
    QString preset;
    switch(key)
    {
    // de l'1 al 0 apliquem els window level standard que estaran ordenats alfabèticament
    case '1':
        preset = m_standardPresets.at(0);
    break;

    case '2':
        preset = m_standardPresets.at(1);
    break;

    case '3':
        preset = m_standardPresets.at(2);
    break;

    case '4':
        preset = m_standardPresets.at(3);
    break;

    case '5':
        preset = m_standardPresets.at(4);
    break;

    case '6':
        preset = m_standardPresets.at(5);
    break;

    case '7':
        preset = m_standardPresets.at(6);
    break;

    case '8':
        preset = m_standardPresets.at(7);
    break;

    case '9':
        preset = m_standardPresets.at(8);
    break;

    case '0':
        preset = m_standardPresets.at(9);
    break;

    case '\'':
        preset = m_standardPresets.at(10);
    break;

    default:
    break;
    }

    m_myToolData->activatePreset( preset );
}

void WindowLevelPresetsTool::setToolData( ToolData *toolData )
{
    m_toolData = toolData;
    m_myToolData = qobject_cast<WindowLevelPresetsToolData *>(toolData);
    if( !m_myToolData )
    {
        DEBUG_LOG("El tooldata proporcionat no és un WindwoLevelPresetsToolData que és l'esperat");
    }
    else
    {
        m_standardPresets.clear();
        m_standardPresets = m_myToolData->getDescriptionsFromGroup( WindowLevelPresetsToolData::StandardPresets );
    }
}

}
