/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "roitoolrepresentation.h"

#include "series.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"
#include "image.h"
#include "mathtools.h"
#include "q2dviewer.h"
#include "volume.h"
#include "toolhandler.h"
// vtk
#include <vtkImageData.h>

namespace udg {

ROIToolRepresentation::ROIToolRepresentation(Drawer *drawer, QObject *parent)
 : ToolRepresentation(drawer, parent), m_hasToComputeStatisticsData(true), m_mean(0.0), m_standardDeviation(0.0)
{
}

ROIToolRepresentation::~ROIToolRepresentation()
{
    delete m_roiPolygon;
    delete m_text;

    foreach (ToolHandler *handler, m_toolHandlers)
    {
        delete handler;
    }

    m_toolHandlers.clear();
    m_toolHandlersMap.clear();
}

void ROIToolRepresentation::setPolygon(DrawerPolygon *polygon)
{
    m_roiPolygon = polygon;
    m_text = new DrawerText;
    m_primitivesList << m_roiPolygon << m_text;
    m_hasToComputeStatisticsData = true;
}

void ROIToolRepresentation::setParameters(int view, int slice, Volume *input, vtkImageData *slabProjection, bool thickSlabInUse)
{
    m_view = view;
    m_slice = slice;
    m_input = input;
    m_slabProjection = slabProjection;
    m_thickSlabInUse = thickSlabInUse;
}

void ROIToolRepresentation::calculate()
{
    this->printData();

    if (m_toolHandlers.size() == 0) // Already created
    {
        this->createHandlers();
    }
}

Volume::VoxelType ROIToolRepresentation::getGrayValue(double *coordinate)
{
    double *origin = m_input->getOrigin();
    double *spacing = m_input->getSpacing(); // TODO Fer servir m_vtkSpacing?
    int index[3];

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_view);
    index[xIndex] = (int)((coordinate[xIndex] - origin[xIndex]) / spacing[xIndex]);
    index[yIndex] = (int)((coordinate[yIndex] - origin[yIndex]) / spacing[yIndex]);
    index[zIndex] = m_slice;

    if (m_thickSlabInUse)
    {
        return *((Volume::VoxelType *)m_slabProjection->GetScalarPointer(index));
    }
    else
    {
        return *(m_input->getScalarPointer(index));
    }
}

