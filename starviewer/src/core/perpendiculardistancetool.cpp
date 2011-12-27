#include "perpendiculardistancetool.h"

#include "drawer.h"
#include "drawerline.h"
#include "drawertext.h"
#include "image.h"
#include "logging.h"
#include "mathtools.h"
#include "series.h"
#include "q2dviewer.h"
#include "volume.h"

#include <QVector3D>

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

PerpendicularDistanceTool::PerpendicularDistanceTool(QViewer *viewer, QObject *parent)
    : Tool(viewer, parent)
{
    m_toolName = "PerpendicularDistanceTool";

    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));

    reset();
}

PerpendicularDistanceTool::~PerpendicularDistanceTool()
{
    reset();
}

void PerpendicularDistanceTool::handleEvent(unsigned long eventId)
{
    if (!m_2DViewer || !m_2DViewer->getInput())
    {
        return;
    }

    switch (eventId)
    {
        case vtkCommand::LeftButtonPressEvent:
            // Si hi ha doble clic només tenim en compte el primer
            if (m_2DViewer->getInteractor()->GetRepeatCount() == 0)
            {
                handleClick();
            }
            break;
        case vtkCommand::MouseMoveEvent:
            if (m_state == DrawingFirstLine)
            {
                updateFirstLineAndRender();
            }
            else if (m_state == DrawingDistanceLine)
            {
                updateDistanceLineAndRender();
            }
            break;
        case vtkCommand::KeyPressEvent:
            int keyCode = m_2DViewer->getInteractor()->GetKeyCode();
            if (keyCode == 27 && m_state != NotDrawing) // Esc
            {
                abortDrawing();
            }
            break;
    }
}

void PerpendicularDistanceTool::handleClick()
{
    switch (m_state)
    {
        case NotDrawing:
            addFirstPoint();
            break;
        case DrawingFirstLine:
            addSecondPoint();
            break;
        case DrawingDistanceLine:
            addThirdPoint();
            break;
    }
}

void PerpendicularDistanceTool::addFirstPoint()
{
    Q_ASSERT(m_state == NotDrawing);

    if (!m_firstLine)
    {
        m_firstLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_firstLine->increaseReferenceCount();
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    // Afegim el punt
    m_firstLine->setFirstPoint(clickedWorldPoint);
    m_firstLine->setSecondPoint(clickedWorldPoint);
    m_firstLine->update();

    m_2DViewer->getDrawer()->draw(m_firstLine);

    m_state = DrawingFirstLine;
}

void PerpendicularDistanceTool::addSecondPoint()
{
    Q_ASSERT(m_state == DrawingFirstLine);

    updateFirstLine();

    if (!m_distanceLine)
    {
        m_distanceLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_distanceLine->increaseReferenceCount();
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    m_distanceLine->setFirstPoint(clickedWorldPoint);
    m_distanceLine->setSecondPoint(clickedWorldPoint);
    m_distanceLine->update();

    m_2DViewer->getDrawer()->draw(m_distanceLine);

    m_state = DrawingDistanceLine;
}

void PerpendicularDistanceTool::addThirdPoint()
{
    Q_ASSERT(m_state == DrawingDistanceLine);

    updateDistanceLine();

    // Movem les línies al pla corresponent
    m_firstLine->decreaseReferenceCount();
    m_distanceLine->decreaseReferenceCount();
    m_2DViewer->getDrawer()->erasePrimitive(m_firstLine);
    m_2DViewer->getDrawer()->erasePrimitive(m_distanceLine);
    m_2DViewer->getDrawer()->draw(m_distanceLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    m_2DViewer->getDrawer()->draw(m_firstLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Calculem i dibuxem la distància
    drawDistance();

    m_firstLine = 0;
    m_distanceLine = 0;

    m_state = NotDrawing;
}

void PerpendicularDistanceTool::updateFirstLine()
{
    Q_ASSERT(m_state == DrawingFirstLine);

    double mouseWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);
    m_firstLine->setSecondPoint(mouseWorldPoint);
    m_firstLine->update();
}

void PerpendicularDistanceTool::updateDistanceLine()
{
    Q_ASSERT(m_state == DrawingDistanceLine);

    // Obtenim la posició del ratolí
    double mouseWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);

    // Obtenim el punt de la primera línia més proper a la posició del ratolí i també la distància
    // Serà el primer punt de la línia de distància
    double distanceLineFirstPoint[3];
    double distance;
    distance = MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_firstLine->getFirstPoint(), m_firstLine->getSecondPoint(), distanceLineFirstPoint);

    // Calculem una línia perpendicular a la primera línia que passi pel punt obtingut al pas anterior
    QVector3D firstLineFirstPoint(m_firstLine->getFirstPoint()[0], m_firstLine->getFirstPoint()[1], m_firstLine->getFirstPoint()[2]);
    QVector3D firstLineSecondPoint(m_firstLine->getSecondPoint()[0], m_firstLine->getSecondPoint()[1], m_firstLine->getSecondPoint()[2]);
    QVector3D firstLineDirectorVector = MathTools::directorVector(firstLineFirstPoint, firstLineSecondPoint);
    double aFirstLineDirectorVector[3] = { firstLineDirectorVector.x(), firstLineDirectorVector.y(), firstLineDirectorVector.z() };
    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
    double aAuxiliarLineDirectorVector[3];
    aAuxiliarLineDirectorVector[xIndex] = aFirstLineDirectorVector[yIndex];
    aAuxiliarLineDirectorVector[yIndex] = -aFirstLineDirectorVector[xIndex];
    aAuxiliarLineDirectorVector[zIndex] = aFirstLineDirectorVector[zIndex];
    double aAuxiliarLineFirstPoint[3] = { distanceLineFirstPoint[0] + distance * aAuxiliarLineDirectorVector[0],
                                          distanceLineFirstPoint[1] + distance * aAuxiliarLineDirectorVector[1],
                                          distanceLineFirstPoint[2] + distance * aAuxiliarLineDirectorVector[2] };
    double aAuxiliarLineSecondPoint[3] = { distanceLineFirstPoint[0] - distance * aAuxiliarLineDirectorVector[0],
                                           distanceLineFirstPoint[1] - distance * aAuxiliarLineDirectorVector[1],
                                           distanceLineFirstPoint[2] - distance * aAuxiliarLineDirectorVector[2] };

    // Obtenim el punt de la línia auxiliar més proper a la posició del ratolí
    // Serà el segon punt de la línia de distància
    double distanceLineSecondPoint[3];
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, aAuxiliarLineFirstPoint, aAuxiliarLineSecondPoint, distanceLineSecondPoint);

    // Assignem els punts calculats a la línia de distància
    m_distanceLine->setFirstPoint(distanceLineFirstPoint);
    m_distanceLine->setSecondPoint(distanceLineSecondPoint);
    m_distanceLine->update();
}

