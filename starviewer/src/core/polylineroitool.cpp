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
 : ROITool(viewer, parent), m_mean(0.0), m_standardDeviation(0.0)
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
        double spacing[3];
        if ( pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0 )
        {
            // Si no coneixem l'spacing ho mostrem en pixels.
            double *vtkSpacing = m_2DViewer->getInput()->getSpacing();
            spacing[0] = 1.0 / vtkSpacing[0];
            spacing[1] = 1.0 / vtkSpacing[1];
            spacing[2] = 1.0 / vtkSpacing[2];

            areaUnits = "px2";
        }
        else        
        {
            // HACK Es fa aquesta comprovació perquè en US les vtk no agafen correctament el pixel spacing.
            if ( m_2DViewer->getInput()->getImage(0)->getParentSeries()->getModality() == "US" )
            {
                double *vtkSpacing = m_2DViewer->getInput()->getSpacing();
                spacing[0] = pixelSpacing[0] / vtkSpacing[0];
                spacing[1] = pixelSpacing[1] / vtkSpacing[1];
                spacing[2] = 1.0 / vtkSpacing[2];
            }
            else
            {
                spacing[0] = spacing[1] = spacing[2] = 1.0;
            }
            areaUnits = "mm2";
        }

        // Calculem les dades estadístiques
        computeStatisticsData();
        text->setText( tr("Area: %1 %2\nMean: %3\nSt.Dev.: %4").arg( m_roiPolygon->computeArea( m_2DViewer->getView(), spacing ), 0, 'f', 0 ).arg(areaUnits).arg( m_mean, 0, 'f', 2 ).arg( m_standardDeviation, 0, 'f', 2 ) );

        text->setAttachmentPoint( intersection );
        text->update( DrawerPrimitive::VTKRepresentation );
        text->shadowOn();
        m_2DViewer->getDrawer()->draw( text , m_2DViewer->getView(), m_2DViewer->getCurrentSlice() );
    }
}

