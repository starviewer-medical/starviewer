/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "polylineroitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "series.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "image.h"
#include "series.h"
#include "mathtools.h"
//vtk
#include <vtkPNGWriter.h>
#include <vtkImageActor.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>
#include <vtkProp.h>
#include <vtkLine.h>
#include <vtkPoints.h>
//Qt

namespace udg {

PolylineROITool::PolylineROITool( QViewer *viewer, QObject *parent )
 : ROITool(viewer, parent)
{
    m_toolName = "PolylineROITool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>( viewer );
    if( !m_2DViewer )
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ")+ viewer->metaObject()->className() );
    }

    connect(this, SIGNAL(finished()), this, SLOT(initCalc()));

    m_mainPolyline = NULL;
}

PolylineROITool::~PolylineROITool()
{
    if ( !m_mainPolyline.isNull() )
        delete m_mainPolyline;
}

void PolylineROITool::initCalc()
{
    if( m_mainPolyline == NULL )
        DEBUG_LOG(QString("PolylineROITool: La línia rebuda és nul·la!"));
    else
        printData();
}

void PolylineROITool::printData()
{
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

        const double * pixelSpacing = m_2DViewer->getInput()->getSeries()->getImages().at(0)->getPixelSpacing();

        if ( pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0 )
        {
            double * spacing = m_2DViewer->getInput()->getSpacing();
            text->setText( tr("Area: %1 px2\nMean: %2").arg( this->computeArea( spacing ), 0, 'f', 0 ).arg( this->computeGrayMean(), 0, 'f', 2 ) );
        }
        else
        {
            text->setText( tr("Area: %1 mm2\nMean: %2").arg( this->computeArea() ).arg( this->computeGrayMean(), 0, 'f', 2 ) );
        }

        text->setAttatchmentPoint( intersection );
        text->update( DrawerPrimitive::VTKRepresentation );
        m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
}

double PolylineROITool::computeGrayMean()
{
	double mean = 0.0;
    int i;
    int subId;
    int initialPosition;
    int endPosition;
    double intersectPoint[3];
    double *firstIntersection;
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
	int currentView = m_2DViewer->getView();

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_mainPolyline->getNumberOfPoints()-1;

    //taula de punters a vtkLine per a representar cadascun dels segments del polígon
    QVector<vtkLine*> segments;

    //creem els diferents segments
    for ( i = 0; i < numberOfSegments; i++ )
    {
        vtkLine *line = vtkLine::New();
        line->GetPointIds()->SetNumberOfIds(2);
        line->GetPoints()->SetNumberOfPoints(2);

        double *p1 = m_mainPolyline->getPoint( i );
        double *p2 = m_mainPolyline->getPoint( i+1 );

        line->GetPoints()->InsertPoint( 0, p1 );
        line->GetPoints()->InsertPoint( 1, p2 );

        segments << line;
    }

    double *bounds = m_mainPolyline->getPolylineBounds();
	double *spacing = m_2DViewer->getInput()->getSpacing();

	int rayPointIndex;
	int intersectionIndex;
	switch( currentView )
	{
	case Q2DViewer::Axial:
		rayP1[0] = bounds[0];//xmin
		rayP1[1] = bounds[2];//y
		rayP1[2] = bounds[4];//z
		rayP2[0] = bounds[1];//xmax
		rayP2[1] = bounds[2];//y
		rayP2[2] = bounds[4];//z

		rayPointIndex = 1;
		intersectionIndex = 0;
		verticalLimit = bounds[3];
	break;

	case Q2DViewer::Sagital:
		rayP1[0] = bounds[0];//xmin
		rayP1[1] = bounds[2];//ymin
		rayP1[2] = bounds[4];//zmin
		rayP2[0] = bounds[0];//xmin
		rayP2[1] = bounds[2];//ymin
		rayP2[2] = bounds[5];//zmax

		rayPointIndex = 1;
		intersectionIndex = 2;
		verticalLimit = bounds[3];

	break;

	case Q2DViewer::Coronal:
		rayP1[0] = bounds[0];//xmin
		rayP1[1] = bounds[2];//ymin
		rayP1[2] = bounds[4];//zmin
		rayP2[0] = bounds[1];//xmax
		rayP2[1] = bounds[2];//ymin
		rayP2[2] = bounds[4];//zmin

		rayPointIndex = 2;
		intersectionIndex = 0;
		verticalLimit = bounds[5];
	break;
	}

	while( rayP1[rayPointIndex] <= verticalLimit )
    {
        intersectionList.clear();
        indexList.clear();
        for ( i = 0; i < numberOfSegments; i++ )
        {
            auxPoints = segments[i]->GetPoints();
            auxPoints->GetPoint(0,p0);
            auxPoints->GetPoint(1,p1);
			if( (rayP1[rayPointIndex] <= p0[rayPointIndex] && rayP1[rayPointIndex] >= p1[rayPointIndex])
				|| (rayP1[rayPointIndex] >= p0[rayPointIndex] && rayP1[rayPointIndex] <= p1[rayPointIndex]) )
                indexList << i;

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
            for ( i = 0; i < limit; i++ )
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at( initialPosition );
                secondIntersection = intersectionList.at( endPosition );

                //Tractem els dos sentits de les interseccions
                if (firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex])//d'esquerra cap a dreta
                {
                    while ( firstIntersection[intersectionIndex] <= secondIntersection[intersectionIndex] )
                    {
                        mean += (double)getGrayValue( firstIntersection );
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] += spacing[0];
                    }
                }
                else //de dreta cap a esquerra
                {
                    while ( firstIntersection[intersectionIndex] >= secondIntersection[intersectionIndex] )
                    {
                        mean += (double)getGrayValue( firstIntersection );
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] -= spacing[0];
                    }
                }
            }
        }
        else
            DEBUG_LOG( "EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI Ã‰S IMPARELL!!" );

        //fem el següent pas en la coordenada que escombrem
		rayP1[rayPointIndex] += spacing[1];
        rayP2[rayPointIndex] += spacing[1];
    }

	 mean /= numberOfVoxels;

    //destruim els diferents segments que hem creat per simular la roi
    for ( i = 0; i < numberOfSegments; i++ )
        segments[i]->Delete();

    return mean;

}

