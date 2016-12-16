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

#include "magicroitool.h"
#include "logging.h"
#include "q2dviewer.h"
#include "volume.h"
#include "drawer.h"
#include "drawerpolygon.h"
#include "drawertext.h"
#include "mathtools.h"
#include "sliceorientedvolumepixeldata.h"
#include "voxel.h"
#include "voxelindex.h"

#include <QApplication> // to check pressed mouse buttons
#include <qmath.h>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

const int MagicROITool::MagicSize = 3;
const double MagicROITool::InitialMagicFactor = 0.0;

MagicROITool::MagicROITool(QViewer *viewer, QObject *parent)
: ROITool(viewer, parent)
{
    m_magicFactor = InitialMagicFactor;
    m_minX = 0;
    m_maxX = 0;
    m_minY = 0;
    m_maxY = 0;
    m_lowerLevel = 0.0;
    m_upperLevel = 0.0;
    m_inputIndex = getROIInputIndex();
    m_toolName = "MagicROITool";

    m_roiPolygon = NULL;
    m_filledRoiPolygon = NULL;

    m_state = Ready;

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(initialize()));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(restartRegion()));
    connect(m_2DViewer, SIGNAL(phaseChanged(int)), SLOT(restartRegion()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(restartRegion()));
    connect(m_2DViewer, SIGNAL(restored()), SLOT(restartRegion()));
}

MagicROITool::~MagicROITool()
{
    deleteTemporalRepresentation();
}

void MagicROITool::deleteTemporalRepresentation()
{
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }
    if (!m_filledRoiPolygon.isNull())
    {
        m_filledRoiPolygon->decreaseReferenceCount();
        delete m_filledRoiPolygon;
        m_2DViewer->render();
    }

    m_state = Ready;
}

void MagicROITool::initialize()
{
    // Alliberem les primitives perquè puguin ser esborrades
    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        m_2DViewer->render();
    }
    if (!m_filledRoiPolygon.isNull())
    {
        m_filledRoiPolygon->decreaseReferenceCount();
        delete m_filledRoiPolygon;
        m_2DViewer->render();
    }

    m_roiPolygon = NULL;
    m_filledRoiPolygon = NULL;

    m_inputIndex = getROIInputIndex();

    m_state = Ready;
}

void MagicROITool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            startRegion();
            break;
        case vtkCommand::LeftButtonReleaseEvent:
            endRegion();
            break;
        case vtkCommand::MouseMoveEvent:
            modifyRegionByFactor();
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

void MagicROITool::setTextPosition(DrawerText *text)
{
    double bounds[6];
    m_roiPolygon->getBounds(bounds);

    int xIndex, yIndex, zIndex;
    m_2DViewer->getView().getXYZIndexes(xIndex, yIndex, zIndex);

    Vector3 attachmentPoint;
    attachmentPoint[xIndex] = (bounds[xIndex * 2] + bounds[xIndex * 2 + 1]) / 2.0;
    attachmentPoint[zIndex] = bounds[zIndex * 2];
    if (m_2DViewer->getView() == OrthogonalPlane::XYPlane)
    {
        attachmentPoint[yIndex] = bounds[yIndex * 2 + 1];
    }
    else
    {
        attachmentPoint[yIndex] = bounds[yIndex * 2];
    }

    const double Padding = 5.0;
    double paddingY = 0.0;

    paddingY = -Padding;
    text->setVerticalJustification("Top");

    Vector3 attachmentPointInDisplay;
    // Passem attachmentPoint a coordenades de display
    m_2DViewer->computeWorldToDisplay(attachmentPoint[0], attachmentPoint[1], attachmentPoint[2], attachmentPointInDisplay.data());
    // Apliquem el padding i tornem a coordenades de món
    m_2DViewer->computeDisplayToWorld(attachmentPointInDisplay[0], attachmentPointInDisplay[1] + paddingY, attachmentPointInDisplay[2], attachmentPoint.data());

    text->setAttachmentPoint(attachmentPoint);
}

SliceOrientedVolumePixelData MagicROITool::getPixelData()
{
    return m_2DViewer->getCurrentPixelDataFromInput(m_inputIndex);
}

void MagicROITool::computeMaskBounds()
{
    auto extent = getPixelData().getExtent();   // slice oriented extent
    m_minX = extent[0];
    m_maxX = extent[1];
    m_minY = extent[2];
    m_maxY = extent[3];
}

double MagicROITool::getVoxelValue(const VoxelIndex &index) // slice oriented index
{
    return getPixelData().getVoxelValue(index).getComponent(0);
}

