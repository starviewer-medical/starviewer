#include "polylineroitool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawerpolyline.h"
#include "drawertext.h"
#include "mathtools.h"

// Vtk
#include <vtkRenderWindowInteractor.h>
#include <vtkCommand.h>

namespace udg {

PolylineROITool::PolylineROITool(QViewer *viewer, QObject *parent)
 : ROITool(viewer, parent), m_mainPolyline(0), m_closingPolyline(0)
{
    m_toolName = "PolylineROITool";

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
}

PolylineROITool::~PolylineROITool()
{
    deleteTemporalRepresentation();
}

void PolylineROITool::deleteTemporalRepresentation()
{
    bool hasToRefresh = false;
    // Cal decrementar el reference count perquè
    // l'annotació s'esborri si "matem" l'eina
    if (!m_mainPolyline.isNull())
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
        hasToRefresh = true;
    }

    if (!m_closingPolyline.isNull())
    {
        m_closingPolyline->decreaseReferenceCount();
        delete m_closingPolyline;
        hasToRefresh = true;
    }

    if (!m_roiPolygon.isNull())
    {
        delete m_roiPolygon;
    }

    if (hasToRefresh)
    {
        m_2DViewer->render();
    }
}

void PolylineROITool::handleEvent(long unsigned eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            handlePointAddition();
            break;

        case vtkCommand::MouseMoveEvent:
            simulateClosingPolyline();
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27) // ESC
            {
                deleteTemporalRepresentation();
            }
            break;
    }
}

void PolylineROITool::handlePointAddition()
{
    if (m_2DViewer->getInput())
    {
        switch (m_2DViewer->getInteractor()->GetRepeatCount())
        {
            // Single-click o primer click d'un doble click. Afegim un nou punt a la ROI
            case 0:
                annotateNewPoint();
                break;
            // Doble-click, si tenim més de 2 punts, llavors tanquem la ROI
            case 1:
                if (m_mainPolyline->getNumberOfPoints() > 2)
                {
                    closeForm();
                }
                break;
        }
    }
}

void PolylineROITool::annotateNewPoint()
{
    double pickedPoint[3];
    m_2DViewer->getEventWorldCoordinate(pickedPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);

    bool firstPoint = false;
    if (!m_mainPolyline)
    {
        firstPoint = true;
        m_mainPolyline = new DrawerPolyline;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_mainPolyline->increaseReferenceCount();
    }
    // Afegim el punt de la nova polilínia
    m_mainPolyline->addPoint(pickedPoint);

    // L'afegim a l'escena
    if (firstPoint)
    {
        m_2DViewer->getDrawer()->draw(m_mainPolyline);
    }
    // Actualitzem l'estructura interna
    else
    {
        m_mainPolyline->update();
    }

    if (!m_roiPolygon)
    {
        m_roiPolygon = new DrawerPolygon;
        // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
        m_roiPolygon->increaseReferenceCount();
    }
    // Afegim el punt al polígon de la ROI
    m_roiPolygon->addVertix(pickedPoint);

    // Com que estem afegint punts cal indicar que si és necessari recalcular les dades estadístiques
    m_hasToComputeStatisticsData = true;
}

void PolylineROITool::simulateClosingPolyline()
{
    if (m_mainPolyline && (m_mainPolyline->getNumberOfPoints() >= 1))
    {
        double pickedPoint[3];
        m_2DViewer->getEventWorldCoordinate(pickedPoint);
        m_2DViewer->putCoordinateInCurrentImageBounds(pickedPoint);

        if (!m_closingPolyline)
        {
            m_closingPolyline = new DrawerPolyline;
            // Així evitem que la primitiva pugui ser esborrada durant l'edició per events externs
            m_closingPolyline->increaseReferenceCount();
            m_closingPolyline->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);
            m_2DViewer->getDrawer()->draw(m_closingPolyline);

            // Afegim els punts que simulen aquesta polilínia
            m_closingPolyline->addPoint(m_mainPolyline->getPoint(0));
            m_closingPolyline->addPoint(pickedPoint);
            m_closingPolyline->addPoint(m_mainPolyline->getPoint(m_mainPolyline->getNumberOfPoints() - 1));
        }
        else
        {
            // Modifiquem els punts que han canviat
            m_closingPolyline->setPoint(1, pickedPoint);
            m_closingPolyline->setPoint(2, m_mainPolyline->getPoint(m_mainPolyline->getNumberOfPoints() - 1));
            // Actualitzem els atributs de la polilínia
            m_closingPolyline->update();
        }
        m_2DViewer->render();
    }
}

void PolylineROITool::closeForm()
{
    // Així alliberem les primitives perquè puguin ser esborrades
    m_closingPolyline->decreaseReferenceCount();
    m_mainPolyline->decreaseReferenceCount();
    m_roiPolygon->decreaseReferenceCount();
    // Eliminem les polilínies amb les que hem simulat el dibuix de la ROI
    delete m_closingPolyline;
    delete m_mainPolyline;

    equalizeDepth();

    // Dibuixem el polígon resultant
    m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Indiquem que hem finalitzat les tasques de dibuix
    printData();

    // Un cop fets els càlculs, fem el punter nul per poder controlar si podem fer una nova roi o no
    // No fem delete, perquè sinó això faria que s'esborrés del drawer
    m_roiPolygon = NULL;
}

