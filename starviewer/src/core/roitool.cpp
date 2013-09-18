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
 : MeasurementTool(viewer, parent), m_roiPolygon(0), m_hasToComputeStatisticsData(true), m_mean(0.0), m_standardDeviation(0.0)
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

void ROITool::computeMean()
{
    m_mean = 0.0;
    foreach (double value, m_grayValues)
    {
        m_mean += value;
    }

    m_mean = m_mean / m_grayValues.size();
}

void ROITool::computeStandardDeviation()
{
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
}

void ROITool::computeStatisticsData()
{
    Q_ASSERT(m_roiPolygon);

    // Només cal calcular les dades si és necessari
    if (!m_hasToComputeStatisticsData)
    {
        return;
    }

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
    double sweepLineBeginPoint[3];
    double sweepLineEndPoint[3];
    // Bounds xMin, yMin, zMin
    sweepLineBeginPoint[xIndex] = bounds[xIndex * 2];
    sweepLineBeginPoint[yIndex] = bounds[yIndex * 2];
    sweepLineBeginPoint[zIndex] = bounds[zIndex * 2];
    // Bounds xMax, yMin, zMin
    sweepLineEndPoint[xIndex] = bounds[xIndex * 2 + 1];
    sweepLineEndPoint[yIndex] = bounds[yIndex * 2];
    sweepLineEndPoint[zIndex] = bounds[zIndex * 2];

    // Bounds yMax
    double verticalLimit = bounds[yIndex * 2 + 1];

    double *spacing = m_2DViewer->getMainInput()->getSpacing();
    double horizontalSpacingIncrement = spacing[xIndex];
    double verticalSpacingIncrement = spacing[yIndex];

    // Obtenim el punter al contenidor de píxels amb el que calcularem els valors
    VolumePixelData *pixelData = m_2DViewer->getCurrentPixelData();
    
    int phaseIndex = 0;
    int numberOfPhases = 1;
    if (!m_2DViewer->isThickSlabActive() && m_2DViewer->getView() == OrthogonalPlane::XYPlane && m_2DViewer->hasPhases())
    {
        numberOfPhases = m_2DViewer->getNumberOfPhases();
        phaseIndex = m_2DViewer->getCurrentPhase();
    }

    int initialPosition;
    int endPosition;
    double *firstIntersection;
    double *secondIntersection;
    QList<double*> intersectionList;
    QList<int> intersectedSegmentsIndexList;
    // Inicialitzem la llista de valors de gris
    m_grayValues.clear();
    while (sweepLineBeginPoint[yIndex] <= verticalLimit)
    {
        intersectionList.clear();

        intersectedSegmentsIndexList = getIndexOfSegmentsCrossingAtHeight(polygonSegments, sweepLineBeginPoint[yIndex], yIndex);
        // Obtenim les interseccions entre tots els segments de la ROI i la línia d'escombrat actual
        foreach (int segmentIndex, intersectedSegmentsIndexList)
        {
            int intersectionState;
            double *foundPoint = MathTools::infiniteLinesIntersection(polygonSegments.at(segmentIndex).getFirstPoint().getAsDoubleArray(),
                                                                      polygonSegments.at(segmentIndex).getSecondPoint().getAsDoubleArray(),
                                                                      sweepLineBeginPoint, sweepLineEndPoint, intersectionState);
            if (intersectionState == MathTools::LinesIntersect)
            {
                // Cal ordenar les interseccions en la direcció horitzontal per tal que el recompte de píxels es faci correctament
                bool found = false;
                int i = 0;
                while (!found && i < intersectionList.count())
                {
                    if (foundPoint[xIndex] > intersectionList.at(i)[xIndex])
                    {
                        intersectionList.insert(i, foundPoint);
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
        if (MathTools::isEven(intersectionList.count()))
        {
            int limit = intersectionList.count() / 2;
            for (int i = 0; i < limit; ++i)
            {
                initialPosition = i * 2;
                endPosition = initialPosition + 1;

                firstIntersection = intersectionList.at(initialPosition);
                secondIntersection = intersectionList.at(endPosition);

                // Tractem els dos sentits de les interseccions
                // D'esquerra cap a dreta
                if (firstIntersection[xIndex] <= secondIntersection[xIndex])
                {
                    while (firstIntersection[xIndex] <= secondIntersection[xIndex])
                    {
                        Voxel voxel = pixelData->getVoxelValue(firstIntersection, phaseIndex, numberOfPhases);
                        if (!voxel.isEmpty())
                        {
                            m_grayValues << voxel.getComponent(0);
                        }
                        firstIntersection[xIndex] += horizontalSpacingIncrement;
                    }
                }
                // I de dreta cap a esquerra
                else
                {
                    while (firstIntersection[xIndex] >= secondIntersection[xIndex])
                    {
                        Voxel voxel = pixelData->getVoxelValue(firstIntersection, phaseIndex, numberOfPhases);
                        if (!voxel.isEmpty())
                        {
                            m_grayValues << voxel.getComponent(0);
                        }
                        firstIntersection[xIndex] -= horizontalSpacingIncrement;
                    }
                }
            }
        }
        else
        {
            DEBUG_LOG("EL NOMBRE D'INTERSECCIONS ENTRE EL RAIG I LA ROI ÉS IMPARELL!!");
        }

        // Desplacem la línia d'escombrat en la direcció que toca tant com espaiat de píxel tinguem en aquella direcció
        sweepLineBeginPoint[yIndex] += verticalSpacingIncrement;
        sweepLineEndPoint[yIndex] += verticalSpacingIncrement;
    }
    
    // Un cop hem obtingut les dades necessàries, calculem la mitjana i la desviació estàndar

    // Mitjana
    computeMean();

    // Desviació estàndar
    computeStandardDeviation();

    // Ja s'han calculat les dades estadístiques
    m_hasToComputeStatisticsData = false;

    // Alliberem el pixel data, en cas que haguem creat un nou objecte
    if (m_2DViewer->isThickSlabActive())
    {
        delete pixelData;
    }
}

DrawerPolygon *ROITool::createProjectedROIPolygon()
{
    Q_ASSERT(m_roiPolygon);

    int xIndex, yIndex, zIndex;
    m_2DViewer->getView().getXYZIndexes(xIndex, yIndex, zIndex);
    // Calculem la coordenda de profunditat a la que volem projectar el polígon
    Volume *input = m_2DViewer->getMainInput();
    double origin[3];
    double spacing[3];
    input->getOrigin(origin);
    input->getSpacing(spacing);
    double zCoordinate = origin[zIndex] + spacing[zIndex] * m_2DViewer->getCurrentSlice();

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
        computeStatisticsData();
        QApplication::restoreOverrideCursor();

        // Afegim la informació de les dades estadístiques a l'annotació
        annotation += tr("\nMean: %1\nSt.Dev.: %2").arg(m_mean, 0, 'f', 2).arg(m_standardDeviation, 0, 'f', 2);
    }

    return annotation;
}

}