void MagicROITool::startRegion()
{
    if (m_state == Ready && m_2DViewer->hasInput())
    {
        if (m_2DViewer->getCurrentCursorImageCoordinateOnInput(m_pickedPosition.data(), m_inputIndex))
        {
            // Discard a border of 1 pixel around the image (workaround for #1949)
            // TODO Implement a better solution, probably reimplementing the whole algorithm
            VoxelIndex index = getPickedPositionVoxelIndex();   // slice oriented index
            computeMaskBounds();

            if (index.x() == m_minX || index.x() == m_maxX || index.y() == m_minY || index.y() == m_maxY)
            {
                return;
            }

            m_pickedPositionInDisplayCoordinates = m_2DViewer->getEventPosition();
            m_magicFactor = InitialMagicFactor;
            m_roiPolygon = new DrawerPolygon;
            m_roiPolygon->increaseReferenceCount();
            m_2DViewer->getDrawer()->draw(m_roiPolygon);
            m_filledRoiPolygon = new DrawerPolygon();
            m_filledRoiPolygon->increaseReferenceCount();
            m_filledRoiPolygon->setFilled(true);
            m_filledRoiPolygon->setOpacity(0.5);
            m_2DViewer->getDrawer()->draw(m_filledRoiPolygon);
            m_state = Drawing;

            this->generateRegion();
        }
    }
}

