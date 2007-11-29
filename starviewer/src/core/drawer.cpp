/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawer.h"
#include "q2dviewer.h"
#include "drawerprimitive.h"
#include "logging.h"
//vtk
#include <vtkRenderer.h>

namespace udg {

Drawer::Drawer( Q2DViewer *viewer, QObject *parent )
 : QObject(parent)
{
    m_2DViewer = viewer;
    connect( m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(refresh()) );
}

Drawer::~Drawer()
{
}

void Drawer::draw( DrawerPrimitive *primitive, int plane, int slice )
{
    switch( plane )
    {
    case QViewer::AxialPlane:
        m_axialPrimitives.insert( slice, primitive );
        if( m_2DViewer->getView() == Q2DViewer::Axial )
        {
            if( slice < 0 || m_2DViewer->getCurrentSlice() == slice )
                primitive->setVisibility( true );
            else
                primitive->setVisibility( false );
        }
        else
            primitive->setVisibility( false );
    break;

    case QViewer::SagitalPlane:
        m_sagitalPrimitives.insert( slice, primitive );
        if( m_2DViewer->getView() == Q2DViewer::Sagittal )
        {
            if( slice < 0 || m_2DViewer->getCurrentSlice() == slice )
                primitive->setVisibility( true );
            else
                primitive->setVisibility( false );
        }
        else
            primitive->setVisibility( false );
    break;

    case QViewer::CoronalPlane:
        m_coronalPrimitives.insert( slice, primitive );
        if( m_2DViewer->getView() == Q2DViewer::Coronal )
        {
            if( slice < 0 || m_2DViewer->getCurrentSlice() == slice )
                primitive->setVisibility( true );
            else
                primitive->setVisibility( false );
        }
        else
            primitive->setVisibility( false );
    break;

    case QViewer::Top2DPlane:
        m_top2DPlanePrimitives << primitive;
        primitive->setVisibility( true );
    break;

    default:
        DEBUG_LOG("Pla no definit!");
        return;
    break;
    }
    m_2DViewer->getRenderer()->AddActor( primitive->getAsVtkProp() );
}

void Drawer::refresh()
{
    if( m_currentPlane == m_2DViewer->getView() )
    {
        if( m_currentSlice != m_2DViewer->getCurrentSlice() )
        {
            // cal fer invisible el que es veia en aquest pla i llesca i fer visible el que hi ha a la nova llesca
            hide( m_currentPlane, m_currentSlice );
            m_currentSlice = m_2DViewer->getCurrentSlice();
            show( m_currentPlane, m_currentSlice );
        }
    }
    else
    {
        // cal fer invisible el que es veia en aquest pla i llesca i fer visible el que hi ha al nou pla i llesca
        hide( m_currentPlane, m_currentSlice );
        m_currentSlice = m_2DViewer->getCurrentSlice();
        m_currentPlane = m_2DViewer->getView();
        show( m_currentPlane, m_currentSlice );
    }
}

void Drawer::hide( int plane, int slice )
{
    QList< DrawerPrimitive *> primitivesList;
    switch( plane )
    {
    case QViewer::AxialPlane:
        primitivesList = m_axialPrimitives.values(slice);
    break;

    case QViewer::SagitalPlane:
        primitivesList = m_sagitalPrimitives.values(slice);
    break;

    case QViewer::CoronalPlane:
        primitivesList = m_coronalPrimitives.values(slice);
    break;

    case QViewer::Top2DPlane:
        primitivesList = m_top2DPlanePrimitives;
    break;
    }
    foreach( DrawerPrimitive *primitive, primitivesList )
    {
        primitive->visibilityOff();
    }
}

void Drawer::show( int plane, int slice )
{
    QList< DrawerPrimitive *> primitivesList;
    switch( plane )
    {
    case QViewer::AxialPlane:
        primitivesList = m_axialPrimitives.values(slice);
    break;

    case QViewer::SagitalPlane:
        primitivesList = m_sagitalPrimitives.values(slice);
    break;

    case QViewer::CoronalPlane:
        primitivesList = m_coronalPrimitives.values(slice);
    break;

    case QViewer::Top2DPlane:
        primitivesList = m_top2DPlanePrimitives;
    break;
    }
    foreach( DrawerPrimitive *primitive, primitivesList )
    {
        primitive->visibilityOn();
    }
}

}
