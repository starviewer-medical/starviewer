/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "roitool.h"
#include "q2dviewer.h"
#include "ellipserepresentation.h"
#include "roitooldata.h"
#include "olddrawer.h"
#include "drawingprimitive.h"
#include "ellipse.h"
#include "polygon.h"
#include "volume.h"
#include <vtkCommand.h>
#include <vtkRenderer.h>
#include <vtkRenderWindow.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkMath.h>
#include "logging.h"

namespace udg {

ROITool::ROITool( Q2DViewer *viewer , QObject *, const char * )

{
    m_state = STOPPED;
    m_2DViewer = viewer;
    m_ROIType = ELLIPSE;
}

ROITool::~ROITool()
{
    if ( m_ellipseRepresentation )
        delete m_ellipseRepresentation;
}

void ROITool::handleEvent( unsigned long eventID )
{
    switch( eventID )
    {
        case vtkCommand::LeftButtonPressEvent:
           if ( !m_2DViewer->getOldDrawer()->hasSelectedSet() ) //cas en que no hi ha cap conjunt de primitives seleccionat.
            {
                switch ( m_state )
                {
                    case STOPPED:
                        this->startROIAnnotation();
                    break;

                    case SIMULATING_ANNOTATION:
//                         if ( m_ROIType != POLYLINE )
                            this->stopROIAnnotation();
//                         else
//                             this->annotateNextPolylinePoint();
                    break;
                }
            }
        break;

        case vtkCommand::MouseMoveEvent:
            if( m_state == ANNOTATION_STARTED || m_state == SIMULATING_ANNOTATION )
            {
                this->doROISimulation();
            }
        break;
//
//         case vtkCommand::MiddleButtonPressEvent:
//             switch ( m_state )
//             {
//                 case STOPPED:
//                     m_ROIType = RECTANGLE;
//                 break;
//
//                 case SIMULATING_ANNOTATION:
//                     if ( m_ROIType == POLYLINE )
//                         this->stopROIAnnotation();
//                 break;
//             }
//         break;
//
//         //click amb el botó dret: volem que si l'element clickat és una ROI, quedi seleccionat.
//         case vtkCommand::RightButtonPressEvent:
//             this->selectROI();
//             break;
//
//         //resposta: als events del teclat
//         case vtkCommand::KeyPressEvent:
//             this->answerToKeyEvent();
//             break;
//
//         default:
//             break;
    }
}
///\TODO DE MOMENT NOMÉS PERMETEM CREAR EL·LIPSES
void ROITool::startROIAnnotation()
{
/*Hem de contemplar diferents casos:
    - si la ROI és d'alguns dels tipus següents, quadrat, rectangle, cercle o el·lipse, marquem dos punts i a partir d'aquí es construeixen els
    altres punts del polígon.
    -si la ROI és un polyline, caldran tants punts com vèrtexs tingui aquesta roi.
*/
    int xy[2];
    double position[4];

    //esborrem els punts i vèrtexs del polyData
    m_2DViewer->getInteractor()->GetEventPosition( xy );
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );

    //només ens interessen els 3 primers valors de l'array de 4
    m_startPosition[0] = position[0];
    m_startPosition[1] = position[1];
    m_startPosition[2] = position[2];

    //creem les dades de la roi actual: li passem en aquest moment una llista buida
    QList< double* > list;

    switch( m_ROIType )
    {
        case ELLIPSE:
            list << m_startPosition;
            list << m_startPosition;

            ROIToolData *data = new ROIToolData( list, "Ellipse" );

            //creem la representació de la distància actual
            m_ellipseRepresentation = new EllipseRepresentation( data );
//             m_ellipseRepresentation->getText()->shadowOn();

            //li diem al drawer del nostre q2dviewer que dibuixi l'el·lipse
            m_2DViewer->getOldDrawer()->drawEllipse( m_ellipseRepresentation->getEllipse(), m_2DViewer->getCurrentSlice(), m_2DViewer->getView() );
        break;
    }

