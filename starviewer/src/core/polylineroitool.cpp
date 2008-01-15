/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineroitool.h"
#include "polylineroitooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "series.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawertext.h"
//vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkProp.h>
#include <vtkLine.h>
#include <vtkPoints.h>
//Qt

namespace udg {

PolylineROITool::PolylineROITool( QViewer *viewer, QObject *parent )
 : Tool(viewer, parent)
{
    m_toolName = "PolylineROITool";
    m_hasSharedData = false;

//     m_toolData = new PolylineROIToolData;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );

    m_latestTime = 0;
    m_closingPolyline = NULL;
    m_mainPolyline=NULL;
}

PolylineROITool::~PolylineROITool()
{
}

void PolylineROITool::handleEvent( long unsigned eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
            this->annotateNewPoint();
            m_2DViewer->getDrawer()->refresh();

//             int totalTimeElapsed = m_time.elapsed();
//             int timeElapsed = ( totalTimeElapsed - m_latestTime );
//
//             DEBUG_LOG( tr("CLIKS: %1").arg( m_2DViewer->getInteractor()->GetRepeatCount() ) );
//
//             if( timeElapsed < 350 )
//             {
//                 DEBUG_LOG( "DOBLE CLICK");
//             }
//             else
//             {
//                 DEBUG_LOG( "SIMPLE CLICK");
//             }
//             m_latestTime = totalTimeElapsed;
        break;

        case vtkCommand::MouseMoveEvent:
            if( m_mainPolyline && ( m_mainPolyline->getNumberOfPoints() >= 1 ) )
            {
                this->simulateClosingPolyline();
                m_2DViewer->getDrawer()->refresh();
            }
        break;
        case vtkCommand::KeyPressEvent:
            if( m_mainPolyline && ( m_mainPolyline->getNumberOfPoints() >= 3 ) )
            {
                this->answerToKeyEvent();
            }
        break;
    }
}

void PolylineROITool::annotateNewPoint()
{
    if (!m_mainPolyline )
    {
        m_mainPolyline = new DrawerPolyline;
        m_2DViewer->getDrawer()->draw( m_mainPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }

    int x,y;
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    x = m_2DViewer->getEventPositionX();
    y = m_2DViewer->getEventPositionY();
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer(), x, y, 0, position );
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim el punt
    m_mainPolyline->addPoint( computed );

//actualitzem els atributs de la polilinia
    m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );
}

