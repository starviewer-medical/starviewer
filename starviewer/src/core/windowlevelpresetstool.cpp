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
 : Tool(viewer,parent), m_myToolData(0), m_defaultPresetsIndex(0)
{
    m_toolName = "WindowLevelPresetsTool";
//     m_hasSharedData = true;
//     m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
//     if( !m_2DViewer )
//         DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );

    setToolData( m_viewer->getWindowLevelData() );
    m_characterIndexMap.insert('1',0);
    m_characterIndexMap.insert('2',1);
    m_characterIndexMap.insert('3',2);
    m_characterIndexMap.insert('4',3);
    m_characterIndexMap.insert('5',4);
    m_characterIndexMap.insert('6',5);
    m_characterIndexMap.insert('7',6);
    m_characterIndexMap.insert('8',7);
    m_characterIndexMap.insert('9',8);
    m_characterIndexMap.insert('0',9);
    // TODO això ara "va bé" en un teclat estàndard espanyol, però si el layout és per exemple anglès
    // té molts números per no funcionar ( és a dir, les tecles estaran repartides per altres parts del teclat )
    m_characterIndexMap.insert(33,10); // Shift + 1
    m_characterIndexMap.insert(34,11); // Shift + 2
    m_characterIndexMap.insert(-73,12); // Shift + 3
    m_characterIndexMap.insert(36,13); // Shift + 4
    m_characterIndexMap.insert(37,14); // Shift + 5
    m_characterIndexMap.insert(38,15); // Shift + 6
    m_characterIndexMap.insert(47,16); // Shift + 7
    m_characterIndexMap.insert(40,17); // Shift + 8
    m_characterIndexMap.insert(41,18); // Shift + 9
    m_characterIndexMap.insert(61,19); // Shift + 0

    // Cada cop que es canvïi el volum cal actualitzar la llista de ww/wl per defecte d'aquell volum (definits al DICOM)
    connect( viewer, SIGNAL( volumeChanged(Volume *) ), SLOT( updateWindowLevelData() ) );
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

    QString preset;

    if( !m_characterIndexMap.contains(key) )
    {
        // TODO de moment fem servir la tecla "º", que en un teclat espanyol queda a l'esquerra del nº 1, però en altres teclats pot estar a qualsevol lloc
        if ( key == -70 )
        {
            if( !m_defaultPresets.isEmpty() )
            {
                preset = m_defaultPresets.at(m_defaultPresetsIndex);
                m_defaultPresetsIndex = (m_defaultPresetsIndex+1) % m_defaultPresets.count();

                DEBUG_LOG( QString("default Preset index: %1").arg( m_defaultPresetsIndex ) );
            }
        }
    }
    else
    {
        int presetIndex = m_characterIndexMap.value(key);
        if( presetIndex < m_standardPresets.count() ) 
        {
            preset = m_standardPresets.at(presetIndex);
            m_defaultPresetsIndex = 0;
        }
    }

    m_myToolData->activatePreset( preset );
}

void WindowLevelPresetsTool::updateWindowLevelData()
{
    m_defaultPresetsIndex = 0;
    if( !m_myToolData )
    {
        DEBUG_LOG("No tenim tooldata de window level!");
    }
    else
    {
        m_defaultPresets.clear();
        m_defaultPresets = m_myToolData->getDescriptionsFromGroup( WindowLevelPresetsToolData::FileDefined );
    }
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
