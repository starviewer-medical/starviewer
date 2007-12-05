/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerpolyline.h"
#include "logging.h"
// vtk
#include <vtkPolyData.h>
#include <vtkCellArray.h>
#include <vtkActor2D.h>
#include <vtkPolyDataMapper2D.h>
#include <vtkProperty2D.h>
// qt
#include <QVector>

namespace udg {

DrawerPolyline::DrawerPolyline(QObject *parent)
 : DrawerPrimitive(parent), m_vtkPolydata(0), m_vtkPoints(0), m_vtkCellArray(0), m_vtkActor(0), m_vtkMapper(0)
{
}

DrawerPolyline::~DrawerPolyline()
{
    emit dying(this);

    if ( !m_vtkPolydata )
        m_vtkPolydata->Delete();

    if ( !m_vtkPoints )
        m_vtkPoints->Delete();

    if ( !m_vtkCellArray )
        m_vtkCellArray->Delete();
}

void DrawerPolyline::addPoint( double point[3] )
{
    QVector<double> array(3);
    for( int i = 0; i<3; i++ )
        array[i] = point[i];

    m_pointsList << array;
    emit changed();
}

void DrawerPolyline::setPoint( int i, double point[3] )
{
    if( i >= m_pointsList.count() || i < 0 )
        addPoint( point );
    else
    {
        QVector<double> array(3);
        array = m_pointsList.takeAt(i);
        for( int j=0; j < 3; j++ )
            array[j] = point[j];

        m_pointsList.insert(i,array);
        emit changed();
    }
}

void DrawerPolyline::removePoint( int i )
{
    m_pointsList.removeAt( i );
    emit changed();
}

vtkProp *DrawerPolyline::getAsVtkProp()
{
    if( !m_vtkActor )
    {
        buildVtkPoints();
        // creem el pipeline de l'm_vtkActor
        m_vtkActor = vtkActor2D::New();
        m_vtkMapper = vtkPolyDataMapper2D::New();

        m_vtkActor->SetMapper( m_vtkMapper );
        m_vtkMapper->SetInput( m_vtkPolydata );
        // li donem els atributs
        updateVtkActorProperties();
    }
    return m_vtkActor;
}

void DrawerPolyline::update( int representation )
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

void DrawerPolyline::updateVtkProp()
{
    if( m_vtkActor )
    {
        m_vtkPolydata->Reset();
        buildVtkPoints();
        updateVtkActorProperties();
        this->setModified(false);
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar l polilínia, ja que encara no s'ha creat!");
    }
}

void DrawerPolyline::buildVtkPoints()
{
    if( !m_vtkPolydata )
    {
        m_vtkPolydata = vtkPolyData::New();
        m_vtkPoints = vtkPoints::New();
        m_vtkCellArray = vtkCellArray::New();
    }

    //especifiquem el nombre de vèrtexs que té la polilinia
    int numberOfVertices = m_pointsList.count();
    m_vtkCellArray->InsertNextCell( numberOfVertices );
    m_vtkPoints->SetNumberOfPoints( numberOfVertices );

    //donem els punts
    int i = 0;
    foreach( QVector<double> vertix, m_pointsList )
    {
        m_vtkPoints->InsertPoint( i, vertix.data() );
        m_vtkCellArray->InsertCellPoint( i );
        i++;
    }

    //assignem els punts al polydata
    m_vtkPolydata->SetPoints( m_vtkPoints );

    m_vtkPolydata->SetLines( m_vtkCellArray );
}

void DrawerPolyline::updateVtkActorProperties()
{
    // sistema de coordenades
    m_vtkMapper->SetTransformCoordinate( this->getVtkCoordinateObject() );
    // estil de la línia
    m_vtkActor->GetProperty()->SetLineStipplePattern( m_linePattern );
    //Assignem gruix de la línia
    m_vtkActor->GetProperty()->SetLineWidth( m_lineWidth );
    //Assignem opacitat de la línia
    m_vtkActor->GetProperty()->SetOpacity( m_opacity );
    //mirem la visibilitat de l'm_vtkActor
    m_vtkActor->SetVisibility( this->isVisible() );
    //Assignem color
    QColor color = this->getColor();
    m_vtkActor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
}

}
