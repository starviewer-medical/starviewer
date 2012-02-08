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
            if (m_state == DrawingReferenceLine)
            {
                updateReferenceLineAndRender();
            }
            else if (m_state == DrawingFirstPerpendicularLine)
            {
                updateFirstPerpendicularLineAndRender();
            }
            else if (m_state == DrawingSecondPerpendicularLine)
            {
                updateSecondPerpendicularLineAndRender();
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
        case DrawingReferenceLine:
            addSecondPoint();
            break;
        case DrawingFirstPerpendicularLine:
            addThirdPoint();
            break;
        case DrawingSecondPerpendicularLine:
            addFourthPoint();
    }
}

void PerpendicularDistanceTool::addFirstPoint()
{
    Q_ASSERT(m_state == NotDrawing);

    if (!m_referenceLine)
    {
        m_referenceLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_referenceLine->increaseReferenceCount();
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);

    // Afegim el punt
    m_referenceLine->setFirstPoint(clickedWorldPoint);
    m_referenceLine->setSecondPoint(clickedWorldPoint);
    m_referenceLine->update();

    m_2DViewer->getDrawer()->draw(m_referenceLine);

    m_state = DrawingReferenceLine;
}

void PerpendicularDistanceTool::addSecondPoint()
{
    Q_ASSERT(m_state == DrawingReferenceLine);

    updateReferenceLine();

    if (!m_firstPerpendicularLine)
    {
        m_firstPerpendicularLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_firstPerpendicularLine->increaseReferenceCount();
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    m_firstPerpendicularLine->setFirstPoint(clickedWorldPoint);
    m_firstPerpendicularLine->setSecondPoint(clickedWorldPoint);
    m_firstPerpendicularLine->update();

    m_2DViewer->getDrawer()->draw(m_firstPerpendicularLine);

    m_state = DrawingFirstPerpendicularLine;
}

void PerpendicularDistanceTool::addThirdPoint()
{
    Q_ASSERT(m_state == DrawingFirstPerpendicularLine);

    updateFirstPerpendicularLine();

    if (!m_secondPerpendicularLine)
    {
        m_secondPerpendicularLine = new DrawerLine();
        // Així evitem que durant l'edició la primitiva pugui ser esborrada per esdeveniments externs
        m_secondPerpendicularLine->increaseReferenceCount();
    }

    double clickedWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(clickedWorldPoint);
    m_secondPerpendicularLine->setFirstPoint(clickedWorldPoint);
    m_secondPerpendicularLine->setSecondPoint(clickedWorldPoint);
    m_secondPerpendicularLine->update();

    m_2DViewer->getDrawer()->draw(m_secondPerpendicularLine);

    m_state = DrawingSecondPerpendicularLine;
}

void PerpendicularDistanceTool::addFourthPoint()
{
    Q_ASSERT(m_state == DrawingSecondPerpendicularLine);

    updateSecondPerpendicularLine();

    // Movem les línies al pla corresponent
    m_referenceLine->decreaseReferenceCount();
    m_firstPerpendicularLine->decreaseReferenceCount();
    m_secondPerpendicularLine->decreaseReferenceCount();
    m_2DViewer->getDrawer()->erasePrimitive(m_referenceLine);
    m_2DViewer->getDrawer()->erasePrimitive(m_firstPerpendicularLine);
    m_2DViewer->getDrawer()->erasePrimitive(m_secondPerpendicularLine);
    m_2DViewer->getDrawer()->draw(m_firstPerpendicularLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    m_2DViewer->getDrawer()->draw(m_secondPerpendicularLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    m_2DViewer->getDrawer()->draw(m_referenceLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());

    // Calculem i dibuxem la distància
    drawDistance();

    m_referenceLine = 0;
    m_firstPerpendicularLine = 0;
    m_secondPerpendicularLine = 0;

    m_state = NotDrawing;
}

void PerpendicularDistanceTool::updateReferenceLine()
{
    Q_ASSERT(m_state == DrawingReferenceLine);

    double mouseWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);
    m_referenceLine->setSecondPoint(mouseWorldPoint);
    m_referenceLine->update();
}

void PerpendicularDistanceTool::updateFirstPerpendicularLine()
{
    Q_ASSERT(m_state == DrawingFirstPerpendicularLine);

    equalizeDepth();

    // Obtenim la posició del ratolí
    double mouseWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);

    // Obtenim el punt de la línia de referència més proper a la posició del ratolí i també la distància
    // Serà el primer punt de la primera línia perpendicular
    double firstPerpendicularLineFirstPoint[3];
    double distance;
    distance = MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_referenceLine->getFirstPoint(), m_referenceLine->getSecondPoint(),
                                                       firstPerpendicularLineFirstPoint);

    // Calculem una línia perpendicular a la línia de referència que passi pel punt obtingut al pas anterior
    QVector3D referenceLineFirstPoint(m_referenceLine->getFirstPoint()[0], m_referenceLine->getFirstPoint()[1], m_referenceLine->getFirstPoint()[2]);
    QVector3D referenceLineSecondPoint(m_referenceLine->getSecondPoint()[0], m_referenceLine->getSecondPoint()[1], m_referenceLine->getSecondPoint()[2]);
    QVector3D referenceLineDirectorVector = MathTools::directorVector(referenceLineFirstPoint, referenceLineSecondPoint);
    double aReferenceLineDirectorVector[3] = { referenceLineDirectorVector.x(), referenceLineDirectorVector.y(), referenceLineDirectorVector.z() };
    int xIndex, yIndex, zIndex;
    Q2DViewer::getXYZIndexesForView(xIndex, yIndex, zIndex, m_2DViewer->getView());
    double aAuxiliarLineDirectorVector[3];
    aAuxiliarLineDirectorVector[xIndex] = aReferenceLineDirectorVector[yIndex];
    aAuxiliarLineDirectorVector[yIndex] = -aReferenceLineDirectorVector[xIndex];
    aAuxiliarLineDirectorVector[zIndex] = aReferenceLineDirectorVector[zIndex];
    double aAuxiliarLineFirstPoint[3] = { firstPerpendicularLineFirstPoint[0] + distance * aAuxiliarLineDirectorVector[0],
                                          firstPerpendicularLineFirstPoint[1] + distance * aAuxiliarLineDirectorVector[1],
                                          firstPerpendicularLineFirstPoint[2] + distance * aAuxiliarLineDirectorVector[2] };
    double aAuxiliarLineSecondPoint[3] = { firstPerpendicularLineFirstPoint[0] - distance * aAuxiliarLineDirectorVector[0],
                                           firstPerpendicularLineFirstPoint[1] - distance * aAuxiliarLineDirectorVector[1],
                                           firstPerpendicularLineFirstPoint[2] - distance * aAuxiliarLineDirectorVector[2] };

    // Obtenim el punt de la línia auxiliar més proper a la posició del ratolí
    // Serà el segon punt de la primera línia perpendicular
    double firstPerpendicularLineSecondPoint[3];
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, aAuxiliarLineFirstPoint, aAuxiliarLineSecondPoint, firstPerpendicularLineSecondPoint);

    // Assignem els punts calculats a la primera línia perpendicular
    m_firstPerpendicularLine->setFirstPoint(firstPerpendicularLineFirstPoint);
    m_firstPerpendicularLine->setSecondPoint(firstPerpendicularLineSecondPoint);
    m_firstPerpendicularLine->update();
}