void ROIToolRepresentation::printData()
{
    Q_ASSERT(m_roiPolygon);
    // HACK Comprovem si l'imatge té pixel spacing per saber si la mesura ha d'anar en píxels o mm
    // TODO proporcionar algun mètode alternatiu per no haver d'haver de fer aquest hack
    const double *pixelSpacing = m_input->getImage(0)->getPixelSpacing();
    QString areaUnits;
    double spacing[3];
    if (pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0)
    {
        // Si no coneixem l'spacing ho mostrem en pixels.
        double *vtkSpacing = m_input->getSpacing();
        spacing[0] = 1.0 / vtkSpacing[0];
        spacing[1] = 1.0 / vtkSpacing[1];
        spacing[2] = 1.0 / vtkSpacing[2];

        areaUnits = "px2";
    }
    else
    {
        // HACK Es fa aquesta comprovació perquè en US les vtk no agafen correctament el pixel spacing.
        if (m_input->getImage(0)->getParentSeries()->getModality() == "US")
        {
            double *vtkSpacing = m_input->getSpacing();
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

    m_text->setText(tr("Area: %1 %2\nMean: %3\nSt.Dev.: %4").arg(m_roiPolygon->computeArea(m_view, spacing), 0, 'f', 0).arg(areaUnits).arg(m_mean, 0, 'f', 2).arg(m_standardDeviation, 0, 'f', 2));

    placeText();
    m_text->shadowOn();
    m_drawer->drawWorkInProgress(m_text);
}

void ROIToolRepresentation::placeText()
{
    Q_ASSERT(m_roiPolygon);
    Q_ASSERT(m_text);
    
    double bounds[6];
    m_roiPolygon->getBounds(bounds);
    
    double *attachmentPoint = new double[3];
    attachmentPoint[0] = (bounds[1] + bounds[0]) / 2.0;
    attachmentPoint[1] = (bounds[3] + bounds[2]) / 2.0;
    attachmentPoint[2] = (bounds[5] + bounds[4]) / 2.0;
    
    m_text->setAttachmentPoint(attachmentPoint);
}

void ROIToolRepresentation::computeStatisticsData()
{
    Q_ASSERT(m_roiPolygon);
    
    // Només cal calcular les dades si és necessari
    if (!m_hasToComputeStatisticsData)
    {
        return;
    }
    
    int initialPosition;
    int endPosition;
    double *firstIntersection;
    double *secondIntersection;
    QList<double *> intersectionList;
    QList<int> intersectedSegmentsIndexList;
    double sweepLineBeginPoint[3];
    double sweepLineEndPoint[3];
    double verticalLimit;

    // El nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = m_roiPolygon->getNumberOfPoints();

    // Llistes de punts inicials i finals de cada segement
    QVector<const double *> segmentsStartPoints;
    QVector<const double *> segmentsEndPoints;

    // Creem els diferents segments
    for (int i = 0; i < numberOfSegments - 1; ++i)
    {
        const double *p1 = m_roiPolygon->getVertix(i);
        const double *p2 = m_roiPolygon->getVertix(i + 1);
        segmentsStartPoints.append(p1);
        segmentsEndPoints << p2;
    }
    // Cal afegir l'últim segment que es correspondria amb el segment de l'últim punt al primer
    const double *p1 = m_roiPolygon->getVertix(numberOfSegments - 1);
    const double *p2 = m_roiPolygon->getVertix(0);
    segmentsStartPoints.append(p1);
    segmentsEndPoints << p2;

    // Traçarem una lína d'escombrat dins de la regió quadrangular que ocupa el polígon
    // Aquesta línia produirà unes interseccions amb els segments del polígon
    // Les interseccions marcaran el camí a seguir per fer el recompte de vòxels
    double bounds[6];
    m_roiPolygon->getBounds(bounds);
    double *spacing = m_input->getSpacing();

    double horizontalSpacingIncrement;
    double verticalSpacingIncrement;
    int sweepLineCoordinateIndex;
    int intersectionCoordinateIndex;
    switch (m_view)
    {
        case Q2DViewer::Axial:
            sweepLineBeginPoint[0] = bounds[0]; // xmin
            sweepLineBeginPoint[1] = bounds[2]; // ymin
            sweepLineBeginPoint[2] = bounds[4]; // zmin
            sweepLineEndPoint[0] = bounds[1]; // xmax
            sweepLineEndPoint[1] = bounds[2]; // ymin
            sweepLineEndPoint[2] = bounds[4]; // zmin

            sweepLineCoordinateIndex = 1;
            intersectionCoordinateIndex = 0;
            verticalLimit = bounds[3];

            horizontalSpacingIncrement = spacing[0];
            verticalSpacingIncrement = spacing[1];
            break;

        case Q2DViewer::Sagital:
            sweepLineBeginPoint[0] = bounds[0]; // xmin
            sweepLineBeginPoint[1] = bounds[2]; // ymin
            sweepLineBeginPoint[2] = bounds[4]; // zmin
            sweepLineEndPoint[0] = bounds[0]; // xmin
            sweepLineEndPoint[1] = bounds[2]; // ymin
            sweepLineEndPoint[2] = bounds[5]; // zmax

            sweepLineCoordinateIndex = 1;
            intersectionCoordinateIndex = 2;
            verticalLimit = bounds[3];

            horizontalSpacingIncrement = spacing[1];
            verticalSpacingIncrement = spacing[2];
            break;

        case Q2DViewer::Coronal:
            sweepLineBeginPoint[0] = bounds[0]; // xmin
            sweepLineBeginPoint[1] = bounds[2]; // ymin
            sweepLineBeginPoint[2] = bounds[4]; // zmin
            sweepLineEndPoint[0] = bounds[1]; // xmax
            sweepLineEndPoint[1] = bounds[2]; // ymin
            sweepLineEndPoint[2] = bounds[4]; // zmin

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
    while (sweepLineBeginPoint[sweepLineCoordinateIndex] <= verticalLimit)
    {
        intersectionList.clear();
        intersectedSegmentsIndexList.clear();
        for (int i = 0; i < numberOfSegments; ++i)
        {
            if ((sweepLineBeginPoint[sweepLineCoordinateIndex] <= segmentsStartPoints.at(i)[sweepLineCoordinateIndex] && sweepLineBeginPoint[sweepLineCoordinateIndex] >= segmentsEndPoints.at(i)[sweepLineCoordinateIndex])
            || (sweepLineBeginPoint[sweepLineCoordinateIndex] >= segmentsStartPoints.at(i)[sweepLineCoordinateIndex] && sweepLineBeginPoint[sweepLineCoordinateIndex] <= segmentsEndPoints.at(i)[sweepLineCoordinateIndex]))
            {
                intersectedSegmentsIndexList << i;
            }
        }
        
        // Obtenim les interseccions entre tots els segments de la ROI i la línia d'escombrat actual
        foreach (int segmentIndex, intersectedSegmentsIndexList)
        {
            double *foundPoint = MathTools::infiniteLinesIntersection((double *)segmentsStartPoints.at(segmentIndex), (double *)segmentsEndPoints.at(segmentIndex), sweepLineBeginPoint, sweepLineEndPoint, intersectionState);
            if (intersectionState == MathTools::LinesIntersect)
            {
                // Cal ordenar les interseccions en la direcció horitzontal per tal que el recompte de píxels es faci correctament
                bool found = false;
                int i = 0;
                while (!found && i<intersectionList.count())
                {
                    if (foundPoint[intersectionCoordinateIndex] > intersectionList.at(i)[intersectionCoordinateIndex])
                    {
                        intersectionList.insert(i,foundPoint);
                        found = true;
                    }
                    else
                    {
                        ++i;
                    }
                }
                // Si tots els punts són més grans, cal inserir la intersecció al final
                if (!found)
                {
                    intersectionList << foundPoint;
                }
            }
        }

        // Fem el recompte de píxels
        if ((intersectionList.count() % 2) == 0)
        {
            int limit = intersectionList.count()/2;
            for (int i = 0; i < limit; ++i)
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at(initialPosition);
                secondIntersection = intersectionList.at(endPosition);

                // Tractem els dos sentits de les interseccions
                // D'esquerra cap a dreta
                if (firstIntersection[intersectionCoordinateIndex] <= secondIntersection[intersectionCoordinateIndex])
                {
                    while (firstIntersection[intersectionCoordinateIndex] <= secondIntersection[intersectionCoordinateIndex])
                    {
                        m_grayValues << (double)getGrayValue(firstIntersection);
                        firstIntersection[intersectionCoordinateIndex] += horizontalSpacingIncrement;
                    }
                }
                else // I de dreta cap a esquerra
                {
                    while (firstIntersection[intersectionCoordinateIndex] >= secondIntersection[intersectionCoordinateIndex])
                    {
                        m_grayValues << (double)getGrayValue(firstIntersection);
                        firstIntersection[intersectionCoordinateIndex] -= horizontalSpacingIncrement;
                    }
                }
            }
        }
        else
        {
            DEBUG_LOG("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!");
        }

        // Desplacem la línia d'escombrat en la direcció que toca tant com espaiat de píxel tinguem en aquella direcció
        sweepLineBeginPoint[sweepLineCoordinateIndex] += verticalSpacingIncrement;
        sweepLineEndPoint[sweepLineCoordinateIndex] += verticalSpacingIncrement;
    }
    // Un cop hem obtingut les dades necessàries, calculem la mitjana i la desviació estàndar
    
    // Mitjana
    m_mean = 0.0;
    foreach (double value, m_grayValues)
    {
        m_mean += value;
    }
    
    m_mean = m_mean / m_grayValues.size();
    
    // Desviació estàndar
    m_standardDeviation = 0.0;
    QList<double> deviations;
    foreach (double value, m_grayValues)
    {
        double individualDeviation = value - m_mean;
        deviations << (individualDeviation * individualDeviation);
    }

    foreach (double deviation, deviations)
    {
        m_standardDeviation += deviation;
    }

    m_standardDeviation /= deviations.size();
    m_standardDeviation = std::sqrt(m_standardDeviation);

    // Ja s'han calculat les dades estadístiques
    m_hasToComputeStatisticsData = false;
}

void ROIToolRepresentation::moveAllPoints(double *movement)
{
    double *point;

    // Primer movem tots els punts del polígon
    // TODO Caldria tenir en compte que la ROI no surt dels bounds de la imatge
    //      Si surt dels bounds, parar el desplaçament
    for (int i = 0; i < m_roiPolygon->getNumberOfPoints(); ++i)
    {
        point = (double *)m_roiPolygon->getVertix(i);

        point[0] += movement[0];
        point[1] += movement[1];
        point[2] += movement[2];
    }

    // I després movem la ubicació del texte
    // TODO Seria millor recalcular el posicionament del texte, ja que podríem no 
    //      fer tot el moviment si aquest fa sortir dels bounds la ROI
    placeText();

    this->refresh();
}

QList<double *> ROIToolRepresentation::mapHandlerToPoint(ToolHandler *toolHandler)
{
    const int key = m_toolHandlersMap.key(toolHandler);

    QList<double *> list;
    list << (double *)m_roiPolygon->getVertix(key);

    return list;
}

}

