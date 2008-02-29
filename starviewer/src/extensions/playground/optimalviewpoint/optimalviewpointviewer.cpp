/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#include "optimalviewpointviewer.h"

#include <QVTKWidget.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkRenderer.h>

#include "vtkInteractorStyleSwitchGgg.h"


namespace udg {


OptimalViewpointViewer::OptimalViewpointViewer( QWidget * parent )
    : QViewer( parent )
{
    vtkInteractorStyle * style = vtkInteractorStyleSwitchGgg::New();
    this->getInteractor()->SetInteractorStyle( style );
    style->Delete();

    m_renderer = vtkRenderer::New();

    m_renderer->SetBackground( 1.0, 1.0, 1.0 );

    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
}


OptimalViewpointViewer::~OptimalViewpointViewer()
{
    m_renderer->Delete();
}


vtkRenderer * OptimalViewpointViewer::getRenderer()
{
    return m_renderer;
}


void OptimalViewpointViewer::setInput( Volume * volume )
{
    m_mainVolume = volume;
}


void OptimalViewpointViewer::render()
{
    m_vtkWidget->GetRenderWindow()->Render();
}


void OptimalViewpointViewer::setEnableTools( bool /*enable*/ )
{
    // de moment res
}


void OptimalViewpointViewer::enableTools()
{
    // de moment res
}


void OptimalViewpointViewer::disableTools()
{
    // de moment res
}


void OptimalViewpointViewer::setTool( QString /*tool*/ )
{
    // de moment res
}


void OptimalViewpointViewer::reset()
{
    // de moment res
}


}
