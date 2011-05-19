#include "referencelinestool.h"
#include "referencelinestooldata.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "series.h"
#include "image.h"
#include "imageplane.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawerline.h"
#include "mathtools.h"
// Vtk
#include <vtkPlane.h>

namespace udg {

ReferenceLinesTool::ReferenceLinesTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_projectedReferencePlane(0), m_showPlaneThickness(true), m_planesToProject(SingleImage)
{
    m_toolName = "ReferenceLinesTool";
    m_hasSharedData = true;

    m_myData = new ReferenceLinesToolData;
    m_toolData = m_myData;
    connect(m_myData, SIGNAL(changed()), SLOT(updateProjectionLines()));

    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

// Descomentar aquestes 4 linies si es vol mostrar el poligon del pla projectat
//      m_projectedReferencePlane = new DrawerPolygon;
//     // HACK sucedani d'smart pointer(TM)
//     m_projectedReferencePlane->increaseReferenceCount();
//     m_2DViewer->getDrawer()->draw(m_projectedReferencePlane, QViewer::Top2DPlane);
//     m_2DViewer->getDrawer()->addToGroup(m_projectedReferencePlane, "ReferenceLines");

    refreshReferenceViewerData();

    // Cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(refreshReferenceViewerData()));
    // Cada cop que el viewer canvïi de llesca, hem d'actualitzar el pla de projecció
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateImagePlane()));
    // Cada cop que canvii l'slab thickness haurem d'actualitzar els plans a projectar
    connect(m_2DViewer, SIGNAL(slabThicknessChanged(int)), SLOT(updateImagePlane()));

    connect(m_2DViewer, SIGNAL(selected()), SLOT(refreshReferenceViewerData()));
}

ReferenceLinesTool::~ReferenceLinesTool()
{
// Descomentar aquestes 4 linies si es vol mostrar el poligon del pla projectat(nomes debug)
//     // Ja no som propietaris de les línies creades
//     m_projectedReferencePlane->decreaseReferenceCount();
//     // Ara al fer delete, s'esborraran automàticament del drawer, ja que nosaltres érem els únics propietaris
//     delete m_projectedReferencePlane;
    // Cal esborrar les línies que hàgim creat
    foreach (DrawerLine *line, m_projectedIntersectionLines)
    {
        // HACK Sucedani d'smart pointer(TM)
        line->decreaseReferenceCount();
        delete line;
    }
    foreach (DrawerLine *backgroundLine, m_backgroundProjectedIntersectionLines)
    {
        // HACK Sucedani d'smart pointer(TM)
        backgroundLine->decreaseReferenceCount();
        delete backgroundLine;
    }
}

void ReferenceLinesTool::setToolData(ToolData *data)
{
    // Desfem els vincles anteriors
    disconnect(m_myData, SIGNAL(changed()), this, SLOT(updateProjectionLines()));
    delete m_myData;

    // Creem de nou les dades
    m_toolData = data;
    m_myData = qobject_cast<ReferenceLinesToolData*>(data);
    // Quan canvïn les dades (ImagePlane), actualitzem les línies de projecció
    connect(m_myData, SIGNAL(changed()), SLOT(updateProjectionLines()));

    // Això serveix perquè s'apliqui tot just quan es creïi la tool
    // amb múltiples viewers
    if (m_2DViewer->isActive())
    {
        refreshReferenceViewerData();
    }
    updateProjectionLines();
}

void ReferenceLinesTool::updateProjectionLines()
{
    // En cas que no sigui el viewer que estem modificant i que tingui input
    if (!m_2DViewer->isActive() && m_2DViewer->getInput())
    {
        // Intentarem projectar el pla que hi ha a m_myData
        // Primer cal que comparteixin el mateix FrameOfReference
        if (m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID())
        {
            QList<ImagePlane*> planesToProject = m_myData->getPlanesToProject();
            // Primer comprovar si tenim el nombre adequat de linies creades, donat el nombre de plans a projectar
            checkAvailableLines();
            if (planesToProject.count() == 0)
            {
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
            }
            else
            {
                int drawerLineOffset = 0;
                foreach (ImagePlane *referencePlane, planesToProject)
                {
                    // Aquí ja ho deixem en mans de la projecció
                    ImagePlane *localizerPlane = m_2DViewer->getCurrentImagePlane();
                    projectIntersection(referencePlane, localizerPlane, drawerLineOffset);
                    delete localizerPlane;
                    drawerLineOffset += m_showPlaneThickness ? 2 : 1;
                }
            }
        }
        else
        {
            m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
        }
    }
    else
    {
        // TODO Això només hauria de ser necessari quan el viewer és marcat com actiu
        m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    }
}