void PolylineROITool::simulateClosingPolyline()
{
    if (!m_closingPolyline )
    {
        m_closingPolyline = new DrawerPolyline;
        m_closingPolyline->setLinePattern( DrawerPrimitive::DiscontinuousLinePattern );
        m_2DViewer->getDrawer()->draw( m_closingPolyline , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }

    m_closingPolyline->deleteAllPoints();

    int x,y;
    double position[4];
    double computed[3];

    //capturem l'event de clic esquerre
    x = m_2DViewer->getEventPositionX();
    y = m_2DViewer->getEventPositionY();
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer(), x, y, 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    computed[0] = position[0];
    computed[1] = position[1];
    computed[2] = position[2];

    //afegim els punts que simulen aquesta polilinia
    m_closingPolyline->addPoint( m_mainPolyline->getPoint( 0 ) );
    m_closingPolyline->addPoint( computed );
    m_closingPolyline->addPoint( m_mainPolyline->getPoint( m_mainPolyline->getNumberOfPoints() - 1 ) );

//actualitzem els atributs de la polilinia
    m_closingPolyline->update( DrawerPrimitive::VTKRepresentation );
}

void PolylineROITool::answerToKeyEvent()
{
    //responem a la intenció d'esborrar una distància, sempre que hi hagi una distància seleccionada i
    //s'hagi polsat la tecla adequada (tecla sup) o seleccionar una distància amb el Ctrl i un botó del mouse

    char keyChar = m_2DViewer->getInteractor()->GetKeyCode();
    int keyInt = (int)keyChar;


   if ( keyInt == 32 ) // s'ha polsat l'espai per tancar la forma
    {
        m_mainPolyline->addPoint( m_mainPolyline->getPoint( 0 ) );
        m_mainPolyline->update( DrawerPrimitive::VTKRepresentation );

        double *bounds = m_mainPolyline->getPolylineBounds();
        if( !bounds )
        {
            DEBUG_LOG( "Bounds no definits" );
        }
        else
        {
            double *intersection = new double[3];

            intersection[0] = (bounds[1]+bounds[0])/2.0;
            intersection[1] = (bounds[3]+bounds[2])/2.0;
            intersection[2] = (bounds[5]+bounds[4])/2.0;

            DrawerText * text = new DrawerText;
            text->setText( tr("Area: %1 mm2\nMean: %2").arg( m_mainPolyline->computeArea( m_2DViewer->getView() ) ).arg( this->computeGrayMean() ) );
            text->setAttatchmentPoint( intersection );
            text->update( DrawerPrimitive::VTKRepresentation );
            m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
        }
        delete m_closingPolyline;
        m_closingPolyline=NULL;
        m_2DViewer->getDrawer()->refresh();
        m_mainPolyline=NULL;
    }
}
double PolylineROITool::computeGrayMean()
{
    double mean = 0.0;
    switch( m_2DViewer->getView())
    {
        case Q2DViewer::Axial:
            mean = computeGrayMeanAxial();
            break;

        case Q2DViewer::Sagital:
            mean = computeGrayMeanSagittal();
            break;

        case Q2DViewer::Coronal:
            mean = computeGrayMeanCoronal();
            break;
    }
    return mean;
}
double PolylineROITool::computeGrayMeanAxial()
{
    double mean = 0.0;
    double spacing0;
    double spacing1;
    double spacing2;
    int index;
    int subId;
    int initialPosition;
    int endPosition;
    double intersectPoint[3];
    double  *firstIntersection;
    double *secondIntersection;
    double pcoords[3];
    double t;
    double p0[3];
    double p1[3];
    int numberOfVoxels = 0;
    QList<double*> intersectionList;
    QList<int> indexList;
    vtkPoints *auxPoints;
    double rayP1[3];
    double rayP2[3];
    double verticalLimit;

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_mainPolyline->getNumberOfPoints()-1;

    //taula de punters a vtkLine per a representar cadascun dels segments del polígon
    vtkLine* segments[ numberOfSegments ];

    //creem els diferents segments
    for ( index = 0; index < numberOfSegments; index++ )
    {
        segments[index] = vtkLine::New();
        segments[index]->GetPointIds()->SetNumberOfIds(2);
        segments[index]->GetPoints()->SetNumberOfPoints(2);

        double *p1 = m_mainPolyline->getPoint( index );
        double *p2 = m_mainPolyline->getPoint( index+1 );

        segments[index]->GetPoints()->InsertPoint( 0, p1 );
        segments[index]->GetPoints()->InsertPoint( 1, p2 );
    }

    double *bounds = m_mainPolyline->getPolylineBounds();

    rayP1[0] = bounds[0];//xmin
    rayP1[1] = bounds[2];//y
    rayP1[2] = bounds[4];//z
    rayP2[0] = bounds[1];//xmax
    rayP2[1] = bounds[2];//y
    rayP2[2] = bounds[4];//z

    spacing0 = m_2DViewer->getInput()->getSpacing()[0];
    spacing1 = m_2DViewer->getInput()->getSpacing()[1];
    spacing2 = m_2DViewer->getInput()->getSpacing()[2];

    verticalLimit = bounds[3];

    while( rayP1[1] <= verticalLimit )
    {
        intersectionList.clear();
        indexList.clear();

        for ( index = 0; index < numberOfSegments; index++ )
        {
            auxPoints = segments[index]->GetPoints();
            auxPoints->GetPoint(0,p0);
            auxPoints->GetPoint(1,p1);

            if ((rayP1[1] <= p0[1] && rayP1[1] >= p1[1]) || (rayP1[1] >= p0[1] && rayP1[1] <= p1[1]))
                indexList << index;
        }
        //obtenim les interseccions entre tots els segments de la ROI i el raig actual
        foreach (int segment, indexList)
        {
            if ( segments[segment]->IntersectWithLine(rayP1, rayP2, 0.0001, t, intersectPoint, pcoords, subId) > 0)
            {
                double *findedPoint = new double[3];
                findedPoint[0] = intersectPoint[0];
                findedPoint[1] = intersectPoint[1];
                findedPoint[2] = intersectPoint[2];
                intersectionList.append ( findedPoint );
            }
        }

        if ( (intersectionList.count() % 2)==0 )
        {
            int limit = intersectionList.count()/2;
            for ( index = 0; index < limit; index++ )
            {
                initialPosition = index * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at( initialPosition );
                secondIntersection = intersectionList.at( endPosition );

                //Tractem els dos sentits de les interseccions
                if (firstIntersection[0] <= secondIntersection[0])//d'esquerra cap a dreta
                {
                    while ( firstIntersection[0] <= secondIntersection[0] )
                    {
                        mean += (double)getGrayValue( firstIntersection, spacing0, spacing1, spacing2 );
                        numberOfVoxels++;
                        firstIntersection[0] += spacing0;
                    }
                }
                else //de dreta cap a esquerra
                {
                    while ( firstIntersection[0] >= secondIntersection[0] )
                    {
                        mean += (double)getGrayValue( firstIntersection, spacing0, spacing1, spacing2 );
                        numberOfVoxels++;
                        firstIntersection[0] -= spacing0;
                    }
                }
            }
        }
        else
            DEBUG_LOG( "EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!" );

        //fem el següent pas en la coordenada que escombrem
        rayP1[1] += spacing1;
        rayP2[1] += spacing1;
    }

    mean /= numberOfVoxels;

    //destruïm els diferents segments que hem creat per simular la roi
    for ( index = 0; index < numberOfSegments; index++ )
        segments[index]->Delete();

    return mean;
}

double PolylineROITool::computeGrayMeanSagittal()
{
    double mean = 0.0;
    double spacing0;
    double spacing1;
    double spacing2;
    int index;
    int subId;
    int initialPosition;
    int endPosition;
    double intersectPoint[3];
    double  *firstIntersection;
    double *secondIntersection;
    double pcoords[3];
    double t;
    double p0[3];
    double p1[3];
    int numberOfVoxels = 0;
    QList<double*> intersectionList;
    QList<int> indexList;
    vtkPoints *auxPoints;
    double rayP1[3];
    double rayP2[3];
    double verticalLimit;

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_mainPolyline->getNumberOfPoints()-1;

    //taula de punters a vtkLine per a representar cadascun dels segments del polígon
    vtkLine* segments[ numberOfSegments ];

    //creem els diferents segments
    for ( index = 0; index < numberOfSegments; index++ )
    {
        segments[index] = vtkLine::New();
        segments[index]->GetPointIds()->SetNumberOfIds(2);
        segments[index]->GetPoints()->SetNumberOfPoints(2);

        double *p1 = m_mainPolyline->getPoint( index );
        double *p2 = m_mainPolyline->getPoint( index+1 );

        segments[index]->GetPoints()->InsertPoint( 0, p1 );
        segments[index]->GetPoints()->InsertPoint( 1, p2 );
    }

    double *bounds = m_mainPolyline->getPolylineBounds();

    rayP1[0] = bounds[0];//xmin
    rayP1[1] = bounds[2];//ymin
    rayP1[2] = bounds[4];//zmin
    rayP2[0] = bounds[0];//xmin
    rayP2[1] = bounds[2];//ymin
    rayP2[2] = bounds[5];//zmax

    spacing0 = m_2DViewer->getInput()->getSpacing()[0];
    spacing1 = m_2DViewer->getInput()->getSpacing()[1];
    spacing2 = m_2DViewer->getInput()->getSpacing()[2];

    verticalLimit = bounds[3];

    while( rayP1[1] <= verticalLimit )
    {
        intersectionList.clear();
        indexList.clear();

        for ( index = 0; index < numberOfSegments; index++ )
        {
            auxPoints = segments[index]->GetPoints();
            auxPoints->GetPoint(0,p0);
            auxPoints->GetPoint(1,p1);

            if ((rayP1[1] <= p0[1] && rayP1[1] >= p1[1]) || (rayP1[1] >= p0[1] && rayP1[1] <= p1[1]))
                indexList << index;
        }

        //obtenim les interseccions entre tots els segments de la ROI i el raig actual
        foreach (int segment, indexList)
        {
            if ( segments[segment]->IntersectWithLine(rayP1, rayP2, 0.0001, t, intersectPoint, pcoords, subId) > 0)
            {
                double *findedPoint = new double[3];
                findedPoint[0] = intersectPoint[0];
                findedPoint[1] = intersectPoint[1];
                findedPoint[2] = intersectPoint[2];
                intersectionList.append ( findedPoint );
            }
        }

        if ( (intersectionList.count() % 2)==0 )
        {
            int limit = intersectionList.count()/2;
            for ( index = 0; index < limit; index++ )
            {
                initialPosition = index * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at( initialPosition );
                secondIntersection = intersectionList.at( endPosition );

                //Tractem els dos sentits de les interseccions
                if (firstIntersection[2] <= secondIntersection[2])//d'esquerra cap a dreta
                {
                    while ( firstIntersection[2] <= secondIntersection[2] )
                    {
                        mean += (double)getGrayValue( firstIntersection, spacing0, spacing1, spacing2 );
                        numberOfVoxels++;
                        firstIntersection[2] += spacing0;
                    }
                }
                else //de dreta cap a esquerra
                {
                    while ( firstIntersection[2] >= secondIntersection[2] )
                    {
                        mean += (double)getGrayValue( firstIntersection, spacing0, spacing1, spacing2 );
                        numberOfVoxels++;
                        firstIntersection[2] -= spacing0;
                    }
                }
            }
        }
        else
            DEBUG_LOG( "EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!" );

        //fem el següent pas en la coordenada que escombrem
        rayP1[1] += spacing1;
        rayP2[1] += spacing1;
    }

    mean /= numberOfVoxels;

    //destruïm els diferents segments que hem creat per simular la roi
    for ( index = 0; index < numberOfSegments; index++ )
        segments[index]->Delete();

    return mean;
}

int PolylineROITool::getGrayValue( double *coords, double spacing0, double spacing1, double spacing2 )
{
    double *origin = m_2DViewer->getInput()->getOrigin();
    int index[3];

    switch( m_2DViewer->getView() )
    {
        case Q2DViewer::Axial:
            index[0] = (int)((coords[0] - origin[0])/spacing0);
            index[1] = (int)((coords[1] - origin[1])/spacing1);
            index[2] = m_2DViewer->getCurrentSlice();
            break;

        case Q2DViewer::Sagital:
            index[0] = m_2DViewer->getCurrentSlice();
            index[1] = (int)((coords[1] - origin[1])/spacing1);
            index[2] = (int)((coords[2] - origin[2])/spacing2);
            break;

        case Q2DViewer::Coronal:
            index[0] = (int)((coords[0] - origin[0])/spacing0);
            index[1] = m_2DViewer->getCurrentSlice();
            index[2] = (int)((coords[2] - origin[2])/spacing2);
            break;
    }
    return *((int*)m_2DViewer->getInput()->getVtkData()->GetScalarPointer(index));
}

double PolylineROITool::computeGrayMeanCoronal()
{
    double mean = 0.0;
    double spacing0;
    double spacing1;
    double spacing2;
    int index;
    int subId;
    int initialPosition;
    int endPosition;
    double intersectPoint[3];
    double  *firstIntersection;
    double *secondIntersection;
    double pcoords[3];
    double t;
    double p0[3];
    double p1[3];
    int numberOfVoxels = 0;
    QList<double*> intersectionList;
    QList<int> indexList;
    vtkPoints *auxPoints;
    double rayP1[3];
    double rayP2[3];
    double verticalLimit;

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_mainPolyline->getNumberOfPoints()-1;

    //taula de punters a vtkLine per a representar cadascun dels segments del polígon
    vtkLine* segments[ numberOfSegments ];

    //creem els diferents segments
    for ( index = 0; index < numberOfSegments; index++ )
    {
        segments[index] = vtkLine::New();
        segments[index]->GetPointIds()->SetNumberOfIds(2);
        segments[index]->GetPoints()->SetNumberOfPoints(2);

        double *p1 = m_mainPolyline->getPoint( index );
        double *p2 = m_mainPolyline->getPoint( index+1 );

        segments[index]->GetPoints()->InsertPoint( 0, p1 );
        segments[index]->GetPoints()->InsertPoint( 1, p2 );
    }

    double *bounds = m_mainPolyline->getPolylineBounds();

    rayP1[0] = bounds[0];//xmin
    rayP1[1] = bounds[2];//ymin
    rayP1[2] = bounds[4];//zmin
    rayP2[0] = bounds[1];//xmax
    rayP2[1] = bounds[2];//ymin
    rayP2[2] = bounds[4];//zmin

    spacing0 = m_2DViewer->getInput()->getSpacing()[0];
    spacing1 = m_2DViewer->getInput()->getSpacing()[1];
    spacing2 = m_2DViewer->getInput()->getSpacing()[2];

    verticalLimit = bounds[5];

    while( rayP1[2] <= verticalLimit )
    {
        intersectionList.clear();
        indexList.clear();

        for ( index = 0; index < numberOfSegments; index++ )
        {
            auxPoints = segments[index]->GetPoints();
            auxPoints->GetPoint(0,p0);
            auxPoints->GetPoint(1,p1);

            if ((rayP1[2] <= p0[2] && rayP1[2] >= p1[2]) || (rayP1[2] >= p0[2] && rayP1[2] <= p1[2]))
                indexList << index;
        }

        //obtenim les interseccions entre tots els segments de la ROI i el raig actual
        foreach (int segment, indexList)
        {
            if ( segments[segment]->IntersectWithLine(rayP1, rayP2, 0.0001, t, intersectPoint, pcoords, subId) > 0)
            {
                double *findedPoint = new double[3];
                findedPoint[0] = intersectPoint[0];
                findedPoint[1] = intersectPoint[1];
                findedPoint[2] = intersectPoint[2];
                intersectionList.append ( findedPoint );
            }
        }

        if ( (intersectionList.count() % 2)==0 )
        {
            int limit = intersectionList.count()/2;
            for ( index = 0; index < limit; index++ )
            {
                initialPosition = index * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at( initialPosition );
                secondIntersection = intersectionList.at( endPosition );

                //Tractem els dos sentits de les interseccions
                if (firstIntersection[0] <= secondIntersection[0])//d'esquerra cap a dreta
                {
                    while ( firstIntersection[0] <= secondIntersection[0] )
                    {
                        mean += (double)getGrayValue( firstIntersection, spacing0, spacing1, spacing2 );
                        numberOfVoxels++;
                        firstIntersection[0] += spacing0;
                    }
                }
                else //de dreta cap a esquerra
                {
                    while ( firstIntersection[0] >= secondIntersection[0] )
                    {
                        mean += (double)getGrayValue( firstIntersection, spacing0, spacing1, spacing2 );
                        numberOfVoxels++;
                        firstIntersection[0] -= spacing0;
                    }
                }
            }
        }
        else
            DEBUG_LOG( "EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!" );

        //fem el següent pas en la coordenada que escombrem
        rayP1[2] += spacing1;
        rayP2[2] += spacing1;
    }

    mean /= numberOfVoxels;

    //destruïm els diferents segments que hem creat per simular la roi
    for ( index = 0; index < numberOfSegments; index++ )
        segments[index]->Delete();

    return mean;
}
}
