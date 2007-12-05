/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerline.h"
#include "logging.h"
// vtk
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkActor2D.h>
namespace udg {

DrawerLine::DrawerLine(QObject *parent) : DrawerPrimitive(parent), m_vtkLineSource(0), m_vtkActor(0), m_vtkMapper(0)
{}

DrawerLine::~DrawerLine()
{
    emit dying(this);

    if ( m_vtkActor )
        m_vtkActor->Delete();

    if ( m_vtkLineSource )
        m_vtkLineSource->Delete();

    if ( m_vtkMapper )
        m_vtkMapper->Delete();
}

void DrawerLine::setFirstPoint( double point[3] )
{
    for( int i = 0; i<3; i++ )
        m_firstPoint[i] = point[i];

    emit changed();
}

void DrawerLine::setSecondPoint( double point[3] )
{
    for( int i = 0; i<3; i++ )
        m_secondPoint[i] = point[i];

    emit changed();
}

vtkProp *DrawerLine::getAsVtkProp()
{
    if( !m_vtkActor )
    {
        // creem el pipeline de l'm_vtkActor
        m_vtkActor = vtkActor2D::New();
        m_vtkLineSource = vtkLineSource::New();
        m_vtkMapper = vtkPolyDataMapper2D::New();

        //assignem els punts a la línia
        m_vtkLineSource->SetPoint1( m_firstPoint );
        m_vtkLineSource->SetPoint2( m_secondPoint );

        m_vtkActor->SetMapper( m_vtkMapper );
        m_vtkMapper->SetInputConnection( m_vtkLineSource->GetOutputPort() );

        // li donem els atributs
        updateVtkActorProperties();
    }
    return m_vtkActor;
}

void DrawerLine::update( int representation )
{
    switch( representation )
    {
    case VTKRepresentation:
        updateVtkProp();
    break;

    case OpenGLRepresentation:
    break;
    }
}

void DrawerLine::updateVtkProp()
{
    if( m_vtkActor )
    {
        //assignem els punts a la línia
        m_vtkLineSource->SetPoint1( m_firstPoint );
        m_vtkLineSource->SetPoint2( m_secondPoint );
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar la línia, ja que encara no s'ha creat!");
    }
}

void DrawerLine::updateVtkActorProperties()
{
    vtkProperty2D *properties = m_vtkActor->GetProperty();

    // sistema de coordenades
    m_vtkMapper->SetTransformCoordinate( this->getVtkCoordinateObject() );

    // estil de la línia
    properties->SetLineStipplePattern( m_linePattern );

    //Assignem gruix de la línia
    properties->SetLineWidth( m_lineWidth );

    //Assignem opacitat de la línia
    properties->SetOpacity( m_opacity );

    //mirem la visibilitat de l'm_vtkActor
    m_vtkActor->SetVisibility( this->isVisible() );

    //Assignem color
    QColor color = this->getColor();
    properties->SetColor( color.redF(), color.greenF(), color.blueF() );
}

}