void PerpendicularDistanceTool::updateFirstLineAndRender()
{
    updateFirstLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::updateDistanceLineAndRender()
{
    updateDistanceLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::drawDistance() const
{
    DrawerText *text = new DrawerText();
    text->setText(getDistanceText());

    // Col·loquem el text a l'esquerra o a la dreta del segon punt segons la forma de la línia i apliquem un padding de 5 píxels
    const double Padding = 5.0;
    double textPadding;
    int xIndex = Q2DViewer::getXIndexForView(m_2DViewer->getView());
    double *firstPoint = m_distanceLine->getFirstPoint();
    double *secondPoint = m_distanceLine->getSecondPoint();

    if (firstPoint[xIndex] <= secondPoint[xIndex])
    {
        textPadding = Padding;
        text->setHorizontalJustification("Left");
    }
    else
    {
        textPadding = -Padding;
        text->setHorizontalJustification("Right");
    }

    double secondPointInDisplay[3];
    // Passem secondPoint a coordenades de display
    m_2DViewer->computeWorldToDisplay(secondPoint[0], secondPoint[1], secondPoint[2], secondPointInDisplay);
    // Apliquem el padding i tornem a coordenades de món
    double attachmentPoint[4];
    m_2DViewer->computeDisplayToWorld(secondPointInDisplay[0] + textPadding, secondPointInDisplay[1], secondPointInDisplay[2], attachmentPoint);

    text->setAttachmentPoint(attachmentPoint);

    m_2DViewer->getDrawer()->draw(text, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
}

QString PerpendicularDistanceTool::getDistanceText() const
{
    // HACK Comprovem si l'imatge té pixel spacing per saber si la mesura ha d'anar en píxels o mm
    // TODO Proporcionar algun mètode alternatiu per no haver d'haver de fer aquest hack
    double *vtkSpacing = m_2DViewer->getInput()->getSpacing();
    const double *pixelSpacing = m_2DViewer->getInput()->getImage(0)->getPixelSpacing();

    if (pixelSpacing[0] == 0.0 && pixelSpacing[1] == 0.0)
    {
        return QString(tr("%1 px")).arg(m_distanceLine->computeDistance(vtkSpacing), 0, 'f', 0);
    }
    else
    {
        // En cas de Ultrasons es fa un tractament especial perquè VTK no agafa l'spacing correcte.
        // TODO S'hauria d'unificar.
        // Podem tenir imatges de la mateixa sèrie amb spacings diferents
        if (m_2DViewer->getInput()->getImage(0)->getParentSeries()->getModality() == "US")
        {
            Image *image = m_2DViewer->getCurrentDisplayedImage();
            if (image)
            {
                const double *usSpacing = image->getPixelSpacing();
                double *firstPoint = m_distanceLine->getFirstPoint();
                double *secondPoint = m_distanceLine->getSecondPoint();
                double xx = (firstPoint[0] - secondPoint[0]) / vtkSpacing[0] * usSpacing[0];
                double yy = (firstPoint[1] - secondPoint[1]) / vtkSpacing[1] * usSpacing[1];
                double value = std::pow(xx, 2) + std::pow(yy, 2);
                return QString(tr("%1 mm")).arg(std::sqrt(value), 0, 'f', 2);
            }
        }

        // S'ha aplicat una reconstrucció, per tant l'spacing que es donarà serà el de vtk
        // TODO Això en algun moment desapareixerà ja que caldria deshabilitar les reconstruccions per
        // modalitats en les que les reconstruccions no tinguin sentit
        return QString(tr("%1 mm")).arg(m_distanceLine->computeDistance(), 0, 'f', 2);
    }
}

void PerpendicularDistanceTool::abortDrawing()
{
    Q_ASSERT(m_state != NotDrawing);

    reset();
}

void PerpendicularDistanceTool::reset()
{
    bool hasToRender = m_2DViewer;

    if (m_firstLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_firstLine->decreaseReferenceCount();
        delete m_firstLine;
        hasToRender &= true;
    }

    if (m_distanceLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_distanceLine->decreaseReferenceCount();
        delete m_distanceLine;
        hasToRender &= true;
    }

    if (hasToRender)
    {
        m_2DViewer->render();
    }

    m_state = NotDrawing;
}

}
