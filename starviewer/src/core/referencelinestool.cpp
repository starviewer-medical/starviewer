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

const QString ReferenceLinesTool::ReferenceLinesDrawerGroup("ReferenceLines");

const double ReferenceLinesTool::MinimumAngleConstraint = 45.0;
const double ReferenceLinesTool::MaximumAngleConstraint = 135.0;

ReferenceLinesTool::ReferenceLinesTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_projectedReferencePlane(0), m_showPlaneThickness(true), m_planesToProject(SingleImage)
{
    m_toolName = "ReferenceLinesTool";
    m_hasSharedData = true;

    m_myData = new ReferenceLinesToolData;
    m_toolData = m_myData;
    connect(m_myData, SIGNAL(changed()), SLOT(updateProjectionLines()));

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);

// Descomentar aquestes 4 linies si es vol mostrar el poligon del pla projectat
//      m_projectedReferencePlane = new DrawerPolygon;
//     // HACK sucedani d'smart pointer(TM)
//     m_projectedReferencePlane->increaseReferenceCount();
//     m_2DViewer->getDrawer()->draw(m_projectedReferencePlane);
//     m_2DViewer->getDrawer()->addToGroup(m_projectedReferencePlane, ReferenceLinesDrawerGroup);

    initialize();

    // Cada cop que el viewer canvïi d'input, hem d'actualitzar el frame of reference
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(updateDataForCurrentInput()));
    // Cada cop que el viewer canvïi de llesca, hem d'actualitzar el pla de projecció
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateReferenceImagePlanesToProject()));
    // Cada cop que canvii l'slab thickness haurem d'actualitzar els plans a projectar
    connect(m_2DViewer, SIGNAL(slabThicknessChanged(int)), SLOT(updateReferenceImagePlanesToProject()));

    connect(m_2DViewer, SIGNAL(selected()), SLOT(setAsReferenceViewer()));
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

    if (m_2DViewer->isActive())
    {
        m_myData->setFrameOfReferenceUID(QString());
    }
}