void PerpendicularDistanceTool::updateSecondPerpendicularLine()
{
    Q_ASSERT(m_state == DrawingSecondPerpendicularLine);

    equalizeDepth();

    // Obtenim la posició del ratolí
    double mouseWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);

    // Obtenim el punt de la línia de referència més proper a la posició del ratolí
    // Serà el primer punt de la segona línia perpendicular
    double secondPerpendicularLineFirstPoint[3];
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_referenceLine->getFirstPoint(), m_referenceLine->getSecondPoint(),
                                            secondPerpendicularLineFirstPoint);

    // Calculem la longitud i el vector director de la primera línia perpendicular per fer que la segona sigui igual
    QVector3D firstPerpendicularLineFirstPoint(m_firstPerpendicularLine->getFirstPoint()[0],
                                               m_firstPerpendicularLine->getFirstPoint()[1],
                                               m_firstPerpendicularLine->getFirstPoint()[2]);
    QVector3D firstPerpendicularLineSecondPoint(m_firstPerpendicularLine->getSecondPoint()[0],
                                                m_firstPerpendicularLine->getSecondPoint()[1],
                                                m_firstPerpendicularLine->getSecondPoint()[2]);
    // No fem servir el mètode de MathTools perquè volem el vector que va del primer punt al segon, i MathTools no assegura que sigui sempre així.
    QVector3D firstPerpendicularLineDirectorVector = firstPerpendicularLineSecondPoint - firstPerpendicularLineFirstPoint;
    double length = firstPerpendicularLineDirectorVector.length();
    firstPerpendicularLineDirectorVector.normalize();

    // Calculem el segon punt de la segona línia perpendicular
    double secondPerpendicularLineSecondPoint[3] = { secondPerpendicularLineFirstPoint[0] + length * firstPerpendicularLineDirectorVector.x(),
                                                     secondPerpendicularLineFirstPoint[1] + length * firstPerpendicularLineDirectorVector.y(),
                                                     secondPerpendicularLineFirstPoint[2] + length * firstPerpendicularLineDirectorVector.z() };

    // Assignem els punts calculats a la segona línia perpendicular
    m_secondPerpendicularLine->setFirstPoint(secondPerpendicularLineFirstPoint);
    m_secondPerpendicularLine->setSecondPoint(secondPerpendicularLineSecondPoint);
    m_secondPerpendicularLine->update();
}

