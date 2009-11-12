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
#include "drawerpolygon.h"
#include "drawertext.h"
#include "image.h"
#include "mathtools.h"

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

    connect(this, SIGNAL(finished()), this, SLOT(start()));

    m_roiPolygon = NULL;
}

PolylineROITool::~PolylineROITool()
{
    if ( !m_roiPolygon.isNull() )
        delete m_roiPolygon;
}

void PolylineROITool::start()
{
    if( m_roiPolygon == NULL )
        DEBUG_LOG(QString("PolylineROITool: La línia rebuda és nul·la!"));
    else
        printData();
}

void PolylineROITool::printData()
{
    double *bounds = m_roiPolygon->getBounds();
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
        // HACK Comprovem si l'imatge té pixel spacing per saber si la mesura ha d'anar en píxels o mm
        // TODO proporcionar algun mètode alternatiu per no haver d'haver de fer aquest hack
        const double *pixelSpacing = m_2DViewer->getInput()->getImage(0)->getPixelSpacing();
        QString areaUnits;
        if ( pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0 )
            areaUnits = "px2";
        else        
            areaUnits = "mm2";

        text->setText( tr("Area: %1 %2\nMean: %3\nSt.Dev.: %4").arg( m_roiPolygon->computeArea( m_2DViewer->getView() ), 0, 'f', 0 ).arg(areaUnits).arg( this->computeGrayMean(), 0, 'f', 2 ).arg( this->computeStandardDeviation(), 0, 'f', 2 ) );

        text->setAttatchmentPoint( intersection );
        text->update( DrawerPrimitive::VTKRepresentation );
        m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
}

void PolylineROITool::computeGrayValues()
{
    int i;
    int initialPosition;
    int endPosition;
    double *firstIntersection;
    double *secondIntersection;
    int numberOfVoxels = 0;
    QList<double*> intersectionList;
    QList<int> indexList;
    double rayP1[3];
    double rayP2[3];
    double verticalLimit;
	int currentView = m_2DViewer->getView();

    //el nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_roiPolygon->getNumberOfPoints()-1;

    // Llistes de punts inicials i finals de cada segement
    QVector<const double *> segmentsStartPoints;
    QVector<const double *> segmentsEndPoints;

    // Creem els diferents segments
    for ( i = 0; i < numberOfSegments; i++ )
    {
        const double *p1 = m_roiPolygon->getVertix(i);
        const double *p2 = m_roiPolygon->getVertix(i+1);
        segmentsStartPoints.append( p1 );
        segmentsEndPoints << p2;
    }

    double *bounds = m_roiPolygon->getBounds();
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

    int intersectionState;
	while( rayP1[rayPointIndex] <= verticalLimit )
    {
        intersectionList.clear();
        indexList.clear();
        for ( i = 0; i < numberOfSegments; i++ )
        {
            if( (rayP1[rayPointIndex] <= segmentsStartPoints.at(i)[rayPointIndex] && rayP1[rayPointIndex] >= segmentsEndPoints.at(i)[rayPointIndex])
            || (rayP1[rayPointIndex] >= segmentsStartPoints.at(i)[rayPointIndex] && rayP1[rayPointIndex] <= segmentsEndPoints.at(i)[rayPointIndex]) )
                indexList << i;
        }
        
        //obtenim les interseccions entre tots els segments de la ROI i el raig actual
        foreach (int segment, indexList)
        {
            double *foundPoint = MathTools::intersectionPoint3DLines( (double *)segmentsStartPoints.at(segment), (double *)segmentsEndPoints.at(segment), rayP1, rayP2, intersectionState );
            if( intersectionState == MathTools::LinesIntersect )
                intersectionList << foundPoint;
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
                        grayValues << (double)getGrayValue( firstIntersection );
                        numberOfVoxels++;
                        firstIntersection[intersectionIndex] += spacing[0];
                    }
                }
                else //de dreta cap a esquerra
                {
                    while ( firstIntersection[intersectionIndex] >= secondIntersection[intersectionIndex] )
                    {
                        grayValues << (double)getGrayValue( firstIntersection );
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
}

double PolylineROITool::computeGrayMean()
{
    computeGrayValues();

    double mean = 0.0;

    foreach ( double value, grayValues )
        mean += value;

    //no es buida la llista pq la utilitzara computeStandardDeviation()

    return mean / grayValues.size();
}

double PolylineROITool::computeStandardDeviation()
{
    // no cal computeGrayValues(); ja ho ha fet computeGrayMean, que sempre es crida just abans
    double standardDeviation = 0.0;
    double mean = computeGrayMean();

    QList<double> deviations;

    foreach ( double value, grayValues )
    {
        double individualDeviation = value - mean;
        deviations << ( individualDeviation * individualDeviation );
    }

    grayValues.clear();

    foreach ( double deviation, deviations )
        standardDeviation += deviation;

    standardDeviation /= deviations.size();

    return std::sqrt( standardDeviation );
}

}
