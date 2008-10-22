#include "qexperimental3dviewer.h"

#include <QVTKWidget.h>

#include <vtkEncodedGradientEstimator.h>
#include <vtkEncodedGradientShader.h>
#include <vtkRenderWindow.h>
#include <vtkRenderer.h>
#include <vtkVolumeRayCastMapper.h>

#include "experimental3dvolume.h"
#include "../optimalviewpoint/vtkInteractorStyleSwitchGgg.h"


namespace udg {


QExperimental3DViewer::QExperimental3DViewer( QWidget *parent )
 : QViewer( parent ), m_volume( 0 )
{
    vtkInteractorStyleSwitchGgg *style = vtkInteractorStyleSwitchGgg::New();
    getInteractor()->SetInteractorStyle( style );
    style->SetCurrentStyleToTrackballCamera();
    style->Delete();

    m_renderer = vtkRenderer::New();
    m_renderer->SetBackground( 1.0, 1.0, 1.0 );

    m_vtkWidget->GetRenderWindow()->AddRenderer( m_renderer );
    m_vtkWidget->setAutomaticImageCacheEnabled( true );
}


QExperimental3DViewer::~QExperimental3DViewer()
{
    m_renderer->Delete();
    delete m_volume;
}


vtkRenderer* QExperimental3DViewer::getRenderer()
{
    return m_renderer;
}


void QExperimental3DViewer::setInput( Volume *volume )
{
    m_mainVolume = volume;

    m_volume = new Experimental3DVolume( volume );
    m_renderer->AddViewProp( m_volume->getVolume() );
}


Experimental3DVolume* QExperimental3DViewer::getVolume() const
{
    return m_volume;
}


QColor QExperimental3DViewer::getBackgroundColor()
{
    double *background = m_renderer->GetBackground();
    return QColor::fromRgbF( background[0], background[1], background[2] );
}


void QExperimental3DViewer::setBackgroundColor( QColor color )
{
    m_renderer->SetBackground( color.redF(), color.greenF(), color.blueF() );
    render();
}


void QExperimental3DViewer::updateShadingTable()
{
    vtkVolume *volume = m_volume->getVolume();
    vtkVolumeRayCastMapper *mapper = vtkVolumeRayCastMapper::SafeDownCast( volume->GetMapper() );
    mapper->GetGradientShader()->UpdateShadingTable( m_renderer, volume, mapper->GetGradientEstimator() );
}


void QExperimental3DViewer::render()
{
    m_vtkWidget->GetRenderWindow()->Render();
}


void QExperimental3DViewer::reset()
{
    m_renderer->SetBackground( 1.0, 1.0, 1.0 );
}


void QExperimental3DViewer::getCurrentWindowLevel( double wl[2] )
{
    // TODO estem obligats a implementar-lo. De moment retornem 0,0
    wl[0] = wl[1] = 0.0;
}


void QExperimental3DViewer::setWindowLevel( double window, double level )
{
    // TODO estem obligats a implementar-lo.
    Q_UNUSED( window );
    Q_UNUSED( level );
}


void QExperimental3DViewer::resetView( CameraOrientationType view )
{
    // TODO estem obligats a implementar-lo. De moment només assignem variable
    // però caldria aplicar la orientació que se'ns demana
    Q_UNUSED( view );
}


}
