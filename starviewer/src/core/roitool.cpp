#include "roitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"
#include "image.h"
#include "mathtools.h"
#include "areameasurecomputer.h"
#include "voxel.h"

#include <QApplication>

namespace udg {

ROITool::ROITool(QViewer *viewer, QObject *parent)
 : MeasurementTool(viewer, parent), m_roiPolygon(0)
{
    m_toolName = "ROITool";
    m_hasSharedData = false;
}

ROITool::~ROITool()
{
}

MeasureComputer* ROITool::getMeasureComputer()
{
    return new AreaMeasureComputer(m_roiPolygon);
}

double ROITool::computeMean(const QList<double> &grayValues)
{
    double mean = 0.0;
    foreach (double value, grayValues)
    {
        mean += value;
    }

    mean = mean / grayValues.size();

    return mean;
}

double ROITool::computeStandardDeviation(const QList<double> &grayValues, double meanOfGrayValues)
{
    double standardDeviation = 0.0;
    QList<double> deviations;
    foreach (double value, grayValues)
    {
        double individualDeviation = value - meanOfGrayValues;
        deviations << (individualDeviation * individualDeviation);
    }

    foreach (double deviation, deviations)
    {
        standardDeviation += deviation;
    }

    standardDeviation /= deviations.size();
    standardDeviation = std::sqrt(standardDeviation);

    return standardDeviation;
}

ROITool::StatisticsData ROITool::computeStatisticsData()
{
    Q_ASSERT(m_roiPolygon);

    // List with the segments of the polygon
    QList<Line3D> polygonSegments = m_roiPolygon->getSegments();

    // Traçarem una lína d'escombrat dins de la regió quadrangular que ocupa el polígon
    // Aquesta línia produirà unes interseccions amb els segments del polígon
    // Les interseccions marcaran el camí a seguir per fer el recompte de vòxels
    double bounds[6];
    m_roiPolygon->getBounds(bounds);

    OrthogonalPlane currentView = m_2DViewer->getView();
    int xIndex, yIndex, zIndex;
    currentView.getXYZIndexes(xIndex, yIndex, zIndex);

    // Initialization of the sweep line
    
    Point3D sweepLineBeginPoint;
    // Bounds xMin, yMin, zMin
    sweepLineBeginPoint[xIndex] = bounds[xIndex * 2];
    sweepLineBeginPoint[yIndex] = bounds[yIndex * 2];
    sweepLineBeginPoint[zIndex] = bounds[zIndex * 2];
    
    Point3D sweepLineEndPoint;
    // Bounds xMax, yMin, zMin
    sweepLineEndPoint[xIndex] = bounds[xIndex * 2 + 1];
    sweepLineEndPoint[yIndex] = bounds[yIndex * 2];
    sweepLineEndPoint[zIndex] = bounds[zIndex * 2];

    // Bounds yMax
    double verticalLimit = bounds[yIndex * 2 + 1];

    // Obtenim el punter al contenidor de píxels amb el que calcularem els valors
    VolumePixelData *pixelData = m_2DViewer->getCurrentPixelData();
    
    double spacing[3];
    pixelData->getSpacing(spacing);
    double verticalSpacingIncrement = spacing[yIndex];
    
    int phaseIndex = 0;
    if (!m_2DViewer->isThickSlabActive() && m_2DViewer->getView() == OrthogonalPlane::XYPlane && m_2DViewer->hasPhases())
    {
        phaseIndex = m_2DViewer->getCurrentPhase();
    }

    double currentZDepth = m_2DViewer->getCurrentDisplayedImageDepth();
    
    QList<double*> intersectionList;
    // Inicialitzem la llista de valors de gris
    QList<double> grayValues;
    while (sweepLineBeginPoint.at(yIndex) <= verticalLimit)
    {
        // Obtenim les interseccions entre tots els segments de la ROI i la línia d'escombrat actual
        intersectionList = getIntersectionPoints(polygonSegments, Line3D(sweepLineBeginPoint, sweepLineEndPoint), currentView);

        // Fem el recompte de píxels
        addVoxelsFromIntersections(intersectionList, currentZDepth, currentView, pixelData, phaseIndex, grayValues);
        
        // Desplacem la línia d'escombrat en la direcció que toca tant com espaiat de píxel tinguem en aquella direcció
        sweepLineBeginPoint[yIndex] += verticalSpacingIncrement;
        sweepLineEndPoint[yIndex] += verticalSpacingIncrement;
    }
    
    // Un cop hem obtingut les dades necessàries, calculem la mitjana i la desviació estàndar
    StatisticsData data;
    // Mitjana
    data.m_mean = computeMean(grayValues);

    // Desviació estàndar
    data.m_standardDeviation = computeStandardDeviation(grayValues, data.m_mean);

    return data;
}

QList<int> ROITool::getIndexOfSegmentsCrossingAtHeight(const QList<Line3D> &segments, double height, int heightIndex)
{
    QList<int> intersectedSegmentsIndexList;

    for (int i = 0; i < segments.count(); ++i)
    {
        if ((height <= segments.at(i).getFirstPoint().at(heightIndex) && height >= segments.at(i).getSecondPoint().at(heightIndex))
        || (height >= segments.at(i).getFirstPoint().at(heightIndex) && height <= segments.at(i).getSecondPoint().at(heightIndex)))
        {
            intersectedSegmentsIndexList << i;
        }
    }

    return intersectedSegmentsIndexList;
}

QList<double*> ROITool::getIntersectionPoints(const QList<Line3D> &polygonSegments, const Line3D &sweepLine, const OrthogonalPlane &view)
{
    QList<double*> intersectionPoints;
    int sortIndex = view.getXIndex();
    int heightIndex = view.getYIndex();
    
    QList<int> indexListOfSegmentsToIntersect = getIndexOfSegmentsCrossingAtHeight(polygonSegments, sweepLine.getFirstPoint().at(heightIndex), heightIndex);
    foreach (int segmentIndex, indexListOfSegmentsToIntersect)
    {
        int intersectionState;
        double *foundPoint = MathTools::infiniteLinesIntersection(polygonSegments.at(segmentIndex).getFirstPoint().getAsDoubleArray(),
                                                                    polygonSegments.at(segmentIndex).getSecondPoint().getAsDoubleArray(),
                                                                    sweepLine.getFirstPoint().getAsDoubleArray(), sweepLine.getSecondPoint().getAsDoubleArray(),
                                                                    intersectionState);
        if (intersectionState == MathTools::LinesIntersect)
        {
            // Cal ordenar les interseccions en la direcció horitzontal per tal que el recompte de píxels es faci correctament
            bool found = false;
            int i = 0;
            while (!found && i < intersectionPoints.count())
            {
                if (foundPoint[sortIndex] > intersectionPoints.at(i)[sortIndex])
                {
                    intersectionPoints.insert(i, foundPoint);
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
                intersectionPoints << foundPoint;
            }
        }
    }

    return intersectionPoints;
}

void ROITool::addVoxelsFromIntersections(const QList<double*> &intersectionPoints, double currentZDepth, const OrthogonalPlane &view, VolumePixelData *pixelData, int phaseIndex, QList<double> &grayValues)
{
    if (MathTools::isEven(intersectionPoints.count()))
    {
        int scanDirectionIndex = view.getXIndex();
        double spacing[3];
        pixelData->getSpacing(spacing);
        double scanDirectionIncrement = spacing[scanDirectionIndex];
        
        int zIndex = view.getZIndex();

        int limit = intersectionPoints.count() / 2;
        for (int i = 0; i < limit; ++i)
        {
            double *firstIntersection = intersectionPoints.at(i * 2);
            double *secondIntersection = intersectionPoints.at(i * 2 + 1);
            // First we check which will be the direction of the scan line
            Point3D currentScanLinePoint;
            double scanLineEnd;
            if (firstIntersection[scanDirectionIndex] <= secondIntersection[scanDirectionIndex])
            {
                for (int i = 0; i < 3; ++i)
                {
                    currentScanLinePoint[i] = firstIntersection[i];
                }
                scanLineEnd = secondIntersection[scanDirectionIndex];
            }
            else
            {
                for (int i = 0; i < 3; ++i)
                {
                    currentScanLinePoint[i] = secondIntersection[i];
                }
                scanLineEnd = firstIntersection[scanDirectionIndex];
            }
            // Then we scan and get the voxels along the line
            while (currentScanLinePoint.at(scanDirectionIndex) <= scanLineEnd)
            {
                Point3D voxelCoordinate(currentScanLinePoint.getAsDoubleArray());
                voxelCoordinate[zIndex] = currentZDepth;
                
                Voxel voxel = pixelData->getVoxelValue(voxelCoordinate.getAsDoubleArray(), phaseIndex);
                if (!voxel.isEmpty())
                {
                    grayValues << voxel.getComponent(0);
                }
                currentScanLinePoint[scanDirectionIndex] += scanDirectionIncrement;
            }
        }
    }
    else
    {
        DEBUG_LOG(QString("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!: %1").arg(intersectionPoints.count()));
    }
}

void ROITool::printData()
{
    QString annotation = getAnnotation();

    DrawerText *text = new DrawerText;
    text->setText(annotation);

    setTextPosition(text);

    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

void ROITool::setTextPosition(DrawerText *text)
{
    double bounds[6];
    m_roiPolygon->getBounds(bounds);

    double attachmentPoint[3];
    attachmentPoint[0] = (bounds[1] + bounds[0]) / 2.0;
    attachmentPoint[1] = (bounds[3] + bounds[2]) / 2.0;
    attachmentPoint[2] = (bounds[5] + bounds[4]) / 2.0;

    text->setAttachmentPoint(attachmentPoint);
}

QString ROITool::getAnnotation()
{
    Q_ASSERT(m_roiPolygon);
    
    QString annotation = tr("Area: %1").arg(getMeasurementString());

    // Només calcularem mitjana i desviació estàndar per imatges monocrom.
    if (m_2DViewer->getMainInput()->getImage(0)->getPhotometricInterpretation().contains("MONOCHROME"))
    {
        // Calculem les dades estadístiques
        QApplication::setOverrideCursor(Qt::WaitCursor);
        StatisticsData statistics = computeStatisticsData();
        QApplication::restoreOverrideCursor();

        // Afegim la informació de les dades estadístiques a l'annotació
        annotation += tr("\nMean: %1\nSt.Dev.: %2").arg(statistics.m_mean, 0, 'f', 2).arg(statistics.m_standardDeviation, 0, 'f', 2);
    }

    return annotation;
}

}
