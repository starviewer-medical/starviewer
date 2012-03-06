#include "roitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"
#include "image.h"
#include "series.h"
#include "mathtools.h"

#include <QApplication>

namespace udg {

ROITool::ROITool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_roiPolygon(0), m_hasToComputeStatisticsData(true), m_mean(0.0), m_standardDeviation(0.0)
{
    m_toolName = "ROITool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }
}

ROITool::~ROITool()
{
}

void ROITool::computeStatisticsData()
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
    QList<double*> intersectionList;
    QList<int> intersectedSegmentsIndexList;
    double sweepLineBeginPoint[3];
    double sweepLineEndPoint[3];
    double verticalLimit;
    int currentView = m_2DViewer->getView();

    // Creem una còpia de m_roiPolygon projectada a la mateixa profunditat que la llesca actual
    // Serà amb aquest polígon amb el que calcularem els corresponents valors de vòxel
    DrawerPolygon *projectedROIPolygon = createProjectedROIPolygon();
    // El nombre de segments és el mateix que el nombre de punts del polígon
    int numberOfSegments = projectedROIPolygon->getNumberOfPoints();

    // Llistes de punts inicials i finals de cada segement
    QVector<const double*> segmentsStartPoints;
    QVector<const double*> segmentsEndPoints;

    // Creem els diferents segments
    for (int i = 0; i < numberOfSegments - 1; ++i)
    {
        const double *p1 = projectedROIPolygon->getVertix(i);
        const double *p2 = projectedROIPolygon->getVertix(i + 1);
        segmentsStartPoints.append(p1);
        segmentsEndPoints << p2;
    }
    // Cal afegir l'últim segment que es correspondria amb el segment de l'últim punt al primer
    const double *p1 = projectedROIPolygon->getVertix(numberOfSegments - 1);
    const double *p2 = projectedROIPolygon->getVertix(0);
    segmentsStartPoints.append(p1);
    segmentsEndPoints << p2;

    // Traçarem una lína d'escombrat dins de la regió quadrangular que ocupa el polígon
    // Aquesta línia produirà unes interseccions amb els segments del polígon
    // Les interseccions marcaran el camí a seguir per fer el recompte de vòxels
    double bounds[6];
    projectedROIPolygon->getBounds(bounds);
    double *spacing = m_2DViewer->getInput()->getSpacing();

    double horizontalSpacingIncrement;
    double verticalSpacingIncrement;
    int sweepLineCoordinateIndex;
    int intersectionCoordinateIndex;
    switch (currentView)
    {
        case Q2DViewer::Axial:
            // xmin
            sweepLineBeginPoint[0] = bounds[0];
            // ymin
            sweepLineBeginPoint[1] = bounds[2];
            // zmin
            sweepLineBeginPoint[2] = bounds[4];
            // xmax
            sweepLineEndPoint[0] = bounds[1];
            // ymin
            sweepLineEndPoint[1] = bounds[2];
            // zmin
            sweepLineEndPoint[2] = bounds[4];

            sweepLineCoordinateIndex = 1;
            intersectionCoordinateIndex = 0;
            verticalLimit = bounds[3];

            horizontalSpacingIncrement = spacing[0];
            verticalSpacingIncrement = spacing[1];
            break;

        case Q2DViewer::Sagital:
            // xmin
            sweepLineBeginPoint[0] = bounds[0];
            // ymin
            sweepLineBeginPoint[1] = bounds[2];
            // zmin
            sweepLineBeginPoint[2] = bounds[4];
            // xmin
            sweepLineEndPoint[0] = bounds[0];
            // ymin
            sweepLineEndPoint[1] = bounds[2];
            // zmax
            sweepLineEndPoint[2] = bounds[5];

            sweepLineCoordinateIndex = 1;
            intersectionCoordinateIndex = 2;
            verticalLimit = bounds[3];

            horizontalSpacingIncrement = spacing[1];
            verticalSpacingIncrement = spacing[2];
            break;

        case Q2DViewer::Coronal:
            // xmin
            sweepLineBeginPoint[0] = bounds[0];
            // ymin
            sweepLineBeginPoint[1] = bounds[2];
            // zmin
            sweepLineBeginPoint[2] = bounds[4];
            // xmax
            sweepLineEndPoint[0] = bounds[1];
            // ymin
            sweepLineEndPoint[1] = bounds[2];
            // zmin
            sweepLineEndPoint[2] = bounds[4];

            sweepLineCoordinateIndex = 2;
            intersectionCoordinateIndex = 0;
            verticalLimit = bounds[5];

            horizontalSpacingIncrement = spacing[0];
            verticalSpacingIncrement = spacing[2];
            break;
    }

