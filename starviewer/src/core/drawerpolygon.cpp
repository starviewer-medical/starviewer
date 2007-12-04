/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "drawerpolygon.h"
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

DrawerPolygon::DrawerPolygon(QObject *parent)
 : DrawerPrimitive(parent), m_vtkPolydata(0), m_vtkPoints(0), m_vtkCellArray(0), m_vtkActor(0), m_vtkMapper(0)
{
}

DrawerPolygon::~DrawerPolygon()
{
    emit dying(this);
}

void DrawerPolygon::addVertix( double point[3] )
{
    QVector<double> array(3);
    for( int i = 0; i<3; i++ )
        array[i] = point[i];

    m_pointsList << array;
    emit changed();
}

void DrawerPolygon::setVertix( int i, double point[3] )
{
    if( i >= m_pointsList.count() || i < 0 )
        addVertix( point );
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

vtkProp *DrawerPolygon::getAsVtkProp()
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

        connect( this, SIGNAL(changed()), SLOT(updateVtkProp()) );
    }
    return m_vtkActor;
}

void DrawerPolygon::update( int representation )
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

void DrawerPolygon::updateVtkProp()
{
    if( m_vtkActor )
    {
        m_vtkPolydata->Reset();
        buildVtkPoints();
        updateVtkActorProperties();
    }
    else
    {
        DEBUG_LOG("No es pot actualitzar el polígon, ja que encara no s'ha creat!");
    }
}

void DrawerPolygon::buildVtkPoints()
{
    // primer comprovem si el polígon és tancat. En cas que l'últim i el primer no coincideixin, l'afegim
    // TODO es podria comprovar si com a mínim té tres punts, sinó, no es pot considerar polígon
    bool extraVertix = false;
    if( !m_pointsList.isEmpty() )
    {
        double *firstPoint = m_pointsList.first().data();
        double *lastPoint = m_pointsList.last().data();
        if ( ( firstPoint[0] != lastPoint[0] ) || ( firstPoint[1] != lastPoint[1] ) || ( firstPoint[2] != lastPoint[2] ) )
        {
            extraVertix = true;
        }
    }
    if( !m_vtkPolydata )
    {
        m_vtkPolydata = vtkPolyData::New();
        m_vtkPoints = vtkPoints::New();
        m_vtkCellArray = vtkCellArray::New();
    }

    //especifiquem el nombre de vèrtexs que té el polígon
    int numberOfVertices = m_pointsList.count() + ( extraVertix ? 1 : 0 );
    m_vtkCellArray->InsertNextCell( numberOfVertices );
    m_vtkPoints->SetNumberOfPoints( numberOfVertices );

    //donem els punts/vertexs
    int i = 0;
    foreach( QVector<double> vertix, m_pointsList )
    {
        m_vtkPoints->InsertPoint( i, vertix.data() );
        m_vtkCellArray->InsertCellPoint( i );
        i++;
    }
//     for ( int i = 0; i < numberOfVertices; i++ )
//     {
//         m_vtkPoints->InsertPoint( i, m_pointsList.at(i).data() );
//         m_vtkCellArray->InsertCellPoint( i );
//     }
    if( extraVertix )
    {
        // tornem a afegir el primer punt
        m_vtkPoints->InsertPoint( numberOfVertices-1, m_pointsList.at(0).data() );
        m_vtkCellArray->InsertCellPoint( numberOfVertices-1 );
    }
    //assignem els punts al polydata
    m_vtkPolydata->SetPoints( m_vtkPoints );
    // comprovem si la forma està "plena" o no
    if ( this->isFilled() )
        m_vtkPolydata->SetPolys( m_vtkCellArray );
    else
        m_vtkPolydata->SetLines( m_vtkCellArray );
}

void DrawerPolygon::updateVtkActorProperties()
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
    if ( !this->isVisible() )
        m_vtkActor->VisibilityOff();
    //Assignem color
    QColor color = this->getColor();
    m_vtkActor->GetProperty()->SetColor( color.redF(), color.greenF(), color.blueF() );
}

}