    m_state = ANNOTATION_STARTED;
}
///\TODO tenir en compte segons el tipus de primitiva que estem dibuixant
void ROITool::doROISimulation()
{
    int xy[2];
    double position[4];

    m_2DViewer->getInteractor()->GetEventPosition (xy);
    m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer(), xy[0], xy[1], 0, position );

    //establem el segon punt d'anotació de la roi
    //només ens interessen els 3 primers valors de l'array de 4
    m_currentPosition[0] = position[0];
    m_currentPosition[1] = position[1];
    m_currentPosition[2] = position[2];

    //actualitzem l'objecte ROIToolData de la representació actual i això farà que s'actualitzi la línia en el visor
    Ellipse *ellipse = m_ellipseRepresentation->getEllipse();
    ellipse->setBottomRightPoint( m_currentPosition );

    //li diem a la representació que actualitzi l'el·lipse
    m_ellipseRepresentation->refreshEllipse();

    m_state = SIMULATING_ANNOTATION;
}

void ROITool::simulationOfNewPolyLinePoint( double *point1, double *point2 )
{
//     //afegim el nou segment de la polilínia per tal de simular la posició del següent punt
//     m_segment->SetPoint1( point1 );
//     m_segment->SetPoint2( point2 );
//     m_polyLineMapper->SetInput( m_segment->GetOutput() );
//     m_polyLineActor->VisibilityOn();
}

void ROITool::annotateNextPolylinePoint()
{
//     int xy[2];
//     int i = 0;
//     double position[4];
//
//     double *newPoint = new double[3];
//
//     m_2DViewer->getInteractor()->GetEventPosition( xy );
//     m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , xy[0], xy[1], 0, position );
//
//     //només ens interessen els 3 primers valors de l'array de 4
//     newPoint[0] = position[0];
//     newPoint[1] = position[1];
//     newPoint[2] = position[2];
//
//     //afegim el següent punt de la polilínia
//     m_newROIAssembly->m_pointsList.append( newPoint );
//     m_points->Reset();
//     m_vertexs->Reset();
//
//     //afegim els punts al vtkpolydata per a representar el nou punt afegit
//     m_vertexs->InsertNextCell( m_newROIAssembly->m_pointsList.count() );
//
//     foreach ( double *value, m_newROIAssembly->m_pointsList )
//     {
//         m_points->InsertPoint( i, value );
//         m_vertexs->InsertCellPoint( i );
//         i++;
//     }
//
//     m_ROI->SetPoints( m_points );
//     m_ROI->SetLines( m_vertexs );
//     m_ROIActor->VisibilityOn();
//     m_2DViewer->refresh();
}

