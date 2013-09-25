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

void ROITool::computeStatisticsData(double &mean, double &standardDeviation)
{
    Q_ASSERT(m_roiPolygon);

    // Creem una còpia de m_roiPolygon projectada a la mateixa profunditat que la llesca actual
    // Serà amb aquest polígon amb el que calcularem els corresponents valors de vòxel
    DrawerPolygon *projectedROIPolygon = createProjectedROIPolygon();

    // List with the segments of the polygon
    QList<Line3D> polygonSegments = projectedROIPolygon->getSegments();

    // Traçarem una lína d'escombrat dins de la regió quadrangular que ocupa el polígon
    // Aquesta línia produirà unes interseccions amb els segments del polígon
    // Les interseccions marcaran el camí a seguir per fer el recompte de vòxels
    double bounds[6];
    projectedROIPolygon->getBounds(bounds);
    
    // Ja no necessitem més la còpia del polígon, per tant es pot eliminar de memòria
    delete projectedROIPolygon;

    int xIndex, yIndex, zIndex;
    m_2DViewer->getView().getXYZIndexes(xIndex, yIndex, zIndex);

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

    QList<double*> intersectionList;
    QList<int> intersectedSegmentsIndexList;
    // Inicialitzem la llista de valors de gris
    QList<double> grayValues;
    while (sweepLineBeginPoint.at(yIndex) <= verticalLimit)
    {
        intersectedSegmentsIndexList = getIndexOfSegmentsCrossingAtHeight(polygonSegments, sweepLineBeginPoint.at(yIndex), yIndex);
        // Obtenim les interseccions entre tots els segments de la ROI i la línia d'escombrat actual
        intersectionList = getIntersectionPoints(polygonSegments, intersectedSegmentsIndexList, Line3D(sweepLineBeginPoint, sweepLineEndPoint), xIndex);

        // Fem el recompte de píxels
        addVoxelsFromIntersections(intersectionList, xIndex, pixelData, phaseIndex, grayValues);
        
        // Desplacem la línia d'escombrat en la direcció que toca tant com espaiat de píxel tinguem en aquella direcció
        sweepLineBeginPoint[yIndex] += verticalSpacingIncrement;
        sweepLineEndPoint[yIndex] += verticalSpacingIncrement;
    }
    
    // Un cop hem obtingut les dades necessàries, calculem la mitjana i la desviació estàndar

    // Mitjana
    mean = computeMean(grayValues);

    // Desviació estàndar
    standardDeviation = computeStandardDeviation(grayValues, mean);
}

DrawerPolygon *ROITool::createProjectedROIPolygon()
{
    Q_ASSERT(m_roiPolygon);

    int xIndex, yIndex, zIndex;
    m_2DViewer->getView().getXYZIndexes(xIndex, yIndex, zIndex);
    // Calculem la coordenda de profunditat a la que volem projectar el polígon
    double zCoordinate = m_2DViewer->getCurrentDisplayedImageDepth();

    DrawerPolygon *projectedROIPolygon = new DrawerPolygon;
    int numberOfPolygonPoints = m_roiPolygon->getNumberOfPoints();
    for (int i = 0; i < numberOfPolygonPoints; ++i)
    {
        const double *vertix = m_roiPolygon->getVertix(i);
        double projectedVertix[3];
        projectedVertix[xIndex] = vertix[xIndex];
        projectedVertix[yIndex] = vertix[yIndex];
        projectedVertix[zIndex] = zCoordinate;
        projectedROIPolygon->addVertix(projectedVertix);
    }

    // Necessari perquè després necessitem el getBounds() que es calcula amb els objectes de vtk
    // TODO Haver de fer aquesta crida ens planteja si el disseny dels DrawerPrimitive és prou bo
    // o que potser en aquest cas estem fent servir un DrawerPrimitive per una tasca per la que no està pensat
    projectedROIPolygon->getAsVtkProp();
    return projectedROIPolygon;
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

QList<double*> ROITool::getIntersectionPoints(const QList<Line3D> &polygonSegments, const QList<int> &indexListOfSegmentsToIntersect, const Line3D &sweepLine, int sortIndex)
{
    QList<double*> intersectionPoints;
    
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

void ROITool::addVoxelsFromIntersections(const QList<double*> &intersectionPoints, int scanDirectionIndex, VolumePixelData *pixelData, int phaseIndex, QList<double> &grayValues)
{
    if (MathTools::isEven(intersectionPoints.count()))
    {
        double spacing[3];
        pixelData->getSpacing(spacing);
        double scanDirectionIncrement = spacing[scanDirectionIndex];
        int firstPointIndex;
        int secondPointIndex;
        int limit = intersectionPoints.count() / 2;
        for (int i = 0; i < limit; ++i)
        {
            firstPointIndex = i * 2;
            secondPointIndex = firstPointIndex + 1;

            double *firstIntersection = intersectionPoints.at(firstPointIndex);
            double *secondIntersection = intersectionPoints.at(secondPointIndex);

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
                Voxel voxel = pixelData->getVoxelValue(currentScanLinePoint.getAsDoubleArray(), phaseIndex);
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
        double mean;
        double standardDeviation;
        QApplication::setOverrideCursor(Qt::WaitCursor);
        computeStatisticsData(mean, standardDeviation);
        QApplication::restoreOverrideCursor();

        // Afegim la informació de les dades estadístiques a l'annotació
        annotation += tr("\nMean: %1\nSt.Dev.: %2").arg(mean, 0, 'f', 2).arg(standardDeviation, 0, 'f', 2);
    }

    return annotation;
}

}