void MagicROITool::endRegion()
{
    if (m_roiPolygon)
    {
        this->generateRegion();
        this->printData();
        // Alliberem la primitiva perquè es pugui esborrar
        m_roiPolygon->decreaseReferenceCount();
        // Col·loquem el dibuix al lloc corresponent
        m_2DViewer->getDrawer()->erasePrimitive(m_roiPolygon);
        m_2DViewer->getDrawer()->draw(m_roiPolygon, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
        // Re-iniciem el punter
        m_roiPolygon = NULL;
    }

    if (m_filledRoiPolygon)
    {
        // Alliberem la primitiva perquè es pugui esborrar
        m_filledRoiPolygon->decreaseReferenceCount();
        // Esborrem el polígon ple del visor i el destruïm
        m_2DViewer->getDrawer()->erasePrimitive(m_filledRoiPolygon);
        delete m_filledRoiPolygon;
        m_filledRoiPolygon = NULL;
    }

    m_state = Ready;
}

void MagicROITool::restartRegion()
{
    // Check that mouse is over the viewer and the left button is pressed
    if (m_2DViewer->underMouse() && QApplication::mouseButtons().testFlag(Qt::LeftButton))
    {
        if (!m_filledRoiPolygon.isNull())
        {
            m_filledRoiPolygon->decreaseReferenceCount();
            delete m_filledRoiPolygon;
        }

        if (!m_roiPolygon.isNull())
        {
            m_roiPolygon->decreaseReferenceCount();
            delete m_roiPolygon;
        }

        m_state = Ready;
        startRegion();
    }
}

void MagicROITool::modifyRegionByFactor()
{
    if (m_state == Drawing)
    {
        const double ScaleFactor = 0.05;

        // Fem servir la distància al punt inicial que s'ha clicat. Es fa la distància de mahattan que és una bona aproximació i molt més ràpida de calcular
        int displacement =  (m_viewer->getEventPosition() - m_pickedPositionInDisplayCoordinates).manhattanLength();
        m_magicFactor = displacement * ScaleFactor;
        if (m_magicFactor < 0.0)
        {
            m_magicFactor = 0.0;
        }

        this->generateRegion();
    }
}

void MagicROITool::generateRegion()
{
    computeMaskBounds();

    // Posem a true els punts on la imatge està dins els llindard i connectat amb la llavor (region growing)
    this->computeRegionMask();

    // Trobem els punts frontera i creem el polígon
    this->computePolygon();

    m_2DViewer->render();
}

int MagicROITool::getROIInputIndex() const
{
    if (!m_2DViewer)
    {
        return 0;
    }
    
    int index = 0;
    if (m_2DViewer->getNumberOfInputs() == 2)
    {
        QStringList modalities;
        modalities << m_2DViewer->getInput(0)->getModality() << m_2DViewer->getInput(1)->getModality();

        if (modalities.contains("CT") && modalities.contains("PT"))
        {
            if (m_2DViewer->getInput(1)->getModality() == "PT")
            {
                index = 1;
            }
        }
        else if (modalities.contains("CT") && modalities.contains("NM"))
        {
            if (m_2DViewer->getInput(1)->getModality() == "NM")
            {
                index = 1;
            }
        }
    }
    
    return index;
}

VoxelIndex MagicROITool::getPickedPositionVoxelIndex()
{
    return getPixelData().getVoxelIndex(m_pickedPosition);  // slice oriented voxel index
}

void MagicROITool::computeLevelRange()
{
    // Calculem la desviació estàndard dins la finestra que ens marca la magic size
    double standardDeviation = getStandardDeviation();
    
    // Calculem els llindars com el valor en el pixel +/- la desviació estàndard * magic factor
    double value = this->getVoxelValue(getPickedPositionVoxelIndex());
    m_lowerLevel = value - m_magicFactor * standardDeviation;
    m_upperLevel = value + m_magicFactor * standardDeviation;
}

void MagicROITool::computeRegionMask()
{
    this->computeLevelRange();

    // Creem la màscara
    if (m_minX == 0 && m_minY == 0)
    {
        m_mask = QVector<bool>((m_maxX + 1) * (m_maxY + 1), false);
    }
    else
    {
        DEBUG_LOG("ERROR: extension no comença a 0");
    }
    
    VoxelIndex index = getPickedPositionVoxelIndex();   // slice oriented index
    double value = this->getVoxelValue(index);
    int x = index.x(), y = index.y(), z = index.z();

    if ((value >= m_lowerLevel) && (value <= m_upperLevel))
    {
        int maskIndex = getMaskVectorIndex(x, y);
        m_mask[maskIndex] = true;
    }
    else
    {
        DEBUG_LOG("Ha petat i sortim");
        return;
    }

    // Comencem el Region Growing
    QVector<int> movements;
    // First movement \TODO Codi duplicat amb main loop
    int i = 0;
    bool found = false;
    int maskIndex = 0;
    while (i < 4 && !found)
    {
        this->doMovement(x, y, i);
        value = this->getVoxelValue(VoxelIndex(x, y, z));

        if ((value >= m_lowerLevel) && (value <= m_upperLevel))
        {
            maskIndex = getMaskVectorIndex(x, y);
            m_mask[maskIndex] = true;
            found = true;
            movements.push_back(i);
        }
        if (!found)
        {
            this->undoMovement(x, y, i);
        }
        ++i;
    }

    // Main loop
    i = 0;
    while (movements.size() > 0)
    {
        found = false;
        while (i < 4 && !found)
        {
            this->doMovement(x, y, i);
            if ((x > m_minX) && (x < m_maxX) && (y > m_minY) && (y < m_maxY))
            {
                value = this->getVoxelValue(VoxelIndex(x, y, z));
                maskIndex = getMaskVectorIndex(x, y);
                if ((value >= m_lowerLevel) && (value <= m_upperLevel) && (!m_mask[maskIndex]))
                {
                    m_mask[maskIndex] = true;
                    found = true;
                    movements.push_back(i);
                    i = 0;
                }
            }
            if (!found)
            {
                this->undoMovement(x, y, i);
                ++i;
            }
        }
        if (!found)
        {
            this->undoMovement(x, y, movements.back());
            i = movements.back();
            ++i;
            movements.pop_back();
        }
    }
}

void MagicROITool::doMovement(int &x, int &y, int movement)
{
    switch (movement)
    {
        case MoveRight:
            x++;
            break;
        case MoveLeft:
            x--;
            break;
        case MoveUp:
            y++;
            break;
        case MoveDown:
            y--;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicROITool::undoMovement(int &x, int &y, int movement)
{
    switch (movement)
    {
        case MoveRight:
            x--;
            break;
        case MoveLeft:
            x++;
            break;
        case MoveUp:
            y--;
            break;
        case MoveDown:
            y++;
            break;
        default:
            DEBUG_LOG("Invalid movement");
    }
}

void MagicROITool::computePolygon()
{
    int i = m_minX;
    int j;
    int maskIndex = 0;
    // Busquem el primer punt
    bool found = false;
    while ((i <= m_maxX) && !found)
    {
        j = m_minY;
        while ((j <= m_maxY) && !found)
        {
            maskIndex = getMaskVectorIndex(i, j);
            if (m_mask[maskIndex])
            {
                found = true;
            }
            ++j;
        }
        ++i;
    }

    // L'índex és -1 pq els hem incrementat una vegada més    
    int x = i - 1;
    int y = j - 1;
    m_roiPolygon->removeVertices();
    m_filledRoiPolygon->removeVertices();
    
    this->addPoint(7, x, y);
    this->addPoint(1, x, y);
    
    int nextX;
    int nextY;

    int direction = 0;

    bool loop = false;
    bool next = false;
    while (!loop)
    {
        this->getNextIndex(direction, x, y, nextX, nextY);
        next = getMaskValue(nextX, nextY);
        while (!next && !loop)
        {
            if (MathTools::isOdd(direction) && !next)
            {
                this->addPoint(direction, x, y);
                loop = this->isLoopReached();
            }
            direction = this->getNextDirection(direction);
            this->getNextIndex(direction, x, y, nextX, nextY);
            next = getMaskValue(nextX, nextY);
        }
        x = nextX;
        y = nextY;
        direction = this->getInverseDirection(direction);
        direction = this->getNextDirection(direction);
    }

    m_roiPolygon->update();
    m_filledRoiPolygon->update();
}

void MagicROITool::getNextIndex(int direction, int x, int y, int &nextX, int &nextY)
{
    switch (direction)
    {
        case LeftDown:
            nextX = x - 1;
            nextY = y - 1;
            break;
        case Down:
            nextX = x;
            nextY = y - 1;
            break;
        case RightDown:
            nextX = x + 1;
            nextY = y - 1;
            break;
        case Right:
            nextX = x + 1;
            nextY = y;
            break;
        case RightUp:
            nextX = x + 1;
            nextY = y + 1;
            break;
        case Up:
            nextX = x;
            nextY = y + 1;
            break;
        case LeftUp:
            nextX = x - 1;
            nextY = y + 1;
            break;
        case Left:
            nextX = x - 1;
            nextY = y;
            break;
        default:
            DEBUG_LOG("ERROR: This direction doesn't exist");
    }
}

int MagicROITool::getNextDirection(int direction)
{
    int nextDirection = direction + 1;
    return (nextDirection == 8)? 0: nextDirection;
}

int MagicROITool::getInverseDirection(int direction)
{
    return (direction + 4) % 8;
}

void MagicROITool::addPoint(int direction, int x, int y)
{
    int z = getPickedPositionVoxelIndex().z();
    Vector3 p1 = getPixelData().getWorldCoordinate(VoxelIndex(x, y, z));
    Vector3 p2;

    switch (direction)
    {
        case Down:
            p2 = getPixelData().getWorldCoordinate(VoxelIndex(x, y-1, z));
            break;
        case Right:
            p2 = getPixelData().getWorldCoordinate(VoxelIndex(x+1, y, z));
            break;
        case Up:
            p2 = getPixelData().getWorldCoordinate(VoxelIndex(x, y+1, z));
            break;
        case Left:
            p2 = getPixelData().getWorldCoordinate(VoxelIndex(x-1, y, z));
            break;
        default:
            DEBUG_LOG("ERROR: This direction doesn't exist");
    }

    Vector3 point = (p1 + p2) * 0.5;

    m_roiPolygon->addVertex(point);
    m_filledRoiPolygon->addVertex(point);
}

bool MagicROITool::isLoopReached()
{
    auto firstVertex = this->m_roiPolygon->getVertex(0);
    auto lastVertex = this->m_roiPolygon->getVertex(m_roiPolygon->getNumberOfPoints() - 1);
    return ((qAbs(firstVertex[0] - lastVertex[0]) < 0.0001)
         && (qAbs(firstVertex[1] - lastVertex[1]) < 0.0001)
         && (qAbs(firstVertex[2] - lastVertex[2]) < 0.0001));
}

double MagicROITool::getStandardDeviation()
{
    VoxelIndex index = getPickedPositionVoxelIndex();   // slice oriented index
    int minX = qMax(index.x() - MagicSize, m_minX);
    int maxX = qMin(index.x() + MagicSize, m_maxX);
    int minY = qMax(index.y() - MagicSize, m_minY);
    int maxY = qMin(index.y() + MagicSize, m_maxY);
    int z = index.z();

    // Calculem la mitjana
    double mean = 0.0;
    double value;

    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            value = this->getVoxelValue(VoxelIndex(i, j, z));
            mean += value;
        }
    }

    int numberOfSamples = (maxX - minX + 1) * (maxY - minY + 1);
    mean = mean / (double)numberOfSamples;

    // Calculem la desviació estandard
    double deviation = 0.0;
    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            value = this->getVoxelValue(VoxelIndex(i, j, z));
            deviation += qPow(value - mean, 2);
        }
    }
    deviation = qSqrt(deviation / (double)numberOfSamples);
    return deviation;
}

int MagicROITool::getMaskVectorIndex(int x, int y) const
{
    return y * (m_maxX + 1) + x;
}

bool MagicROITool::getMaskValue(int x, int y) const
{
    if (MathTools::isInsideRange(x, m_minX, m_maxX) && MathTools::isInsideRange(y, m_minY, m_maxY))
    {
        int maskIndex = getMaskVectorIndex(x, y);
        return m_mask[maskIndex];
    }
    else
    {
        return false;
    }
}

}
