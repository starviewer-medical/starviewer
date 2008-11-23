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
    m_vtkWidget->setAutomaticImageCacheEnabled( true );
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


void OptimalViewpointViewer::setBackgroundColor( QColor color )
{
    m_renderer->SetBackground( color.redF(), color.greenF(), color.blueF() );
}


QColor OptimalViewpointViewer::getBackgroundColor()
{
    double *background = m_renderer->GetBackground();
    return QColor::fromRgbF( background[0], background[1], background[2] );
}


void OptimalViewpointViewer::getCurrentWindowLevel( double wl[2] )
{
    // TODO estem obligats a implementar-lo. De moment retornem 0,0
    wl[0] = wl[1] = 0.0;
}

void OptimalViewpointViewer::resetView( CameraOrientationType view )
{
    // TODO estem obligats a implementar-lo. De moment només assignem variable
    // però caldria aplicar la orientació que se'ns demana
    view = view;
}

void OptimalViewpointViewer::render()
{
    m_vtkWidget->GetRenderWindow()->Render();
}

void OptimalViewpointViewer::reset()
{
    m_renderer->SetBackground( 1.0, 1.0, 1.0 );
}

void OptimalViewpointViewer::setWindowLevel( double, double )
{
    // TODO estem obligats a implementar-lo.
}

}
