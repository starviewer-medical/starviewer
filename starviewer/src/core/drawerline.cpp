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

DrawerLine::DrawerLine(QObject *parent) 
: DrawerPrimitive(parent), m_vtkLineSource(0), m_vtkActor(0), m_vtkMapper(0)
{
}

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
        // Creem el pipeline de l'm_vtkActor
        m_vtkActor = vtkActor2D::New();
        m_vtkLineSource = vtkLineSource::New();
        m_vtkMapper = vtkPolyDataMapper2D::New();

        // Assignem els punts a la línia
        m_vtkLineSource->SetPoint1( m_firstPoint );
        m_vtkLineSource->SetPoint2( m_secondPoint );

        m_vtkActor->SetMapper( m_vtkMapper );
        m_vtkMapper->SetInputConnection( m_vtkLineSource->GetOutputPort() );

        // Li donem els atributs
        updateVtkActorProperties();
    }
    return m_vtkActor;
}

void DrawerLine::update()
{
    switch( m_internalRepresentation )
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
        // Assignem els punts a la línia
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

    // Sistema de coordenades
    m_vtkMapper->SetTransformCoordinate( this->getVtkCoordinateObject() );

    // Estil de la línia
    properties->SetLineStipplePattern( m_linePattern );

    // Assignem gruix de la línia
    properties->SetLineWidth( m_lineWidth );

    // Assignem opacitat de la línia
    properties->SetOpacity( m_opacity );

    // Mirem la visibilitat de l'm_vtkActor
    m_vtkActor->SetVisibility( this->isVisible() );

    // Assignem color
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

double DrawerLine::computeDistance( double * spacing )
{
    double distance;
    if ( spacing == NULL )
    {
        distance = MathTools::getDistance3D( m_firstPoint, m_secondPoint );
    }
    else
    {
        double firstPoint[3];
        double secondPoint[3];

        firstPoint[0] = MathTools::truncate( m_firstPoint[0]/spacing[0] );
        firstPoint[1] = MathTools::truncate( m_firstPoint[1]/spacing[1] );
        firstPoint[2] = MathTools::truncate( m_firstPoint[2]/spacing[2] );

        secondPoint[0] = MathTools::truncate( m_secondPoint[0]/spacing[0] );
        secondPoint[1] = MathTools::truncate( m_secondPoint[1]/spacing[1] );
        secondPoint[2] = MathTools::truncate( m_secondPoint[2]/spacing[2] );

        distance = MathTools::getDistance3D( firstPoint, secondPoint );
    }
    return distance;
}

double DrawerLine::getDistanceToPoint( double *point3D )
{
    return vtkLine::DistanceToLine( point3D , m_firstPoint , m_secondPoint );
}

bool DrawerLine::isInsideOfBounds( double p1[3], double p2[3], int view )
{
    double minX, maxX, minY, maxY;
    bool inside;
    int xIndex = Q2DViewer::getXIndexForView(view);
    int yIndex = Q2DViewer::getYIndexForView(view);

    if ( p1[xIndex] < p2[xIndex] )
    {
        minX = p1[xIndex];
        maxX = p2[xIndex];
    }
    else
    {
        maxX = p1[xIndex];
        minX = p2[xIndex];
    }

    if ( p1[yIndex] < p2[yIndex] )
    {
        minY = p1[yIndex];
        maxY = p2[yIndex];
    }
    else
    {
        maxY = p1[yIndex];
        minY = p2[yIndex];
    }

    inside = ( m_firstPoint[xIndex] <= maxX && m_firstPoint[xIndex] >= minX && m_firstPoint[yIndex] <= maxY && m_firstPoint[yIndex] >= minY && m_secondPoint[xIndex] <= maxX && m_secondPoint[xIndex] >= minX && m_secondPoint[yIndex] <= maxY && m_secondPoint[yIndex] >= minY );

    return ( inside );
}
}
