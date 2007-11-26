/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "windowlevelpresetstool.h"
#include "q2dviewer.h"
#include "logging.h"
// vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

WindowLevelPresetsTool::WindowLevelPresetsTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent)
{
    m_toolName = "WindowLevelPresetsTool";
    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );
}

WindowLevelPresetsTool::~WindowLevelPresetsTool()
{
}

void WindowLevelPresetsTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::KeyPressEvent:
        applyPreset( m_2DViewer->getInteractor()->GetKeyCode() );
    break;

    default:
    break;
    }
}

void WindowLevelPresetsTool::applyPreset(char key)
{
    switch(key)
    {
    case '1':
        m_2DViewer->setWindowLevel(2000,500);//CT Bone
    break;

    case '2':
        m_2DViewer->setWindowLevel(1500,-650);// CT Lung
    break;

    case '3':
        m_2DViewer->setWindowLevel(400,40); //CT Soft Tissues, Non Contrast
    break;

    case '4':
        m_2DViewer->setWindowLevel(400,70); //CT Soft Tissues, Contrast Medium
    break;

    case '5':
        m_2DViewer->setWindowLevel(300,60); //CT Liver, Contrast Medium // 60-100
    break;

    case '6':
        m_2DViewer->setWindowLevel(200,40); //CT Liver, Non Contrast
    break;

    case '7':
        m_2DViewer->setWindowLevel(300,50); //CT Neck, Contrast Medium
    break;

    case '8':
        m_2DViewer->setWindowLevel(500,100); //Angiography // 100-200
    break;

    case '9':
        m_2DViewer->setWindowLevel(1000,300); //Osteoporosis// 100-1500:window!
    break;

    case '0':
        m_2DViewer->setWindowLevel(800,-800); //Emphysema
    break;

    case '\'':
        m_2DViewer->setWindowLevel(4000,700); //Petrous Bone
    break;

    default:
    break;
    }

}

}
