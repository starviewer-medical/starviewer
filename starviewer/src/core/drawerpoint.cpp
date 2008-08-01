/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerpoint.h"
#include "logging.h"
#include "q2dviewer.h"
// vtk
#include <vtkPolyDataMapper.h>
#include <vtkProperty.h>
#include <vtkActor.h>
#include <vtkSphereSource.h>

namespace udg {

DrawerPoint::DrawerPoint(QObject *parent) : DrawerPrimitive(parent), m_sphereRadius(2.0), m_pointActor(NULL)
{
}


DrawerPoint::~DrawerPoint()
{
}

void DrawerPoint::setPosition( double point[3] )
{
    m_position[0]=point[0];
    m_position[1]=point[1];
    m_position[2]=point[2];

    emit changed();
}

void DrawerPoint::setPosition( QVector<double> point )
{
    m_position[0]=point[0];
    m_position[1]=point[1];
    m_position[2]=point[2];

    emit changed();
}

vtkProp *DrawerPoint::getAsVtkProp()
{
    if( !m_pointActor )
    {
        // creem el pipeline de l'm_vtkActor
        m_pointActor = vtkActor::New();
        m_pointSphere = vtkSphereSource::New();
        m_pointMapper = vtkPolyDataMapper::New();
        m_pointMapper->SetInput( m_pointSphere->GetOutput() );
        m_pointActor->SetMapper( m_pointMapper );
    }

    // li donem els atributs
    m_pointSphere-> SetCenter(m_position);

    //std::cout<<"center point: "<<m_pointSphere-> GetCenter()<<std::endl;
    updateVtkActorProperties();

    return m_pointActor;
}

void DrawerPoint::update( int representation )
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

void DrawerPoint::updateVtkProp()
{
    if( m_pointActor )
    {
        //assignem les propietats del punt
        m_pointSphere-> SetCenter(m_position);
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar el punt, ja que encara no s'ha creat!");
    }
}

void DrawerPoint::updateVtkActorProperties()
{
    vtkProperty *properties = m_pointActor->GetProperty();

    // sistema de coordenades
    //m_pointMapper->SetTransformCoordinate( this->getVtkCoordinateObject() );

    //Assignem gruix de l'esfera
    m_pointSphere->SetRadius( m_sphereRadius );

    //Assignem opacitat al punt
    properties->SetOpacity( m_opacity );

    //mirem la visibilitat de l'm_vtkActor
    m_pointActor->SetVisibility( this->isVisible() );

    //Assignem color
    QColor color = this->getColor();
    properties->SetColor( color.redF(), color.greenF(), color.blueF() );
}

double DrawerPoint::getDistanceToPoint( double *point3D )
{
    return sqrt( (point3D[0]-m_position[0])*(point3D[0]-m_position[0]) + (point3D[1]-m_position[1])*(point3D[1]-m_position[1]) + (point3D[2]-m_position[2])*(point3D[2]-m_position[2]) );
}

bool DrawerPoint::isInsideOfBounds( double p1[3], double p2[3], int view )
{
    double minX, maxX, minY, maxY;
    bool inside;

    //determinem x i y màximes i mínimes segons la vista
    switch( view )
    {
        case Q2DViewer::AxialPlane:
            if ( p1[0] < p2[0] )
            {
                minX = p1[0];
                maxX = p2[0];
            }
            else
            {
                maxX = p1[0];
                minX = p2[0];
            }

            if ( p1[1] < p2[1] )
            {
                minY = p1[1];
                maxY = p2[1];
            }
            else
            {
                maxY = p1[1];
                minY = p2[1];
            }
            inside = ( m_position[0] <= maxX && m_position[0] >= minX && m_position[1] <= maxY && m_position[1] >= minY );
            break;
        case Q2DViewer::SagitalPlane:
            if ( p1[2] < p2[2] )
            {
                minX = p1[2];
                maxX = p2[2];
            }
            else
            {
                maxX = p1[2];
                minX = p2[2];
            }

            if ( p1[1] < p2[1] )
            {
                minY = p1[1];
                maxY = p2[1];
            }
            else
            {
                maxY = p1[1];
                minY = p2[1];
            }
            inside = ( m_position[0] <= maxX && m_position[0] >= minX && m_position[1] <= maxY && m_position[1] >= minY );
            break;
        case Q2DViewer::CoronalPlane:
            if ( p1[0] < p2[0] )
            {
                minX = p1[0];
                maxX = p2[0];
            }
            else
            {
                maxX = p1[0];
                minX = p2[0];
            }

            if ( p1[2] < p2[2] )
            {
                minY = p1[2];
                maxY = p2[2];
            }
            else
            {
                maxY = p1[2];
                minY = p2[2];
            }
            inside = ( m_position[0] <= maxX && m_position[0] >= minX && m_position[1] <= maxY && m_position[1] >= minY );
            break;
    }
    return ( inside );
}


}