void ROITool::stopROIAnnotation()
{
    //busquem la base i l'altura del rectangle que engloba l'el·lipse que ens serviran per calcular la seva àrea:
    //àrea el·lipse = PI * ((1/2)*base) * ((1/2)*alçada)
    double *topLeft = m_ellipseRepresentation->getEllipse()->getTopLeftPoint();
    double *bottomRight = m_ellipseRepresentation->getEllipse()->getBottomRightPoint();

    //per finalitzar la roi, calculem el text i la posició. També calculem les coordenades del fons del text
    m_ellipseRepresentation->refreshText( topLeft, bottomRight, m_2DViewer->getView() );

    //afegim al drawer un conjunt de primitives relacionades.
    m_2DViewer->getOldDrawer()->addSetOfPrimitives( m_ellipseRepresentation );

    m_state = STOPPED;

//
//     if ( m_ROIType != NO_SPECIFIED )
//     {
//         double *pact, *pseg;
//         double area = 0, mean = 0;
//         int i, j;
//         vtkPoints *p;
//
// //         vtkPolyData *auxpolydata = m_newROIAssembly->getPolyData();
//
//         if ( m_ROIType != POLYLINE )
//         {
//             //assignem a l'objecte ROIASSEMBLY els punts que tenim dins del vtkPolyData auxiliar que utilitzem per fer la simulació:
//             //per això cal fer una còpia d'aquests punts no trreballar sermpre amb una sola referència
//             vtkPoints *newPoints = vtkPoints::New();
//             vtkCellArray *newLines = vtkCellArray::New();
//
//             newPoints->DeepCopy( m_ROI->GetPoints() );
//             newLines->DeepCopy( m_ROI->GetLines() );
//
//             auxpolydata->SetPoints( newPoints );
//             auxpolydata->SetLines( newLines );
//
//             p = m_ROI->GetPoints();
//
//             for (i = 0; i < p->GetNumberOfPoints(); i++ )
//             {
//                 double *auxp = new double[3];
//                 p->GetPoint( i, auxp );
//                 m_newROIAssembly->m_pointsList.append( auxp );
//             }
//
// //             //afegim l'ultim punt per a tancar la ROI
// //             double *auxp = new double[3];
// //             p->GetPoint( 0, auxp );
// //             m_newROIAssembly->m_pointsList.append( auxp );
//
//             //busquem la base i l'altura del rectangle que generem amb el primer i segon punt de l'anotació, ja que ens serviran per calcular
//             //qualsevol de les àrees: rectangle = base * altura, cercle = PI * ((1/2)*base)2 i el·lipse = PI * ((1/2)*base) * ((1/2)*alçada)
//             double width = sqrt( pow((m_firstPoint[0]-m_secondPoint[0]), 2) + pow((m_firstPoint[2]-m_secondPoint[2]), 2) ) ;
//             double height = sqrt( pow((m_firstPoint[1]-m_secondPoint[1]), 2) + pow((m_firstPoint[2]-m_secondPoint[2]), 2) );
//
//             switch( m_ROIType )
//             {
//                 case RECTANGLE:
//                     area = width * height;
//                 break;
//
//                 case CIRCLE:
//                     area = M_PI * (width/2) * (width/2);
//                 break;
//
//                 case ELLIPSE:
//                     area = M_PI * (height/2) * (width/2);
//                 break;
//
//                 default:
//                     break;
//             }
//         }
//         else
//         {
//             // en el cas del polyline hem d'assignar els punts de la llista de l'assembly, ho fem aquí perquè no és un polígon regular
//             //i necessita un tractament especial.
//
//             //l'actor que visualitza l'últim segment el fem invisible perquè no volem que se'ns mostri per pantalla.
//             m_polyLineActor->VisibilityOff();
//
//             //càlcul de l'àrea del polígon definit per la polilínia
//             for ( j = 0; j < m_newROIAssembly->m_pointsList.count() ; j++ )
//             {
//                 pact = m_newROIAssembly->m_pointsList.at( j );
//
//                 if ( j == ( m_newROIAssembly->m_pointsList.count() - 1 ) )
//                     pseg = m_newROIAssembly->m_pointsList.at( 0 );
//                 else
//                     pseg = m_newROIAssembly->m_pointsList.at( (j+1) );
//
//                 area += (pact[0]+pseg[0])*(pseg[1]-pact[1]);
//             }
//
//             //en el cas de que l'àrea de la polilínia ens doni negativa, vol dir que hem anotat els punts en sentit antihorari,
//             //per això cal girar-los per tenir una disposició correcta. Cal girar-ho del vtkPoints i de la QList de la ROI
//             if ( area < 0 )
//             {
//                 //donem el resultat el valor absolut
//                 area *= -1;
//
//                 //intercanviem els punts de la QList
//                 m_newROIAssembly->swap();
//             }
//
//             //afegim els punts definitius a la polilínia
//             m_vertexs->InsertNextCell( (m_newROIAssembly->m_pointsList.count()+1) );
//
//             i = 0;
//
//             //afegim els punts de la llista a l'objecte vtkPoints
//             foreach ( double *value, m_newROIAssembly->m_pointsList )
//             {
//                 m_points->InsertPoint( i, value );
//                 m_vertexs->InsertCellPoint( i );
//                 i++;
//             }
//
//             //afegim un altre cop el primer punt perquè quedi tancat la ROI de tipus Polyline
//             m_points->InsertPoint( i, m_newROIAssembly->m_pointsList.first() );
//             m_vertexs->InsertCellPoint( i );
//
//             //assignem els punts al vtkPolYData auxiliar
//             auxpolydata->SetPoints( m_points );
//             auxpolydata->SetLines( m_vertexs );
//
//             m_points = vtkPoints::New();
//             m_vertexs = vtkCellArray::New();
//         }
//         //\TODO falta el càlcul de la mitjana
//         mean = 0;//computeMean( m_newROIAssembly );
//
//         m_newROIAssembly->setCaption( createCaption( calculateCaptionPosition( m_newROIAssembly->getPolyData() ), area ) );
//         m_newROIAssembly->setShapeType( m_ROIType );
//         this->saveIntoAList( m_newROIAssembly );
//         m_2DViewer->getRenderer()->AddActor2D( m_newROIAssembly->getCaption() );
//         m_newROIAssembly->getActor()->GetProperty()->SetColor( NormalColor.redF(), NormalColor.greenF(), NormalColor.blueF() );
//         m_newROIAssembly->getActor()->VisibilityOn();
//         m_2DViewer->getRenderer()->AddActor( m_newROIAssembly->getActor() );
//         m_ROIActor->VisibilityOff();
//         m_newROIAssembly->getCaption()->VisibilityOn();
//         m_2DViewer->refresh();
//     }
}