void ReferenceLinesTool::projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane, int drawerLineOffset)
{
    if (!(referencePlane && localizerPlane))
    {
        return;
    }

    // Primer mirem que siguin plans diferents
    if (*localizerPlane != *referencePlane)
    {
        
        // Projecció de la intersecció dels plans
        /// Llegir http://fixunix.com/dicom/51195-scanogram-lines-mr.html

        // Recollim les dades del pla del localitzador sobre el qual volem projectar el de referència
        double localizerNormalVector[3], localizerOrigin[3];
        localizerPlane->getNormalVector(localizerNormalVector);
        localizerPlane->getOrigin(localizerOrigin);

        // TODO mirar exactament quan cal amagar les línies i quan no, depenent de les interseccions trobades
        // un cop tenim les interseccions nomes cal projectar-les i pintar la linia

        if (m_showPlaneThickness)
        {
            // Calculem totes les possibles interseccions
            QList<QVector<double> > upperPlaneBounds = referencePlane->getUpperBounds();
            double firstIntersectionPoint[3], secondIntersectionPoint[3];

            int numberOfIntersections = this->getIntersections(upperPlaneBounds.at(0), upperPlaneBounds.at(1), upperPlaneBounds.at(2), upperPlaneBounds.at(3),
                                                               localizerPlane, firstIntersectionPoint, secondIntersectionPoint);
            if (numberOfIntersections > 0)
            {
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage(firstIntersectionPoint, firstIntersectionPoint);
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage(secondIntersectionPoint, secondIntersectionPoint);

                // Linia discontinua
                m_projectedIntersectionLines[drawerLineOffset]->setFirstPoint(firstIntersectionPoint);
                m_projectedIntersectionLines[drawerLineOffset]->setSecondPoint(secondIntersectionPoint);
                // Linia de background
                m_backgroundProjectedIntersectionLines[drawerLineOffset]->setFirstPoint(firstIntersectionPoint);
                m_backgroundProjectedIntersectionLines[drawerLineOffset]->setSecondPoint(secondIntersectionPoint);

                m_2DViewer->getDrawer()->showGroup("ReferenceLines");
            }
            else
            {
                // TODO en comptes de fer un hide, posar valors 0,0 a cada coordenada perquè no afecti a altres línies que sí interecten?
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
            }

            QList<QVector<double> > lowerPlaneBounds = referencePlane->getLowerBounds();
            numberOfIntersections = this->getIntersections(lowerPlaneBounds.at(0), lowerPlaneBounds.at(1), lowerPlaneBounds.at(2), lowerPlaneBounds.at(3),
                                                           localizerPlane, firstIntersectionPoint, secondIntersectionPoint);

            // Un cop tenim les interseccions nomes cal projectar-les i pintar la linia
            if (numberOfIntersections > 0)
            {
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage(firstIntersectionPoint, firstIntersectionPoint);
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage(secondIntersectionPoint, secondIntersectionPoint);

                // Linia discontinua
                m_projectedIntersectionLines[drawerLineOffset + 1]->setFirstPoint(firstIntersectionPoint);
                m_projectedIntersectionLines[drawerLineOffset + 1]->setSecondPoint(secondIntersectionPoint);
                // Linia de background
                m_backgroundProjectedIntersectionLines[drawerLineOffset + 1]->setFirstPoint(firstIntersectionPoint);
                m_backgroundProjectedIntersectionLines[drawerLineOffset + 1]->setSecondPoint(secondIntersectionPoint);

                m_2DViewer->getDrawer()->showGroup("ReferenceLines");
            }
            else
            {
                // TODO en comptes de fer un hide, posar valors 0,0 a cada coordenada perquè no afecti a altres línies que sí interecten?
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
                // Si no hi ha cap intersecció apliquem el pla directament, "a veure què"
                // TODO això és per debug ONLY!!
    //             projectPlane(referencePlane);
            }
        }
        // Nomes agafem el pla "central"
        else
        {
            QList<QVector<double> > planeBounds = referencePlane->getCentralBounds();
            double firstIntersectionPoint[3], secondIntersectionPoint[3];

            int numberOfIntersections = this->getIntersections(planeBounds.at(0), planeBounds.at(1), planeBounds.at(2), planeBounds.at(3), localizerPlane,
                                                               firstIntersectionPoint, secondIntersectionPoint);
            if (numberOfIntersections > 0)
            {
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage(firstIntersectionPoint, firstIntersectionPoint);
                m_2DViewer->projectDICOMPointToCurrentDisplayedImage(secondIntersectionPoint, secondIntersectionPoint);

                // Linia discontinua
                m_projectedIntersectionLines[drawerLineOffset]->setFirstPoint(firstIntersectionPoint);
                m_projectedIntersectionLines[drawerLineOffset]->setSecondPoint(secondIntersectionPoint);
                // Linia de background
                m_backgroundProjectedIntersectionLines[drawerLineOffset]->setFirstPoint(firstIntersectionPoint);
                m_backgroundProjectedIntersectionLines[drawerLineOffset]->setSecondPoint(secondIntersectionPoint);

                m_2DViewer->getDrawer()->showGroup("ReferenceLines");
            }
            else
            {
                // TODO en comptes de fer un hide, posar valors 0,0 a cada coordenada perquè no afecti a altres línies que sí interecten?
                m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
            }
        }
    }
    else
    {
        m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    }
}