    int intersectionState;
    // Obtenim el punter al contenidor de píxels amb el que calcularem els valors
    VolumePixelData *pixelData = 0;
    int phaseIndex = 0;
    int numberOfPhases = 1;
    if (m_2DViewer->isThickSlabActive())
    {
        pixelData = new VolumePixelData;
        pixelData->setData(m_2DViewer->getCurrentSlabProjection());
    }
    else
    {
        pixelData = m_2DViewer->getInput()->getPixelData();
        if (m_2DViewer->getView() == Q2DViewer::Axial && m_2DViewer->getInput()->getNumberOfPhases() > 1)
        {
            numberOfPhases = m_2DViewer->getInput()->getNumberOfPhases();
            phaseIndex = m_2DViewer->getCurrentPhase();
        }
    }

    // Inicialitzem la llista de valors de gris
    m_grayValues.clear();
    while (sweepLineBeginPoint[sweepLineCoordinateIndex] <= verticalLimit)
    {
        intersectionList.clear();
        intersectedSegmentsIndexList.clear();
        for (int i = 0; i < numberOfSegments; ++i)
        {
            if ((sweepLineBeginPoint[sweepLineCoordinateIndex] <= segmentsStartPoints.at(i)[sweepLineCoordinateIndex] &&
                sweepLineBeginPoint[sweepLineCoordinateIndex] >= segmentsEndPoints.at(i)[sweepLineCoordinateIndex])
            || (sweepLineBeginPoint[sweepLineCoordinateIndex] >= segmentsStartPoints.at(i)[sweepLineCoordinateIndex] &&
                sweepLineBeginPoint[sweepLineCoordinateIndex] <= segmentsEndPoints.at(i)[sweepLineCoordinateIndex]))
            {
                intersectedSegmentsIndexList << i;
            }
        }

        // Obtenim les interseccions entre tots els segments de la ROI i la línia d'escombrat actual
        foreach (int segmentIndex, intersectedSegmentsIndexList)
        {
            double *foundPoint = MathTools::infiniteLinesIntersection((double*)segmentsStartPoints.at(segmentIndex),
                                                                      (double*)segmentsEndPoints.at(segmentIndex),
                                                                      sweepLineBeginPoint, sweepLineEndPoint, intersectionState);
            if (intersectionState == MathTools::LinesIntersect)
            {
                // Cal ordenar les interseccions en la direcció horitzontal per tal que el recompte de píxels es faci correctament
                bool found = false;
                int i = 0;
                while (!found && i < intersectionList.count())
                {
                    if (foundPoint[intersectionCoordinateIndex] > intersectionList.at(i)[intersectionCoordinateIndex])
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
        if ((intersectionList.count() % 2) == 0)
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
                if (firstIntersection[intersectionCoordinateIndex] <= secondIntersection[intersectionCoordinateIndex])
                {
                    while (firstIntersection[intersectionCoordinateIndex] <= secondIntersection[intersectionCoordinateIndex])
                    {
                        QVector<double> voxelValue;
                        if (pixelData->getVoxelValue(firstIntersection, voxelValue, phaseIndex, numberOfPhases))
                        {
                            m_grayValues << voxelValue.at(0);
                        }
                        firstIntersection[intersectionCoordinateIndex] += horizontalSpacingIncrement;
                    }
                }
                // I de dreta cap a esquerra
                else
                {
                    while (firstIntersection[intersectionCoordinateIndex] >= secondIntersection[intersectionCoordinateIndex])
                    {
                        QVector<double> voxelValue;
                        if (pixelData->getVoxelValue(firstIntersection, voxelValue, phaseIndex, numberOfPhases))
                        {
                            m_grayValues << voxelValue.at(0);
                        }
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

    // Alliberem el pixel data, en cas que haguem creat un nou objecte
    if (m_2DViewer->isThickSlabActive())
    {
        delete pixelData;
    }

    // Ja no necessitem més la còpia del polígon, per tant es pot eliminar de memòria
    delete projectedROIPolygon;
}

DrawerPolygon *ROITool::createProjectedROIPolygon()
{
    Q_ASSERT(m_roiPolygon);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
    // Calculem la coordenda de profunditat a la que volem projectar el polígon
    Volume *input = m_2DViewer->getInput();
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
    // HACK Comprovem si l'imatge té pixel spacing per saber si la mesura ha d'anar en píxels o mm
    // TODO proporcionar algun mètode alternatiu per no haver d'haver de fer aquest hack
    const double *pixelSpacing = m_2DViewer->getInput()->getImage(0)->getPixelSpacing();
    QString areaUnits;
    double spacing[3];
    if (pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0)
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
        if (m_2DViewer->getInput()->getImage(0)->getParentSeries()->getModality() == "US")
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

    QString annotation = tr("Area: %1 %2").arg(m_roiPolygon->computeArea(m_2DViewer->getView(), spacing), 0, 'f', 0).arg(areaUnits);
    // Només calcularem mitjana i desviació estàndar per imatges monocrom.
    if (m_2DViewer->getInput()->getImage(0)->getPhotometricInterpretation().contains("MONOCHROME"))
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