void PolylineROITool::computeStatisticsData()
{
    // Només cal calcular les dades si és necessari
    if( !m_hasToComputeStatisticsData )
        return;
    
    int i;
    int initialPosition;
    int endPosition;
    double *firstIntersection;
    double *secondIntersection;
    QList<double*> intersectionList;
    QList<int> intersectedSegmentsIndexList;
    double sweepLineBeginPoint[3];
    double sweepLineEndPoint[3];
    double verticalLimit;
	int currentView = m_2DViewer->getView();

    // El nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_roiPolygon->getNumberOfPoints();

    // Llistes de punts inicials i finals de cada segement
    QVector<const double *> segmentsStartPoints;
    QVector<const double *> segmentsEndPoints;

    // Creem els diferents segments
    for ( i = 0; i < numberOfSegments-1; i++ )
    {
        const double *p1 = m_roiPolygon->getVertix(i);
        const double *p2 = m_roiPolygon->getVertix(i+1);
        segmentsStartPoints.append( p1 );
        segmentsEndPoints << p2;
    }
    // Cal afegir l'últim segment que es correspondria amb el segment de l'últim punt al primer
    const double *p1 = m_roiPolygon->getVertix(numberOfSegments-1);
    const double *p2 = m_roiPolygon->getVertix(0);
    segmentsStartPoints.append( p1 );
    segmentsEndPoints << p2;

    // Traçarem una lína d'escombrat dins de la regió quadrangular que ocupa el polígon
    // Aquesta línia produirà unes interseccions amb els segments del polígon
    // Les interseccions marcaran el camí a seguir per fer el recompte de vòxels
    double *bounds = m_roiPolygon->getBounds();
	double *spacing = m_2DViewer->getInput()->getSpacing();

    double horizontalSpacingIncrement;
    double verticalSpacingIncrement;
    int sweepLineCoordinateIndex;
    int intersectionCoordinateIndex;
    switch( currentView )
    {
    case Q2DViewer::Axial:
        sweepLineBeginPoint[0] = bounds[0];//xmin
        sweepLineBeginPoint[1] = bounds[2];//ymin
        sweepLineBeginPoint[2] = bounds[4];//zmin
        sweepLineEndPoint[0] = bounds[1];//xmax
        sweepLineEndPoint[1] = bounds[2];//ymin
        sweepLineEndPoint[2] = bounds[4];//zmin

        sweepLineCoordinateIndex = 1;
        intersectionCoordinateIndex = 0;
        verticalLimit = bounds[3];

        horizontalSpacingIncrement = spacing[0];
        verticalSpacingIncrement = spacing[1];
        break;

    case Q2DViewer::Sagital:
        sweepLineBeginPoint[0] = bounds[0];//xmin
        sweepLineBeginPoint[1] = bounds[2];//ymin
        sweepLineBeginPoint[2] = bounds[4];//zmin
        sweepLineEndPoint[0] = bounds[0];//xmin
        sweepLineEndPoint[1] = bounds[2];//ymin
        sweepLineEndPoint[2] = bounds[5];//zmax

        sweepLineCoordinateIndex = 1;
        intersectionCoordinateIndex = 2;
        verticalLimit = bounds[3];

        horizontalSpacingIncrement = spacing[1];
        verticalSpacingIncrement = spacing[2];
        break;

    case Q2DViewer::Coronal:
        sweepLineBeginPoint[0] = bounds[0];//xmin
        sweepLineBeginPoint[1] = bounds[2];//ymin
        sweepLineBeginPoint[2] = bounds[4];//zmin
        sweepLineEndPoint[0] = bounds[1];//xmax
        sweepLineEndPoint[1] = bounds[2];//ymin
        sweepLineEndPoint[2] = bounds[4];//zmin

        sweepLineCoordinateIndex = 2;
        intersectionCoordinateIndex = 0;
        verticalLimit = bounds[5];

        horizontalSpacingIncrement = spacing[0];
        verticalSpacingIncrement = spacing[2];
        break;
    }

    int intersectionState;
    // Inicialitzem la llista de valors de gris
    m_grayValues.clear();
	while( sweepLineBeginPoint[sweepLineCoordinateIndex] <= verticalLimit )
    {
        intersectionList.clear();
        intersectedSegmentsIndexList.clear();
        for ( i = 0; i < numberOfSegments; i++ )
        {
            if( (sweepLineBeginPoint[sweepLineCoordinateIndex] <= segmentsStartPoints.at(i)[sweepLineCoordinateIndex] && sweepLineBeginPoint[sweepLineCoordinateIndex] >= segmentsEndPoints.at(i)[sweepLineCoordinateIndex])
            || (sweepLineBeginPoint[sweepLineCoordinateIndex] >= segmentsStartPoints.at(i)[sweepLineCoordinateIndex] && sweepLineBeginPoint[sweepLineCoordinateIndex] <= segmentsEndPoints.at(i)[sweepLineCoordinateIndex]) )
                intersectedSegmentsIndexList << i;
        }
        
        // Obtenim les interseccions entre tots els segments de la ROI i la línia d'escombrat actual
        foreach (int segmentIndex, intersectedSegmentsIndexList)
        {
            double *foundPoint = MathTools::infiniteLinesIntersection( (double *)segmentsStartPoints.at(segmentIndex), (double *)segmentsEndPoints.at(segmentIndex), sweepLineBeginPoint, sweepLineEndPoint, intersectionState );
            if( intersectionState == MathTools::LinesIntersect )
            {
                // Cal ordenar les interseccions en la direcció horitzontal per tal que el recompte de píxels es faci correctament
                bool found = false;
                int i = 0;
                while(!found && i<intersectionList.count() )
                {
                    if( foundPoint[intersectionCoordinateIndex] > intersectionList.at(i)[intersectionCoordinateIndex] )
                    {
                        intersectionList.insert(i,foundPoint);
                        found = true;
                    }
                    else
                        i++;
                }
                // Si tots els punts són més grans, cal inserir la intersecció al final
                if( !found )
                    intersectionList << foundPoint;
            }
        }

        // Fem el recompte de píxels
        if ( (intersectionList.count() % 2)==0 )
        {
            int limit = intersectionList.count()/2;
            for ( i = 0; i < limit; i++ )
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at( initialPosition );
                secondIntersection = intersectionList.at( endPosition );

                // Tractem els dos sentits de les interseccions
                // D'esquerra cap a dreta
                if (firstIntersection[intersectionCoordinateIndex] <= secondIntersection[intersectionCoordinateIndex])
                {
                    while ( firstIntersection[intersectionCoordinateIndex] <= secondIntersection[intersectionCoordinateIndex] )
                    {
                        m_grayValues << (double)getGrayValue( firstIntersection );
                        firstIntersection[intersectionCoordinateIndex] += horizontalSpacingIncrement;
                    }
                }
                else // I de dreta cap a esquerra
                {
                    while ( firstIntersection[intersectionCoordinateIndex] >= secondIntersection[intersectionCoordinateIndex] )
                    {
                        m_grayValues << (double)getGrayValue( firstIntersection );
                        firstIntersection[intersectionCoordinateIndex] -= horizontalSpacingIncrement;
                    }
                }
            }
        }
        else
            DEBUG_LOG( "EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!" );

        // Desplacem la línia d'escombrat en la direcció que toca tant com espaiat de píxel tinguem en aquella direcció
        sweepLineBeginPoint[sweepLineCoordinateIndex] += verticalSpacingIncrement;
        sweepLineEndPoint[sweepLineCoordinateIndex] += verticalSpacingIncrement;
    }
    // Un cop hem obtingut les dades necessàries, calculem la mitjana i la desviació estàndar
    
    // Mitjana
    m_mean = 0.0;
    foreach ( double value, m_grayValues )
        m_mean += value;
    
    m_mean = m_mean / m_grayValues.size();
    
    // Desviació estàndar
    m_standardDeviation = 0.0;
    QList<double> deviations;
    foreach ( double value, m_grayValues )
    {
        double individualDeviation = value - m_mean;
        deviations << ( individualDeviation * individualDeviation );
    }

    foreach ( double deviation, deviations )
        m_standardDeviation += deviation;

    m_standardDeviation /= deviations.size();
    m_standardDeviation = std::sqrt( m_standardDeviation );

    // Ja s'han calculat les dades estadístiques
    m_hasToComputeStatisticsData = false;
}

}