void ReferenceLinesTool::projectPlane(ImagePlane *planeToProject)
{
    QList<QVector<double> > planeBounds = planeToProject->getCentralBounds();
    double projectedVertix1[3], projectedVertix2[3], projectedVertix3[3], projectedVertix4[3];

    m_2DViewer->projectDICOMPointToCurrentDisplayedImage((double*)planeBounds.at(0).data(), projectedVertix1);
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage((double*)planeBounds.at(1).data(), projectedVertix2);
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage((double*)planeBounds.at(2).data(), projectedVertix3);
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage((double*)planeBounds.at(3).data(), projectedVertix4);

    // Donem els punts al poligon a dibuixar
    m_projectedReferencePlane->setVertix(0, projectedVertix1);
    m_projectedReferencePlane->setVertix(1, projectedVertix2);
    m_projectedReferencePlane->setVertix(2, projectedVertix3);
    m_projectedReferencePlane->setVertix(3, projectedVertix4);
    m_2DViewer->getDrawer()->showGroup("ReferenceLines");
}

int ReferenceLinesTool::getIntersections(QVector<double> tlhc, QVector<double> trhc, QVector<double> brhc, QVector<double> blhc, ImagePlane *localizerPlane,
                                         double firstIntersectionPoint[3], double secondIntersectionPoint[3])
{
    double t;
    int numberOfIntersections = 0;
    double localizerNormalVector[3], localizerOrigin[3];
    localizerPlane->getNormalVector(localizerNormalVector);
    localizerPlane->getOrigin(localizerOrigin);

    // Primera "paral·lela"
    if (vtkPlane::IntersectWithLine((double*)tlhc.data(), (double*)trhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint))
    {
        numberOfIntersections++;
    }
    if (vtkPlane::IntersectWithLine((double*)brhc.data(), (double*)blhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint))
    {
        numberOfIntersections++;
    }

    // Provar amb la segona "paral·lela"
    if (numberOfIntersections == 0)
    {
        if (vtkPlane::IntersectWithLine((double *)trhc.data(), (double *)brhc.data(), localizerNormalVector, localizerOrigin, t, firstIntersectionPoint))
        {
            numberOfIntersections++;
        }

        if (vtkPlane::IntersectWithLine((double*)blhc.data(), (double*)tlhc.data(), localizerNormalVector, localizerOrigin, t, secondIntersectionPoint))
        {
            numberOfIntersections++;
        }
    }

    return numberOfIntersections;
}

void ReferenceLinesTool::updateFrameOfReference()
{
    if (!m_2DViewer->getInput())
    {
        // No hi ha frame of reference que valgui
        m_myFrameOfReferenceUID.clear();
    }
    else
    {
        // TODO assumim que totes les imatges són de la mateixa sèrie i per això agafem la informació de la primera imatge
        // Ben fet (les imatges poden ser de diferents sèries), caldria obtenir el frame of reference de la imatge que s'està veient
        Image *image = m_2DViewer->getInput()->getImage(0);
        if (image)
        {
            // Ens guardem el nostre
            m_myFrameOfReferenceUID = image->getParentSeries()->getFrameOfReferenceUID();
        }
        else
        {
            // Sense sèries, no hi ha frame of reference
            m_myFrameOfReferenceUID.clear();
            DEBUG_LOG("EL nou volum no té series NUL!");
        }
    }
    // I actualitzem el de les dades
    m_myData->setFrameOfReferenceUID(m_myFrameOfReferenceUID);
}

