/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawercrosshair.h"

#include "q2dviewer.h"
#include "logging.h"
#include "drawerline.h"
#include "mathtools.h"
// vtk
#include <vtkActor2D.h>
#include <vtkPropAssembly.h>

namespace udg {

DrawerCrossHair::DrawerCrossHair(QObject *parent)
: DrawerPrimitive(parent)
{
    m_vtkPropAssembly = NULL;
    m_lineUp = new DrawerLine();
    m_lineDown = new DrawerLine();
    m_lineLeft = new DrawerLine();
    m_lineRight = new DrawerLine();
    m_lineFront = new DrawerLine();
    m_lineBack = new DrawerLine();
}

DrawerCrossHair::~DrawerCrossHair()
{
    emit dying(this);

    if ( m_vtkPropAssembly )
    {
        m_vtkPropAssembly->Delete();
    }
}

void DrawerCrossHair::setCentrePoint( double x, double y, double z )
{
    m_centrePoint[0] = x;
    m_centrePoint[1] = y;
    m_centrePoint[2] = z;

    //assignem els punts a la línia 1
    m_lineUp->setFirstPoint( m_centrePoint[0], m_centrePoint[1]-6, m_centrePoint[2] );
    m_lineUp->setSecondPoint( m_centrePoint[0], m_centrePoint[1]-1, m_centrePoint[2] );

    //assignem els punts a la línia 2
    m_lineDown->setFirstPoint( m_centrePoint[0], m_centrePoint[1]+6, m_centrePoint[2] );
    m_lineDown->setSecondPoint( m_centrePoint[0], m_centrePoint[1]+1, m_centrePoint[2] );

    //assignem els punts a la línia 3
    m_lineLeft->setFirstPoint( m_centrePoint[0]-6, m_centrePoint[1], m_centrePoint[2] );
    m_lineLeft->setSecondPoint( m_centrePoint[0]-1, m_centrePoint[1], m_centrePoint[2] );

    //assignem els punts a la línia 4
    m_lineRight->setFirstPoint( m_centrePoint[0]+6, m_centrePoint[1], m_centrePoint[2] );
    m_lineRight->setSecondPoint( m_centrePoint[0]+1, m_centrePoint[1], m_centrePoint[2] );

    //assignem els punts a la línia 5
    m_lineBack->setFirstPoint( m_centrePoint[0], m_centrePoint[1], m_centrePoint[2]-6 );
    m_lineBack->setSecondPoint( m_centrePoint[0], m_centrePoint[1], m_centrePoint[2]-1 );

    //assignem els punts a la línia 6
    m_lineFront->setFirstPoint( m_centrePoint[0], m_centrePoint[1], m_centrePoint[2]+6 );
    m_lineFront->setSecondPoint( m_centrePoint[0], m_centrePoint[1], m_centrePoint[2]+1 );

    emit changed();
}

vtkPropAssembly * DrawerCrossHair::getAsVtkPropAssembly()
{
    if( !m_vtkPropAssembly )
    {
        m_vtkPropAssembly = vtkPropAssembly::New();

        // li donem els atributs
        updateVtkActorProperties();

        m_vtkPropAssembly->AddPart( m_lineUp->getAsVtkProp() );
        m_vtkPropAssembly->AddPart( m_lineDown->getAsVtkProp() );
        m_vtkPropAssembly->AddPart( m_lineLeft->getAsVtkProp() );
        m_vtkPropAssembly->AddPart( m_lineRight->getAsVtkProp() );
        m_vtkPropAssembly->AddPart( m_lineBack->getAsVtkProp() );
        m_vtkPropAssembly->AddPart( m_lineFront->getAsVtkProp() );
    }
    return m_vtkPropAssembly;
}

vtkProp * DrawerCrossHair::getAsVtkProp()
{
    return (vtkProp * ) getAsVtkPropAssembly();
}


void DrawerCrossHair::update( int representation )
{
    m_lineUp->update( representation );
    m_lineDown->update( representation );
    m_lineLeft->update( representation );
    m_lineRight->update( representation );
    m_lineBack->update( representation );
    m_lineFront->update( representation );
}

void DrawerCrossHair::updateVtkProp()
{
    if( m_vtkPropAssembly )
    {
        m_lineUp->update( VTKRepresentation );
        m_lineDown->update( VTKRepresentation );
        m_lineLeft->update( VTKRepresentation );
        m_lineRight->update( VTKRepresentation );
        m_lineBack->update( VTKRepresentation );
        m_lineFront->update( VTKRepresentation );

        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar la línia, ja que encara no s'ha creat!");
    }
}

void DrawerCrossHair::updateVtkActorProperties()
{
    m_lineUp->setLineWidth( 2.0 );
    m_lineDown->setLineWidth( 2.0 );
    m_lineLeft->setLineWidth( 2.0 );
    m_lineRight->setLineWidth( 2.0 );
    m_lineBack->setLineWidth( 2.0 );
    m_lineFront->setLineWidth( 2.0 );
}

double DrawerCrossHair::getDistanceToPoint( double *point3D )
{
    return ( MathTools::getDistance3D( m_centrePoint, point3D ) );
}

bool DrawerCrossHair::isInsideOfBounds( double p1[3], double p2[3], int view )
{
    // TODO implementar!
    return false;
}

void DrawerCrossHair::setVisibility( bool visible )
{
    m_isVisible = visible;
    m_lineUp->setVisibility( visible );
    m_lineDown->setVisibility( visible );
    m_lineLeft->setVisibility( visible );
    m_lineRight->setVisibility( visible );
    m_lineBack->setVisibility( visible );
    m_lineFront->setVisibility( visible );
    emit changed();
}

}