double PolylineROITool::computeArea( const double * spacing )
{
    double area = 0.0;
    double actualPoint[3];
    double followPoint[3];
    double * point;
    for ( int j = 0; j < m_mainPolyline->getNumberOfPoints()-1; j++ )
    {
        point = m_mainPolyline->getPoint( j );
        actualPoint[0] = point[0];
        actualPoint[1] = point[1];
        actualPoint[2] = point[2];

        point = m_mainPolyline->getPoint( j+1 );
        followPoint[0] = point[0];
        followPoint[1] = point[1];
        followPoint[2] = point[2];

        if ( spacing != NULL )
        {
            actualPoint[0] = MathTools::trunc( actualPoint[0]/spacing[0] );
            actualPoint[1] = MathTools::trunc( actualPoint[1]/spacing[1] );
            actualPoint[2] = MathTools::trunc( actualPoint[2]/spacing[2] );
            followPoint[0] = MathTools::trunc( followPoint[0]/spacing[0] );
            followPoint[1] = MathTools::trunc( followPoint[1]/spacing[1] );
            followPoint[2] = MathTools::trunc( followPoint[2]/spacing[2] );
        }
        switch( m_2DViewer->getView() )
        {
            case Q2DViewer::Axial:
                area += ( ( followPoint[0]-actualPoint[0] )*(followPoint[1] + actualPoint[1] ) )/2.0;
                break;

            case Q2DViewer::Sagital:
                area += ( ( followPoint[2]-actualPoint[2] )*(followPoint[1] + actualPoint[1] ) )/2.0;
                break;

            case Q2DViewer::Coronal:
                area += ( ( followPoint[0]-actualPoint[0] )*(followPoint[2] + actualPoint[2] ) )/2.0;
                break;
        }
    }

     //en el cas de que l'àrea de la polilínia ens doni negativa, vol dir que hem anotat els punts en sentit antihorari,
     //per això cal girar-los per tenir una disposició correcta. Cal girar-ho del vtkPoints i de la QList de la ROI
     if ( area < 0 )
     {
        //donem el resultat el valor absolut
        area *= -1;

        //intercanviem els punts de la QList
        m_mainPolyline->swap();
    }
    return area;
}

}