// double ROITool::getGrayValue( double *coords )
// {
//     double tol2;
//     vtkCell *cell;
//     vtkPointData *pd;
//     int subId;
//     double pcoords[3], weights[8];
//
//     vtkImageData* volume = m_2DViewer->getInput()->getVtkData();
//
//     if( !coords ) return 0.0;
//
//     pd = volume->GetPointData();
//
//     vtkPointData* outPD = vtkPointData::New();
//     outPD->InterpolateAllocate(pd, 1, 1);
//
//     tol2 = volume->GetLength();
//     tol2 = tol2 ? tol2*tol2 / 1000.0 : 0.001;
//
//     cell = volume->FindAndGetCell(coords,NULL,-1,tol2,subId,pcoords,weights);
//     if (cell)
//     {
//         outPD->InterpolatePoint(pd,0,cell->PointIds,weights);
//         return outPD->GetScalars()->GetTuple1(0);
//     } else
//         return -1;
//
//     outPD->Delete();
// }


// double ROITool::computeMean( ROIAssembly *newROIAssembly )
// {
//     double mean = 0;
//     int index,subId,initialPosition, endPosition;
//
//     vtkPolyData *auxpolydata = newROIAssembly->getPolyData();
//
//     //el nombre de segments és el mateix que el nombre de punts del polígon
//     int numberOfSements = newROIAssembly->m_pointsList.count();
//
//     //taula de punters a vtkLine per a representar cadascun dels segments del polígon
//     vtkLine* segments[ numberOfSements ];
//     //creem els diferents segments
//     for ( index = 0; index < numberOfSements; index++ )
//     {
//         segments[index] = vtkLine::New();
//         segments[index]->GetPointIds()->SetNumberOfIds(2);
//         segments[index]->GetPoints()->SetNumberOfPoints(2);
//
//         double *p1 = newROIAssembly->m_pointsList.at(index);
//         double *p2;
//
//         if ( index < ( numberOfSements - 1 ) )
//             p2 = newROIAssembly->m_pointsList.at( (index+1) );
//         else
//             p2 = newROIAssembly->m_pointsList.at( 0 );
//
//         segments[index]->GetPoints()->InsertPoint( 0, p1 );
//         segments[index]->GetPoints()->InsertPoint( 1, p2 );
//     }
//     double *bounds = auxpolydata->GetBounds();
//
// //     double coordZ = m_2DViewer->getInput()->getOrigin()[2];
//
//     //tracem punts horitzontals per trobar les interseccions amb els segments, col·locant-nos en les y mínimes i anant cap a les y màximes
//     double rayP1[3] = { bounds[0], bounds[2], bounds[4] /*coordZ*/};
//     double rayP2[3] = { bounds[1], bounds[2], bounds[4] /*coordZ*/};
//
//     double intersectPoint[3], *firstIntersection, *secondIntersection, pcoords[3], t;
//
//     int numberOfVoxels = 0;
//
//     QList<double*> intersectionList;
//     double *findedPoint;
//     m_2DViewer->getInput()->updateInformation();
//     double spacingX = m_2DViewer->getInput()->getSpacing()[0];
//     double spacingY = m_2DViewer->getInput()->getSpacing()[1];
//
//
//     //ens col·loquem a la posició bounds[2] (ymin) i hem d'anar fins a bounds[3] (ymax)
//     //es suposa que la coordenada que escombrem té un valor més petit cap a l'esquerra i més gran cap a la dreta
//
//     while( rayP1[1] <= bounds[3] )
//     {
//         intersectionList.clear();
//                 //obtenim les interseccions entre tots els segments de la ROI i el raig actual
//         for ( index = 0; index < numberOfSements; index++ )
//         {
//             if ( segments[index]->IntersectWithLine(rayP1, rayP2, 0.0001, t, intersectPoint, pcoords, subId) > 0)
//             {
//                 findedPoint = new double[3];
//
//                 findedPoint[0] = intersectPoint[0];
//                 findedPoint[1] = intersectPoint[1];
//                 findedPoint[2] = intersectPoint[2];
//
//                 intersectionList.append( findedPoint );
//             }
//         }
//
//         if ( (intersectionList.count() % 2)==0 )
//         {
//             for ( index = 0; index < (intersectionList.count()/2); index++ )
//             {
//                 initialPosition = index * 2;
//                 endPosition = initialPosition + 1;
//
//                 firstIntersection = intersectionList.at( initialPosition );
//                 secondIntersection = intersectionList.at( endPosition );
//
//                 //Tractem els dos sentits de les interseccions
//                 if (firstIntersection[0] <= secondIntersection[0])//d'esquerra cap a dreta
//                 {
//                     while ( firstIntersection[0] <= secondIntersection[0] )
//                     {
//                         mean += getGrayValue( firstIntersection );
//                         numberOfVoxels++;
//                         firstIntersection[0] += spacingX;
//                     }
//                 }
//                 else //de dreta cap a esquerra
//                 {
//                     while ( firstIntersection[0] >= secondIntersection[0] )
//                     {
//                         mean += getGrayValue( firstIntersection );
//                         numberOfVoxels++;
//                         firstIntersection[0] -= spacingX;
//                     }
//                 }
//             }
//         }
//         else
//             DEBUG_LOG( "EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!" );
//
//                 //fem el següent pas en la coordenada que escombrem
//         rayP1[1] += spacingY;
//         rayP2[1] += spacingY;
//     }
//
//     mean /= numberOfVoxels;
//
//             //destruïm els diferents segments que hem creat per simular la roi
//     for ( index = 0; index < numberOfSements; index++ )
//         segments[index]->Delete();
//
//     return mean;
// }

