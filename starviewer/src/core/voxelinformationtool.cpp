/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#include "voxelinformationtool.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawertext.h"
#include "drawer.h"
#include "voxel.h"
#include "logging.h"
#include "standarduptakevaluemeasurehandler.h"
#include "sliceorientedvolumepixeldata.h"
// Vtk
#include <vtkCommand.h>

namespace udg {

VoxelInformationTool::VoxelInformationTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_caption(0)
{
    m_toolName = "VoxelInformationTool";

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    createCaption();
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(updateCaption()));
    connect(m_2DViewer, SIGNAL(phaseChanged(int)), SLOT(updateCaption()));
    connect(m_2DViewer, SIGNAL(cameraChanged()), SLOT(updateCaption()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(updateCaption()));
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
            m_2DViewer->restoreRenderingQuality();
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
    // Don't update the caption if there is no caption or if the mouse isn't over the viewer (#2014)
    if (!m_caption || !m_2DViewer->underMouse())
    {
        return;
    }

    // We create a list with the captions of each input of the viewer
    QStringList inputsCaptions;
    for (int i = 0; i < m_2DViewer->getNumberOfInputs(); ++i)
    {
        double xyz[3];
        if (m_2DViewer->getCurrentCursorImageCoordinateOnInput(xyz, i))
        {
            QString caption = computeVoxelValueOnInput(xyz, i);

            if (m_2DViewer->getNumberOfInputs() > 1)
            {
                caption += " (" + m_2DViewer->getInput(i)->getModality() + ")";
            }

            inputsCaptions << caption;
        }
    }

    // Update the DrawerText element according to the gathered information
    if (!inputsCaptions.isEmpty())
    {
        double attachmentPoint[3];
        QString horizontalJustification, verticalJustification;
        computeCaptionAttachmentPointAndTextAlignment(attachmentPoint, horizontalJustification, verticalJustification);

        // Actualitzem els valors del caption
        m_caption->visibilityOn();
        m_caption->setAttachmentPoint(attachmentPoint);
        m_caption->setHorizontalJustification(horizontalJustification);
        m_caption->setVerticalJustification(verticalJustification);
        m_caption->setText(inputsCaptions.join("\n"));
        m_caption->update();
    }
    else
    {
        m_caption->visibilityOff();
        m_caption->update();
    }
    m_2DViewer->render();
}

QString VoxelInformationTool::computeVoxelValueOnInput(double worldCoordinate[3], int i)
{
    SliceOrientedVolumePixelData pixelData = m_2DViewer->getCurrentPixelDataFromInput(i);
    Voxel voxel = pixelData.getVoxelValue(worldCoordinate);
    
    if (voxel.getNumberOfComponents() == 1 && m_2DViewer->getInput(i)->getModality() == "PT")
    {
        Image *petImage = m_2DViewer->getCurrentDisplayedImageOnInput(i);
        if (!petImage)
        {
            petImage = m_2DViewer->getInput(i)->getImage(0);
        }

        StandardUptakeValueMeasureHandler suvHandler;
        suvHandler.setImage(petImage);
        if (suvHandler.canComputePreferredFormula())
        {
            double value = suvHandler.computePreferredFormula(voxel.getComponent(0));
            return QString::number(value, 'f', 2) + " " + suvHandler.getComputedFormulaUnits() + " " + tr("SUV (%1)").arg(suvHandler.getComputedFormulaLabel());
        }
    }

    return voxel.getAsQString() + " " + m_2DViewer->getInput(i)->getPixelUnits();
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
    QSize viewportSize = m_2DViewer->getRenderWindowSize();
    QPoint cursorPosition = m_2DViewer->getEventPosition();

    // Aquestes seran les coordenades que ajustarem per col·locar el caption
    QPoint adjustedCursorPosition = cursorPosition;

    bool insideMargins = true;
    // Estem quasi a dalt de tot?
    if (cursorPosition.y() > viewportSize.height() - marginPixels)
    {
        adjustedCursorPosition.setY(viewportSize.height() - marginPixels);
        verticalJustification = "Bottom";
        insideMargins = false;
    }

    // Estem quasi abaix del tot?
    if (cursorPosition.y() < marginPixels)
    {
        adjustedCursorPosition.setY(marginPixels);
        verticalJustification = "Top";
        insideMargins = false;
    }

    // Estem a prop de la dreta?
    if (cursorPosition.x() > viewportSize.width() - marginPixels)
    {
        adjustedCursorPosition.setX(viewportSize.width() - marginPixels);
        horizontalJustification = "Right";
        insideMargins = false;
    }

    // Estem a prop de l'esquerra?
    if (cursorPosition.x() < marginPixels)
    {
        adjustedCursorPosition.setX(marginPixels);
        horizontalJustification = "Left";
        insideMargins = false;
    }

    if (insideMargins)
    {
        adjustedCursorPosition.rx() -= 5;
        adjustedCursorPosition.ry() += 5;
    }

    // I finalment transformem la coordenada de viewport en coordenada de món
    m_2DViewer->computeDisplayToWorld(adjustedCursorPosition.x(), adjustedCursorPosition.y(), 0.0, attachmentPoint);
}

}
