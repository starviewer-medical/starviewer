/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "optimalviewpointviewer.h"

#include <QBoxLayout>
#include <QVTKWidget.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkInteractorStyle.h>

#include "vtkInteractorStyleSwitchGgg.h"

namespace udg {

OptimalViewpointViewer::OptimalViewpointViewer( QWidget * parent, vtkRenderer * renderer )
 : QWidget( parent )
{
    QLayout * layout = new QBoxLayout( QBoxLayout::LeftToRight, this );
    layout->setMargin( 0 );

    m_vtkWidget = new QVTKWidget( this );
    vtkRenderWindow * renderWindow = vtkRenderWindow::New();
    QVTKInteractor * interactor = QVTKInteractor::New();
    renderWindow->SetInteractor( interactor );
    m_vtkWidget->SetRenderWindow( renderWindow );
    interactor->Initialize();
    vtkInteractorStyle * style = vtkInteractorStyleSwitchGgg::New();
    interactor->SetInteractorStyle( style );
    renderWindow->Delete();
    interactor->Delete();
    style->Delete();

    layout->addWidget( m_vtkWidget );

    m_renderer = 0;

    this->setRenderer( renderer );
}

OptimalViewpointViewer::~OptimalViewpointViewer()
{
    m_renderer->Delete();
}

/**
 * Assigna a la classe el renderer que haurà de fer la visualització si
 * encara no en té cap. Si la classe ja té un renderer o el mètode rep un
 * null no fa res.
 */
void OptimalViewpointViewer::setRenderer( vtkRenderer * renderer )
{
    if ( !m_renderer && renderer )
    {
        m_renderer = renderer; m_renderer->Register( 0 );
        m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    }
}

/// Retorna l'interactor de la finestra.
QVTKInteractor * OptimalViewpointViewer::getInteractor() const
{
    return m_vtkWidget->GetInteractor();
}

/**
 * Força l'actualització de la visualització. Si la classe no té un renderer
 * no fa res.
 */
void OptimalViewpointViewer::update()
{
    if ( m_renderer )
    {
        m_vtkWidget->GetRenderWindow()->Render();
    }
}

}; // end namespace udg