// void ROITool::selectROI()
// {
//     if( m_selectedROI )
//     {
//         this->setColor( m_selectedROI , NormalColor );
//
//         if( m_previousHighlightedROI )
//         {
//             if ( m_previousHighlightedROI == m_selectedROI )
//                 m_selectedROI = NULL;
//             else
//             {
//                 m_selectedROI = m_previousHighlightedROI;
//                 this->setColor( m_previousHighlightedROI , NormalColor );
//                 m_previousHighlightedROI = NULL;
//             }
//         }
//         else
//         {
//             this->setColor( m_selectedROI , NormalColor );
//             m_selectedROI = NULL;
//         }
//     }
//     else
//     {
//         int x, y;
//         double toWorld[4],point[2];
//         x = m_2DViewer->getInteractor()->GetEventPosition()[0];
//         y = m_2DViewer->getInteractor()->GetEventPosition()[1];
//         m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
//
//         switch( m_2DViewer->getView() )
//         {
//             case Q2DViewer::Axial:
//                 point[0] = toWorld[0];
//                 point[1] = toWorld[1];
//                 break;
//             case Q2DViewer::Sagittal:
//                 point[1] = toWorld[1];
//                 point[2] = toWorld[2];
//                 break;
//             case Q2DViewer::Coronal:
//                 point[0] = toWorld[0];
//                 point[2] = toWorld[2];
//                 break;
//             default:
//                 DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
//                 break;
//         }
//         m_selectedROI = this->getNearestROI( point );
//         this->setColor( m_selectedROI , SelectedColor );
//     }
//     m_2DViewer->refresh();
// }
//
// void ROITool::highlightNearestROI()
// {
//     double point[3] = { .0, .0, .0 };
//     int x = m_2DViewer->getInteractor()->GetEventPosition()[0];
//     int y = m_2DViewer->getInteractor()->GetEventPosition()[1];
//     double toWorld[4];
//     m_2DViewer->computeDisplayToWorld( m_2DViewer->getRenderer() , x, y , 0 , toWorld );
//
//     switch( m_2DViewer->getView() )
//     {
//         case Q2DViewer::Axial:
//             point[0] = toWorld[0];
//             point[1] = toWorld[1];
//             break;
//         case Q2DViewer::Sagittal:
//             point[1] = toWorld[1];
//             point[2] = toWorld[2];
//             break;
//         case Q2DViewer::Coronal:
//             point[0] = toWorld[0];
//             point[2] = toWorld[2];
//             break;
//         default:
//             DEBUG_LOG( "El Q2DViewer no té assignada cap de les 3 vistes possibles!?" );
//             break;
//     }
//
//     ROIAssembly *roi = (this->getNearestROI( point ));
//     if( roi )
//     {
//         this->setColor( roi , HighlightColor );
//         if( m_previousHighlightedROI != roi &&  m_previousHighlightedROI != m_selectedROI )
//             this->setColor( m_previousHighlightedROI , NormalColor );
//
//         m_previousHighlightedROI = roi;
//     }
//     else
//     {
//         if( m_previousHighlightedROI && m_previousHighlightedROI != m_selectedROI )
//             this->setColor( m_previousHighlightedROI , NormalColor );
//     }
// }