void PolylineROITool::setTextPosition(DrawerText *text)
{
    double lastPoint[3];
    m_2DViewer->getEventWorldCoordinate(lastPoint);
    m_2DViewer->putCoordinateInCurrentImageBounds(lastPoint);

    QList<QVector<double> > pointsList = getBoundingBoxPoints();

    double closestPoint[3];
    int closestEdge;
    MathTools::getPointToClosestEdgeDistance(lastPoint, pointsList, true, closestPoint, closestEdge);

    const double Padding = 5.0;
    double paddingX = 0.0;
    double paddingY = 0.0;

    //     0
    //    ____
    // 3 |    | 1
    //   |____|
    //     2
    switch (closestEdge)
    {
        case 0:
            if (m_2DViewer->getView() == Q2DViewer::Axial)
            {
                paddingY = Padding;
                text->setVerticalJustification("Bottom");
            }
            else
            {
                paddingY = -Padding;
                text->setVerticalJustification("Top");
            }
            text->setHorizontalJustification("Centered");
            break;
        case 1:
            paddingX = Padding;
            text->setVerticalJustification("Centered");
            text->setHorizontalJustification("Left");
            break;
        case 2:
            if (m_2DViewer->getView() == Q2DViewer::Axial)
            {
                paddingY = -Padding;
                text->setVerticalJustification("Top");
            }
            else
            {
                paddingY = Padding;
                text->setVerticalJustification("Bottom");
            }
            text->setHorizontalJustification("Centered");
            break;
        case 3:
            paddingX = -Padding;
            text->setVerticalJustification("Centered");
            text->setHorizontalJustification("Right");
            break;
    }

    double closestPointInDisplay[3];
    // Passem closestPoint a coordenades de display
    m_2DViewer->computeWorldToDisplay(closestPoint[0], closestPoint[1], closestPoint[2], closestPointInDisplay);
    // Apliquem el padding i tornem a coordenades de món
    double temporalWorldPoint[4];
    m_2DViewer->computeDisplayToWorld(closestPointInDisplay[0] + paddingX, closestPointInDisplay[1] + paddingY, closestPointInDisplay[2], temporalWorldPoint);
    closestPoint[0] = temporalWorldPoint[0];
    closestPoint[1] = temporalWorldPoint[1];
    closestPoint[2] = temporalWorldPoint[2];

    text->setAttachmentPoint(closestPoint);
}

QList<QVector<double> > PolylineROITool::getBoundingBoxPoints()
{
    double bounds[6];
    m_roiPolygon->getBounds(bounds);

    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());

    QList<QVector<double> > pointsList;
    QVector<double> point(3);
    point[xIndex] = bounds[xIndex * 2];
    point[yIndex] = bounds[yIndex * 2];
    point[zIndex] = bounds[zIndex * 2];
    pointsList << point;
    point[xIndex] = bounds[xIndex * 2 + 1];
    point[yIndex] = bounds[yIndex * 2];
    point[zIndex] = bounds[zIndex * 2];
    pointsList << point;
    point[xIndex] = bounds[xIndex * 2 + 1];
    point[yIndex] = bounds[yIndex * 2 + 1];
    point[zIndex] = bounds[zIndex * 2];
    pointsList << point;
    point[xIndex] = bounds[xIndex * 2];
    point[yIndex] = bounds[yIndex * 2 + 1];
    point[zIndex] = bounds[zIndex * 2];
    pointsList << point;

    return pointsList;
}

void PolylineROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_mainPolyline.isNull())
    {
        m_mainPolyline->decreaseReferenceCount();
        delete m_mainPolyline;
    }

    if (!m_closingPolyline.isNull())
    {
        m_closingPolyline->decreaseReferenceCount();
        delete m_closingPolyline;
    }

    if (!m_roiPolygon.isNull())
    {
        delete m_roiPolygon;
    }

    m_closingPolyline = NULL;
    m_mainPolyline = NULL;
    m_roiPolygon = NULL;
}

void PolylineROITool::equalizeDepth()
{
    // Assignem a tots els punts la z de l'últim
    int zIndex = Q2DViewer::getZIndexForView(m_2DViewer->getView());
    int n = m_roiPolygon->getNumberOfPoints();
    double z = m_roiPolygon->getVertix(n - 1)[zIndex];
    for (int i = 0; i < n - 1; i++)
    {
        const double *constPoint = m_roiPolygon->getVertix(i);
        double point[3] = { constPoint[0], constPoint[1], constPoint[2] };
        point[zIndex] = z;
        m_roiPolygon->setVertix(i, point);
    }
    m_roiPolygon->update();
}

}