void ReferenceLinesTool::initialize()
{
    updateFrameOfReference();
    if (m_2DViewer->isActive())
    {
        m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
        updateReferenceImagePlanesToProject();
    }
    else
    {
        updateProjectionLines();
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
    initialize();
}

void ReferenceLinesTool::updateProjectionLines()
{
    // En cas que no sigui el viewer que estem modificant i que tingui input
    if (!m_2DViewer->isActive() && m_2DViewer->hasInput())
    {
        // Intentarem projectar el pla que hi ha a m_myData
        // Primer cal que comparteixin el mateix FrameOfReference
        if (m_myFrameOfReferenceUID == m_myData->getFrameOfReferenceUID() && !m_myFrameOfReferenceUID.isEmpty())
        {
            QList<ImagePlane*> planesToProject = m_myData->getPlanesToProject();
            // Primer comprovar si tenim el nombre adequat de linies creades, donat el nombre de plans a projectar
            checkAvailableLines();
            if (planesToProject.count() == 0)
            {
                m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
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
            m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
        }
    }
    else
    {
        // TODO Això només hauria de ser necessari quan el viewer és marcat com actiu
        m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
    }
}

void ReferenceLinesTool::projectIntersection(ImagePlane *referencePlane, ImagePlane *localizerPlane, int drawerLineOffset)
{
    if (!(referencePlane && localizerPlane))
    {
        return;
    }

    // Si l'angle entre plans no està dins de l'establert no s'aplicaran reference lines
    if (!meetAngleConstraint(localizerPlane, referencePlane))
    {
        m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
        return;
    }
    
    // Primer mirem que siguin plans diferents
    if (*localizerPlane != *referencePlane)
    {

        // Projecció de la intersecció dels plans
        /// Llegir http://fixunix.com/dicom/51195-scanogram-lines-mr.html

        QList<int> boundsList;
        if (m_showPlaneThickness)
        {
            // Cal que intersectin els dos plans del gruix perquè es mostrin les línies
            boundsList << 0 << 1;
        }
        else
        {
            // Nomes agafem el pla "central"
            boundsList << 2;
        }

        if (computeIntersectionAndUpdateProjectionLines(localizerPlane, referencePlane, boundsList, drawerLineOffset))
        {
            m_2DViewer->getDrawer()->enableGroup(ReferenceLinesDrawerGroup);
        }
        else
        {
            m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
        }
    }
    else
    {
        m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
    }
}

bool ReferenceLinesTool::computeIntersectionAndUpdateProjectionLines(ImagePlane *localizerPlane, ImagePlane *referencePlane, QList<int> boundsList, int lineOffset)
{
    bool hasEnoughIntersections = true;
    int numberOfIntersections = 0;
    for (int i = 0; i < boundsList.size(); ++i)
    {
        double firstIntersectionPoint[3], secondIntersectionPoint[3];
        numberOfIntersections = referencePlane->getIntersections(localizerPlane, firstIntersectionPoint, secondIntersectionPoint, boundsList.at(i));
        if (numberOfIntersections)
        {
            updateProjectionLinesFromIntersections(firstIntersectionPoint, secondIntersectionPoint, lineOffset + i);
            hasEnoughIntersections = hasEnoughIntersections && true;
        }
        else
        {
            resetProjectedLine(lineOffset + i);
            hasEnoughIntersections = hasEnoughIntersections && false;
        }
    }

    return hasEnoughIntersections;
}

void ReferenceLinesTool::updateProjectionLinesFromIntersections(double firstIntersectionPoint[3], double secondIntersectionPoint[3], int lineOffset)
{
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage(firstIntersectionPoint, firstIntersectionPoint);
    m_2DViewer->projectDICOMPointToCurrentDisplayedImage(secondIntersectionPoint, secondIntersectionPoint);

    updateProjectedLine(lineOffset, firstIntersectionPoint, secondIntersectionPoint);
}

void ReferenceLinesTool::updateProjectedLine(int lineOffset, double firstPoint[3], double secondPoint[3])
{
    // Dashed line
    m_projectedIntersectionLines[lineOffset]->setFirstPoint(firstPoint);
    m_projectedIntersectionLines[lineOffset]->setSecondPoint(secondPoint);
    // Background line
    m_backgroundProjectedIntersectionLines[lineOffset]->setFirstPoint(firstPoint);
    m_backgroundProjectedIntersectionLines[lineOffset]->setSecondPoint(secondPoint);
}

void ReferenceLinesTool::resetProjectedLine(int lineOffset)
{
    double voidPoint[3] = { 0.0, 0.0, 0.0 };

    updateProjectedLine(lineOffset, voidPoint, voidPoint);
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
    m_2DViewer->getDrawer()->enableGroup(ReferenceLinesDrawerGroup);
}

void ReferenceLinesTool::updateFrameOfReference()
{
    if (!m_2DViewer->hasInput())
    {
        // No hi ha frame of reference que valgui
        m_myFrameOfReferenceUID.clear();
    }
    else
    {
        // TODO assumim que totes les imatges són de la mateixa sèrie i per això agafem la informació de la primera imatge
        // Ben fet (les imatges poden ser de diferents sèries), caldria obtenir el frame of reference de la imatge que s'està veient
        Image *image = m_2DViewer->getMainInput()->getImage(0);
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
    // I actualitzem el de les dades en cas que sigui un visor actiu
    if (m_2DViewer->isActive())
    {
        m_myData->setFrameOfReferenceUID(m_myFrameOfReferenceUID);
    }
}

void ReferenceLinesTool::updateReferenceImagePlanesToProject()
{
    if (!m_2DViewer->isActive())
    {
        updateProjectionLines();
        return;
    }

    switch (m_planesToProject)
    {
        case SingleImage:
            if (m_2DViewer->isThickSlabActive())
            {
                QList<ImagePlane*> planes;
                planes << m_2DViewer->getCurrentImagePlane();
                Volume *currentInput = m_2DViewer->getMainInput();
                if (currentInput)
                {
                    planes << currentInput->getImagePlane(m_2DViewer->getCurrentSlice() + m_2DViewer->getSlabThickness() - 1, m_2DViewer->getView());
                }

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

void ReferenceLinesTool::updateDataForCurrentInput()
{
    // Actualitzem el frame of reference
    updateFrameOfReference();
    
    // Si sóc el visor actiu
    // - actualitzo el frame of reference de les dades compartides (implícit a updateFrameOfReference();
    // - no cal actualitzar el pla a projectar, ja es farà al respondre al signal sliceChanged()->updateReferenceImagePlanesToProject()
    // - no cal amagar el grup de primitives, al ser l'actiu ja s'ha hagut de fer anteriorment

    // Si sóc el visor inactiu
    // - recalculo la intersecció del pla de referència amb el meu pla actual
    if (!m_2DViewer->isActive())
    {
        updateProjectionLines();
    }
    else
    {
        setAsReferenceViewer();
    }
}

void ReferenceLinesTool::setAsReferenceViewer()
{
    if (!m_2DViewer->isActive())
    {
        return;
    }
    
    // Aquest visor no mostra cap projeccció, s'actualitza el pla de referència i quins plans cal projectar
    m_2DViewer->getDrawer()->disableGroup(ReferenceLinesDrawerGroup);
    updateFrameOfReference();
    updateReferenceImagePlanesToProject();
}

DrawerLine* ReferenceLinesTool::createNewLine(bool isBackgroundLine)
{
    DrawerLine *line = new DrawerLine;
    // HACK Sucedani d'smart pointer(TM)
    line->increaseReferenceCount();
    line->setVisibility(false);

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

    createAndRemoveLines(neededLines, m_backgroundProjectedIntersectionLines, true);
    createAndRemoveLines(neededLines, m_projectedIntersectionLines, false);
}

void ReferenceLinesTool::createAndRemoveLines(int neededLines, QList<DrawerLine*> &linesList, bool areBackgroundLines)
{
    int numberOfLines = linesList.count();

    // Comprovem que el nombre sigui el que necessitem
    if (neededLines > numberOfLines)
    {
        int linesToCreate = neededLines - numberOfLines;
        // Cal crear linies noves
        for (int i = 0; i < linesToCreate; i++)
        {
            DrawerLine *line = createNewLine(areBackgroundLines);
            m_2DViewer->getDrawer()->draw(line);
            m_2DViewer->getDrawer()->addToGroup(line, ReferenceLinesDrawerGroup);
            linesList << line;
        }
    }
    else if (neededLines < numberOfLines)
    {
        int linesToRemove = numberOfLines - neededLines;
        // Tenim mes linies de les necessaries, cal esborrar
        for (int i = 0; i < linesToRemove; i++)
        {
            DrawerLine *line = linesList.takeLast();
            // Ja no som propietaris de les línies creades, HACK sucedani d'smart pointer(TM)
            line->decreaseReferenceCount();
            delete line;
        }
    }
}

bool ReferenceLinesTool::meetAngleConstraint(ImagePlane *firstPlane, ImagePlane *secondPlane)
{
    if (!firstPlane || !secondPlane)
    {
        return false;
    }

    // Comprovem l'angle que formen els plans
    double angle = MathTools::angleInDegrees(firstPlane->getImageOrientation().getNormalVector(), secondPlane->getImageOrientation().getNormalVector());
    
    if (fabs(angle) < MinimumAngleConstraint || fabs(angle) > MaximumAngleConstraint)
    {
        return false;
    }
    else
    {
        return true;
    }

}

}