void ReferenceLinesTool::updateImagePlane()
{
    if (!m_2DViewer->isActive())
    {
        return;
    }

    switch (m_planesToProject)
    {
        case SingleImage:
            if (m_2DViewer->isThickSlabActive())
            {
                QList<ImagePlane*> planes;
                planes << m_2DViewer->getCurrentImagePlane();
                planes << m_2DViewer->getImagePlane(m_2DViewer->getCurrentSlice() + m_2DViewer->getSlabThickness() - 1, m_2DViewer->getCurrentPhase());

                m_myData->setPlanesToProject(planes);
            }
            else
            {
                m_myData->setPlanesToProject(m_2DViewer->getCurrentImagePlane());
            }
            break;

        case AllImages:
            // TODO Implementar
            break;
    }
}

void ReferenceLinesTool::refreshReferenceViewerData()
{
    // Si es projectaven plans sobre el nostre drawer, els amaguem
    m_2DViewer->getDrawer()->hideGroup("ReferenceLines");
    updateFrameOfReference();
    updateImagePlane();
}

DrawerLine* ReferenceLinesTool::createNewLine(bool isBackgroundLine)
{
    DrawerLine *line = new DrawerLine;
    // HACK Sucedani d'smart pointer(TM)
    line->increaseReferenceCount();

    if (isBackgroundLine)
    {
        line->setColor(QColor(0, 0, 0));
    }
    else
    {
        line->setColor(QColor(255, 160, 0));
        line->setLinePattern(DrawerPrimitive::DiscontinuousLinePattern);
    }

    return line;
}

void ReferenceLinesTool::checkAvailableLines()
{
    int planesToProject = m_myData->getPlanesToProject().count();
    int neededLines = planesToProject;
    if (m_showPlaneThickness)
    {
        neededLines *= 2;
    }

    // Llistat de linies de background
    int numberOfLines = m_backgroundProjectedIntersectionLines.count();

    // Ara comprovem pels dos llistats que estiguin be
    if (neededLines > numberOfLines)
    {
        int linesToCreate = neededLines - numberOfLines;
        // Cal crear linies noves
        for (int i = 0; i < linesToCreate; i++)
        {
            DrawerLine *line = createNewLine(true);
            m_2DViewer->getDrawer()->draw(line, QViewer::Top2DPlane);
            m_2DViewer->getDrawer()->addToGroup(line, "ReferenceLines");
            m_backgroundProjectedIntersectionLines << line;
        }
    }
    else if (neededLines < numberOfLines)
    {
        int linesToRemove = numberOfLines - neededLines;
        // Tenim mes linies de les necessaries, cal esborrar
        for (int i = 0; i < linesToRemove; i++)
        {
            DrawerLine *line = m_backgroundProjectedIntersectionLines.takeLast();
            // Ja no som propietaris de les línies creades, HACK sucedani d'smart pointer(TM)
            line->decreaseReferenceCount();
            delete line;
        }
    }

    // Llistat de linies "principals"
    numberOfLines = m_projectedIntersectionLines.count();

    // Ara comprovem pels dos llistats que estiguin be
    if (neededLines > numberOfLines)
    {
        int linesToCreate = neededLines - numberOfLines;
        // Cal crear linies noves
        for (int i = 0; i < linesToCreate; i++)
        {
            DrawerLine *line = createNewLine();
            m_2DViewer->getDrawer()->draw(line, QViewer::Top2DPlane);
            m_2DViewer->getDrawer()->addToGroup(line, "ReferenceLines");
            m_projectedIntersectionLines << line;
        }
    }
    else if (neededLines < numberOfLines)
    {
        int linesToRemove = numberOfLines - neededLines;
        // Tenim mes linies de les necessaries, cal esborrar
        for (int i = 0; i < linesToRemove; i++)
        {
            DrawerLine *line = m_projectedIntersectionLines.takeLast();
            // Ja no som propietaris de les línies creades, HACK sucedani d'smart pointer(TM)
            line->decreaseReferenceCount();
            delete line;
        }
    }
}

}