void ROITool::answerToKeyEvent()
{
    //responem a la intenció d'esborrar una ROI, sempre que hi hagi una ROI seleccionada i s'hagi polsat la tecla adequada (tecla sup)
//     char keyChar = m_2DViewer->getInteractor()->GetKeyCode();
//     int keyInt = (int)keyChar;
//
//     if ( m_selectedROI != NULL && keyInt == 127 )
//     {
//         QMutableMapIterator< int , ROIAssembly* > *iterator = NULL;
//         switch( m_lastView )
//         {
//         case Q2DViewer::Axial:
//             iterator = new QMutableMapIterator< int , ROIAssembly* >( m_ROIsOfAxialViewMap );
//         break;
//
//         case Q2DViewer::Sagittal:
//             iterator = new QMutableMapIterator< int , ROIAssembly* >( m_ROIsOfSagittalViewMap );
//         break;
//
//         case Q2DViewer::Coronal:
//             iterator = new QMutableMapIterator< int , ROIAssembly* >( m_ROIsOfCoronalViewMap );
//         break;
//
//         default:
//             DEBUG_LOG( "Valor inesperat" );
//         break;
//         }
//         if ( iterator )
//         {
//             iterator->toFront();
//             if( iterator->findNext( m_selectedROI ) )
//             {
//                 iterator->remove();
//                 m_2DViewer->getRenderer()->RemoveActor( (m_selectedROI->getActor()) );
//                 m_2DViewer->getRenderer()->RemoveActor( (m_selectedROI->getCaption()) );
//
//                 if( m_selectedROI == m_previousHighlightedROI )
//                     m_previousHighlightedROI = NULL;
//
//                 m_selectedROI = NULL;
//                 m_2DViewer->refresh();
//             }
//         }
//     }
}