void PerpendicularDistanceTool::updateReferenceLineAndRender()
{
    updateReferenceLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::updateFirstPerpendicularLineAndRender()
{
    updateFirstPerpendicularLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::updateSecondPerpendicularLineAndRender()
{
    updateSecondPerpendicularLine();
    m_2DViewer->render();
}

void PerpendicularDistanceTool::drawDistance()
{
    drawDistanceLine();

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

void PerpendicularDistanceTool::drawDistanceLine()
{
    m_distanceLine = new DrawerLine();

    // Obtenim la posició del ratolí
    double mouseWorldPoint[3];
    m_2DViewer->getEventWorldCoordinate(mouseWorldPoint);

    // Obtenim el punt de la primera línia perpendicular més proper a la posició del ratolí
    // Serà el primer punt de la línia de distància
    double distanceLineFirstPoint[3];
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_firstPerpendicularLine->getFirstPoint(), m_firstPerpendicularLine->getSecondPoint(),
                                            distanceLineFirstPoint);

    // Obtenim el punt de la segona línia perpendicular més proper a la posició del ratolí
    // Serà el segon punt de la línia de distància
    double distanceLineSecondPoint[3];
    MathTools::getPointToFiniteLineDistance(mouseWorldPoint, m_secondPerpendicularLine->getFirstPoint(), m_secondPerpendicularLine->getSecondPoint(),
                                            distanceLineSecondPoint);

    // La línia de distància coincidirà amb la de referència; la posem a l'altre extrem de les línies perpendiculars
    if (distanceLineFirstPoint[0] == m_firstPerpendicularLine->getFirstPoint()[0]
        && distanceLineFirstPoint[1] == m_firstPerpendicularLine->getFirstPoint()[1]
        && distanceLineFirstPoint[2] == m_firstPerpendicularLine->getFirstPoint()[2])
    {
        distanceLineFirstPoint[0] = m_firstPerpendicularLine->getSecondPoint()[0];
        distanceLineFirstPoint[1] = m_firstPerpendicularLine->getSecondPoint()[1];
        distanceLineFirstPoint[2] = m_firstPerpendicularLine->getSecondPoint()[2];
        distanceLineSecondPoint[0] = m_secondPerpendicularLine->getSecondPoint()[0];
        distanceLineSecondPoint[1] = m_secondPerpendicularLine->getSecondPoint()[1];
        distanceLineSecondPoint[2] = m_secondPerpendicularLine->getSecondPoint()[2];
    }

    // Assignem els punts calculats a la línia de distància
    m_distanceLine->setFirstPoint(distanceLineFirstPoint);
    m_distanceLine->setSecondPoint(distanceLineSecondPoint);
    m_distanceLine->update();

    // Pintem la línia de distància
    m_2DViewer->getDrawer()->draw(m_distanceLine, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
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

void PerpendicularDistanceTool::equalizeDepth()
{
    // Ens quedem amb la z de la llesca actual
    double currentPoint[3];
    m_2DViewer->getEventWorldCoordinate(currentPoint);
    int zIndex = Q2DViewer::getZIndexForView(m_2DViewer->getView());
    double z = currentPoint[zIndex];
    double *point = m_referenceLine->getFirstPoint();
    point[zIndex] = z;
    m_referenceLine->setFirstPoint(point);
    point = m_referenceLine->getSecondPoint();
    point[zIndex] = z;
    m_referenceLine->setSecondPoint(point);
    m_referenceLine->update();
    point = m_firstPerpendicularLine->getFirstPoint();
    point[zIndex] = z;
    m_firstPerpendicularLine->setFirstPoint(point);
    point = m_firstPerpendicularLine->getSecondPoint();
    point[zIndex] = z;
    m_firstPerpendicularLine->setSecondPoint(point);
    m_firstPerpendicularLine->update();
    if (m_secondPerpendicularLine)
    {
        point = m_secondPerpendicularLine->getFirstPoint();
        point[zIndex] = z;
        m_secondPerpendicularLine->setFirstPoint(point);
        point = m_secondPerpendicularLine->getSecondPoint();
        point[zIndex] = z;
        m_secondPerpendicularLine->setSecondPoint(point);
        m_secondPerpendicularLine->update();
    }
}

void PerpendicularDistanceTool::reset()
{
    bool hasToRender = m_2DViewer;

    if (m_referenceLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_referenceLine->decreaseReferenceCount();
        delete m_referenceLine;
        hasToRender &= true;
    }

    if (m_firstPerpendicularLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_firstPerpendicularLine->decreaseReferenceCount();
        delete m_firstPerpendicularLine;
        hasToRender &= true;
    }

    if (m_secondPerpendicularLine)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_secondPerpendicularLine->decreaseReferenceCount();
        delete m_secondPerpendicularLine;
        hasToRender &= true;
    }

    if (hasToRender)
    {
        m_2DViewer->render();
    }

    m_state = NotDrawing;
}

}
