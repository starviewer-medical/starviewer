#include "slicingkeyboardtool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
#include "study.h"
#include "series.h"
#include "patient.h"
#include "changesliceqviewercommand.h"
#include "renderqviewercommand.h"
//qt
#include <QTime>
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

SlicingKeyboardTool::SlicingKeyboardTool( QViewer *viewer, QObject *parent )
 : Tool(viewer,parent)
{
    m_toolName = "SlicingKeyboardTool";
    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if( !m_2DViewer )
        DEBUG_LOG( "No s'ha pogut realitzar el casting a 2DViewer!!!" );
}

SlicingKeyboardTool::~SlicingKeyboardTool()
{
}

void SlicingKeyboardTool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
    case vtkCommand::KeyPressEvent:
    {
        QString keySymbol = m_2DViewer->getInteractor()->GetKeySym();
        if( keySymbol == "Home" )
        {
            m_2DViewer->setSlice(0);
        }
        else if( keySymbol == "End" )
        {
            m_2DViewer->setSlice( m_2DViewer->getMaximumSlice() );
        }
        else if( keySymbol == "Up" )
        {
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() + 1 );
        }
        else if( keySymbol == "Down" )
        {
            m_2DViewer->setSlice( m_2DViewer->getCurrentSlice() - 1 );
        }
        else if( keySymbol == "Left" )
        {
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() - 1 );
        }
        else if( keySymbol == "Right" )
        {
            m_2DViewer->setPhase( m_2DViewer->getCurrentPhase() + 1 );
        }
        // TODO Vigilar que no es pot fer només començar perquè no hi ha cap viewer seleccionat si no s'aplica cap HP
        else if(keySymbol == "plus" || keySymbol == "minus")
        {
            if ((m_2DViewer->getCurrentSlice() == m_2DViewer->getMaximumSlice() && keySymbol == "plus") ||
                (m_2DViewer->getCurrentSlice() == m_2DViewer->getMinimumSlice() && keySymbol == "minus"))
            {
                QList<Volume*> volumesList;
                int currentVolumeIndex = 0;

                Volume *currentVolume = m_2DViewer->getInput();
                if (currentVolume != NULL)
                {
                    Study *currentStudy = currentVolume->getStudy();
                    if (currentStudy != NULL)
                    {
                        Patient *currentPatient = currentStudy->getParentPatient();
                        foreach(Study *study, currentPatient->getStudies())
                        {
                            foreach(Series *series, study->getSeries())
                            {
                                foreach(Volume *volume, series->getVolumesList())
                                {
                                    if (volume->getIdentifier() == currentVolume->getIdentifier())
                                    {
                                        currentVolumeIndex = volumesList.size(); // Encara no hem afegit el nou volume, si no, seria size - 1
                                    }

                                    volumesList << volume;
                                }
                            }
                        }
                        int nextVolumeIndex = 0;
                        QViewerCommand *command;
                        if (keySymbol == "plus")
                        {
                            if (currentVolumeIndex >= volumesList.size() - 1)
                            {
                                nextVolumeIndex = 0;
                            }
                            else
                            {
                                nextVolumeIndex = currentVolumeIndex + 1;
                            }
                            command = new RenderQViewerCommand(m_2DViewer);
                        }
                        else
                        {
                            if (currentVolumeIndex <= 0)
                            {
                                nextVolumeIndex = volumesList.size() - 1;
                            }
                            else
                            {
                                nextVolumeIndex = currentVolumeIndex - 1;
                            }
                            command = new ChangeSliceQViewerCommand(m_2DViewer, ChangeSliceQViewerCommand::MaximumSlice);
                        }
                        Volume *nextVolume = volumesList.at(nextVolumeIndex);
                        m_2DViewer->setInputAsynchronously(nextVolume, command);
                    }
                }
            }
            else
            {
                if (keySymbol == "plus")
                {
                    m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() + 1);
                }
                else if (keySymbol == "minus")
                {
                    m_2DViewer->setSlice(m_2DViewer->getCurrentSlice() -1);
                }
            }
        }
    }
    break;

    default:
    break;
    }
}
}