// vtkCaptionActor2D* ROITool::createCaption( double *point, double area/*, double mean*/ )
// {
//     vtkCaptionActor2D *captionActor = vtkCaptionActor2D::New();
//     captionActor->GetAttachmentPointCoordinate()->SetCoordinateSystemToWorld();
//     captionActor->BorderOn();
//     captionActor->LeaderOff();
//     captionActor->ThreeDimensionalLeaderOff();
//     captionActor->GetCaptionTextProperty()->SetColor( 1.0, 0.7, .0 );
//     captionActor->SetPadding( 5 );
//     captionActor->SetPosition( -1.0 , -1.0 );
//     captionActor->SetHeight( 0.08 );
//     captionActor->SetWidth( 0.12 );
//     captionActor->GetCaptionTextProperty()->ShadowOff();
//     captionActor->GetCaptionTextProperty()->ItalicOff();
//
//     QString str1 = QString( "Area: %1 mm3" ).arg( area, 0, 'f',  2);
//
//     //Assignem el text a l'etiqueta de la distància i la situem
//     captionActor->SetCaption( qPrintable ( str1 ) );
//     captionActor->SetAttachmentPoint( point );
//
//     return ( captionActor );
// }

// double* ROITool::calculateCaptionPosition( vtkPolyData *roi )
// {
//     double bounds[6];
//     roi->GetBounds( bounds );
//     double *position = new double[3];
//
//     position[0] = bounds[1];
//     position[2] = bounds[5];
//
//     switch( m_2DViewer->getView() )
//     {
//         case Q2DViewer::Axial:
//             position[1] = bounds[2];
//             break;
//
//         case Q2DViewer::Sagittal:
//         case Q2DViewer::Coronal:
//             position[1] = bounds[3];
//             break;
//
//         default:
//             DEBUG_LOG( "Cap vista assignada" );
//             break;
//     }
//     return ( position );
// }

//per saber la ROI més propera, mirarem dins de quina està el punt. Aquesta que conté el punt, serà la més propera.
// ROIAssembly* ROITool::getNearestROI( double point3D[3] )
// {
//     QList< ROIAssembly* > ROIList;
//     char coordinateToZero;
//
//     //mirem la coordenada que no cal tenir en compte per tal de trovbar la ROI més propera
//     switch( m_2DViewer->getView() )
//     {
//         case Q2DViewer::Axial:
//             ROIList = m_ROIsOfAxialViewMap.values( m_2DViewer->getCurrentSlice() );
//             //la coordenada que s'ha de deixar a 0 és la z.
//             coordinateToZero = 'z';
//             break;
//
//         case Q2DViewer::Sagittal:
//             ROIList = m_ROIsOfSagittalViewMap.values( m_2DViewer->getCurrentSlice() );
//             //la coordenada que s'ha de deixar a 0 és la x.
//             coordinateToZero = 'x';
//             break;
//
//         case Q2DViewer::Coronal:
//             ROIList = m_ROIsOfCoronalViewMap.values( m_2DViewer->getCurrentSlice() );
//             //la coordenada que s'ha de deixar a 0 és la y.
//             coordinateToZero = 'y';
//             break;
//
//         default:
//             DEBUG_LOG( "vista del visor 2D no esperada!" );
//             break;
//     }
//
//     ROIAssembly *nearest = NULL;
//
//     double *bounds;
//
//     foreach( ROIAssembly *candidate, ROIList )
//     {
//         bounds = candidate->getPolyData()->GetBounds();
//
//         switch( coordinateToZero )
//         {
//             case 'x': //mirem que la y i la z del punt estigui entre les ymin i ymax i zmin i zmax. No tenim en compte la coordenada x.
//                 if ( bounds[2] <= point3D[1] && bounds[3] >= point3D[1] && bounds[4] <= point3D[2] && bounds[5] >= point3D[2] )
//                     nearest = candidate;
//
//                 break;
//
//             case 'y': //mirem que la x i la z del punt estigui entre les xmin i xmax i zmin i zmax. No tenim en compte la coordenada y.
//                 if ( bounds[0] <= point3D[0] && bounds[1] >= point3D[0] && bounds[4] <= point3D[2] && bounds[5] >= point3D[2] )
//                     nearest = candidate;
//                 break;
//
//             case 'z': //mirem que la x i la y del punt estigui entre les xmin i xmax i ymin i ymax. No tenim en compte la coordenada z.
//                 if ( bounds[0] <= point3D[0] && bounds[1] >= point3D[0] && bounds[2] <= point3D[1] && bounds[3] >= point3D[1] )
//                     nearest = candidate;
//                 break;
//         }
//     }
//     return nearest;
// }
}
