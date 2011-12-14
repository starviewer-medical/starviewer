#include "voxelinformationtool.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawertext.h"
#include "drawer.h"
#include "logging.h"
// Vtk
#include <vtkCommand.h>

namespace udg {

VoxelInformationTool::VoxelInformationTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_caption(0)
{
    m_toolName = "VoxelInformationTool";

    m_2DViewer = qobject_cast<Q2DViewer*>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG("No s'ha pogut realitzar el casting a 2DViewer!!!");
    }
    createCaption();
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateCaption()));
    connect(m_2DViewer, SIGNAL(phaseChanged(int)), SLOT(updateCaption()));
    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(createCaption()));
}

VoxelInformationTool::~VoxelInformationTool()
{
    if (m_caption)
    {
        // Així alliberem la primitiva perquè pugui ser esborrada
        m_caption->decreaseReferenceCount();
        delete m_caption;
    }
}

void VoxelInformationTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::MouseMoveEvent:
            updateCaption();
            break;

        case vtkCommand::EnterEvent:
            break;

        case vtkCommand::LeaveEvent:
            m_caption->visibilityOff();
            m_caption->update();
            m_2DViewer->render();
            break;

        default:
            break;
    }
}

void VoxelInformationTool::createCaption()
{
    if (!m_caption)
    {
        m_caption = new DrawerText;
        // Així evitem que durant l'ús de l'eina la primitiva pugui ser esborrada per events externs
        m_caption->increaseReferenceCount();
        // Inicialment serà invisible
        m_caption->visibilityOff();
        m_2DViewer->getDrawer()->draw(m_caption);
    }
}

void VoxelInformationTool::updateCaption()
{
    if (!m_caption)
    {
        return;
    }

    double xyz[3];
    if (m_2DViewer->getCurrentCursorImageCoordinate(xyz))
    {
        double attachmentPoint[3];
        QString horizontalJustification, verticalJustification;
        computeCaptionAttachmentPointAndTextAlignment(attachmentPoint, horizontalJustification, verticalJustification);

        // Actualitzem els valors del caption
        m_caption->visibilityOn();
        m_caption->setAttachmentPoint(attachmentPoint);
        m_caption->setHorizontalJustification(horizontalJustification);
        m_caption->setVerticalJustification(verticalJustification);
        m_caption->setText(computeVoxelValue(xyz));
        m_caption->update();
    }
    else
    {
        m_caption->visibilityOff();
        m_caption->update();
    }
    m_2DViewer->render();
}

QString VoxelInformationTool::computeVoxelValue(double worldCoordinate[3])
{
    QString valueString;

    QVector<double> voxelValue;
    if (m_2DViewer->getInput()->getVoxelValue(worldCoordinate, voxelValue))
    {
        if (voxelValue.size() == 1)
        {
            valueString = QString("%1").arg(voxelValue.at(0));
        }
        else
        {
            valueString = QString("(%1").arg(voxelValue.at(0));
            for (int i = 1; i < voxelValue.size(); i++)
            {
                valueString += QString(", %1").arg(voxelValue.at(i));
            }
            valueString += ")";
        }
    }
    else
    {
        // Això no hauria de passar mai ja que se suposa que worldCoordinate
        // és una coordenada vàlida dins del Volume actual
        valueString = tr("N/A");
        DEBUG_LOG("No s'ha trobat valor de la imatge");
    }

    return valueString;
}

void VoxelInformationTool::computeCaptionAttachmentPointAndTextAlignment(double attachmentPoint[3], QString &horizontalJustification,
                                                                         QString &verticalJustification)
{
    // Per defecte alinearem el texte a la dreta i el més amunt possible
    horizontalJustification = "Right";
    verticalJustification = "Top";
    // Amb aquest valor definim el marge fins on considerem estar prou a prop d'alguna de les cantonades del visor
    int marginPixels = 50;
    // Calculem les mides del viewport per saber on tenim col·locat el cursor
    int *viewportSize = m_2DViewer->getRenderWindowSize();
    QPoint cursorPosition = m_2DViewer->getEventPosition();

    // Aquestes seran les coordenades que ajustarem per col·locar el caption
    double adjustedXCursorPosition = cursorPosition.x();
    double adjustedYCursorPosition = cursorPosition.y();

    bool insideMargins = true;
    // Estem quasi a dalt de tot?
    if (cursorPosition.y() > viewportSize[1] - marginPixels)
    {
        adjustedYCursorPosition = viewportSize[1] - marginPixels;
        verticalJustification = "Bottom";
        insideMargins = false;
    }

    // Estem quasi abaix del tot?
    if (cursorPosition.y() < marginPixels)
    {
        adjustedYCursorPosition = marginPixels;
        verticalJustification = "Top";
        insideMargins = false;
    }

    // Estem a prop de la dreta?
    if (cursorPosition.x() > viewportSize[0] - marginPixels)
    {
        adjustedXCursorPosition = viewportSize[0] - marginPixels;
        horizontalJustification = "Right";
        insideMargins = false;
    }

    // Estem a prop de l'esquerra?
    if (cursorPosition.x() < marginPixels)
    {
        adjustedXCursorPosition = marginPixels;
        horizontalJustification = "Left";
        insideMargins = false;
    }

    if (insideMargins)
    {
        adjustedXCursorPosition -= 5;
        adjustedYCursorPosition += 5;
    }

    // I finalment transformem la coordenada de viewport en coordenada de món
    double dummy[4];
    m_2DViewer->computeDisplayToWorld(adjustedXCursorPosition, adjustedYCursorPosition, 0.0, dummy);
    attachmentPoint[0] = dummy[0];
    attachmentPoint[1] = dummy[1];
    attachmentPoint[2] = dummy[2];
}

}
