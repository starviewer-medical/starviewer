/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerline.h"
#include "logging.h"
#include "mathtools.h"
#include "q2dviewer.h"
// vtk
#include <vtkLineSource.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
#include <vtkActor2D.h>
#include <vtkLine.h>
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
    this->setFirstPoint( point[0], point[1], point[2] );
}

void DrawerLine::setFirstPoint( double x, double y, double z )
{
    m_firstPoint[0] = x;
    m_firstPoint[1] = y;
    m_firstPoint[2] = z;

    emit changed();
}

void DrawerLine::setSecondPoint( double point[3] )
{
    this->setSecondPoint( point[0], point[1], point[2] );
}

void DrawerLine::setSecondPoint( double x, double y, double z )
{
    m_secondPoint[0] = x;
    m_secondPoint[1] = y;
    m_secondPoint[2] = z;

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

double *DrawerLine::getMiddlePoint()
{
    double * middlePoint = new double[3];

    middlePoint[0] = ( m_firstPoint[0] + m_secondPoint[0] ) / 2;
    middlePoint[1] = ( m_firstPoint[1] + m_secondPoint[1] ) / 2;
    middlePoint[2] = ( m_firstPoint[2] + m_secondPoint[2] ) / 2;

    return middlePoint;
}

double DrawerLine::computeDistance()
{
    return ( MathTools::getDistance3D( m_firstPoint, m_secondPoint ) );
}

double DrawerLine::getDistanceToPoint( double *point3D )
{
    return vtkLine::DistanceToLine( point3D , m_firstPoint , m_secondPoint );
}

bool DrawerLine::isInsideOfBounds( double p1[3], double p2[3], int view )
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
            inside = ( m_firstPoint[0] <= maxX && m_firstPoint[0] >= minX && m_firstPoint[1] <= maxY && m_firstPoint[1] >= minY && m_secondPoint[0] <= maxX && m_secondPoint[0] >= minX && m_secondPoint[1] <= maxY && m_secondPoint[1] >= minY );
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
            inside = ( m_firstPoint[2] <= maxX && m_firstPoint[2] >= minX && m_firstPoint[1] <= maxY && m_firstPoint[1] >= minY && m_secondPoint[2] <= maxX && m_secondPoint[2] >= minX && m_secondPoint[1] <= maxY && m_secondPoint[1] >= minY );
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
            inside = ( m_firstPoint[0] <= maxX && m_firstPoint[0] >= minX && m_firstPoint[2] <= maxY && m_firstPoint[2] >= minY && m_secondPoint[0] <= maxX && m_secondPoint[0] >= minX && m_secondPoint[2] <= maxY && m_secondPoint[2] >= minY );
            break;
    }
    return ( inside );
}
}
