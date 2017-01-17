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
#include "roidata.h"
#include "voxel.h"
#include "voxelindex.h"

#include <QApplication> // to check pressed mouse buttons
#include <QStack>

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

MagicROITool::MagicROITool(QViewer *viewer, QObject *parent)
: ROITool(viewer, parent), m_magicFactor(0), m_minX(0), m_maxX(0), m_minY(0), m_maxY(0)
{
    m_toolName = "MagicROITool";

    reset();

    connect(m_2DViewer, SIGNAL(volumeChanged(Volume*)), SLOT(reset()));
    connect(m_2DViewer, SIGNAL(sliceChanged(int)), SLOT(restartRegion()));
    connect(m_2DViewer, SIGNAL(phaseChanged(int)), SLOT(restartRegion()));
    connect(m_2DViewer, SIGNAL(viewChanged(int)), SLOT(restartRegion()));
    connect(m_2DViewer, SIGNAL(restored()), SLOT(restartRegion()));
}

MagicROITool::~MagicROITool()
{
    reset();
}

void MagicROITool::reset()
{
    bool needsToRender = false;

    if (!m_roiPolygon.isNull())
    {
        m_roiPolygon->decreaseReferenceCount();
        delete m_roiPolygon;
        needsToRender = true;
    }

    if (!m_filledRoiPolygon.isNull())
    {
        m_filledRoiPolygon->decreaseReferenceCount();
        delete m_filledRoiPolygon;
        needsToRender = true;
    }

    if (needsToRender)
    {
        m_2DViewer->render();
    }

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
                reset();
            }
            break;
    }
}

void MagicROITool::setTextPosition(DrawerText *text)
{
    auto displayBounds = getDisplayBounds();

    const double Padding = 5.0;
    Vector3 attachmentPointDisplay((displayBounds[0] + displayBounds[1]) * 0.5, displayBounds[2] - Padding, 0.0);
    auto attachmentPoint = m_2DViewer->computeDisplayToWorld(attachmentPointDisplay);

    text->setVerticalJustification("Top");
    text->setAttachmentPoint(attachmentPoint);
}

SliceOrientedVolumePixelData& MagicROITool::getPixelData()
{
    return m_cachedPixelData;
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
    constexpr double InitialMagicFactor = 0.0;

    if (m_state == Ready && m_2DViewer->hasInput())
    {
        if (m_2DViewer->getCurrentCursorImageCoordinateOnInput(m_startWorldCoordinate.data(), m_inputIndex))
        {
            // Cache current pixel data
            m_cachedPixelData = m_2DViewer->getCurrentPixelDataFromInput(m_inputIndex);
            m_startVoxelIndex = getPixelData().getVoxelIndex(m_startWorldCoordinate);   // slice oriented voxel index

            // Discard a border of 1 pixel around the image (workaround for #1949)
            // TODO Implement a better solution, probably reimplementing the whole algorithm
            computeMaskBounds();
            if (m_startVoxelIndex.x() == m_minX || m_startVoxelIndex.x() == m_maxX || m_startVoxelIndex.y() == m_minY || m_startVoxelIndex.y() == m_maxY)
            {
                return;
            }

            m_startEventPosition = m_2DViewer->getEventPosition();
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
    if (m_state == Drawing)
    {
        if (m_roiPolygon)
        {
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
}

void MagicROITool::restartRegion()
{
    // Check that mouse is over the viewer and the left button is pressed
    if (m_state == Drawing && m_2DViewer->underMouse() && QApplication::mouseButtons().testFlag(Qt::LeftButton))
    {
        reset();
        startRegion();
    }
}

void MagicROITool::modifyRegionByFactor()
{
    if (m_state == Drawing)
    {
        const double ScaleFactor = 0.05;

        // Fem servir la distància al punt inicial que s'ha clicat. Es fa la distància de mahattan que és una bona aproximació i molt més ràpida de calcular
        int displacement =  (m_viewer->getEventPosition() - m_startEventPosition).manhattanLength();
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
    // Posem a true els punts on la imatge està dins els llindard i connectat amb la llavor (region growing)
    auto firstTrueInMask = this->computeRegionMask();

    // Trobem els punts frontera i creem el polígon
    this->computePolygon(firstTrueInMask);

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

const VoxelIndex& MagicROITool::getStartVoxelIndex() const
{
    return m_startVoxelIndex;   // slice oriented voxel index
}

std::array<double, 2> MagicROITool::computeLevelRange()
{
    // Calculem la desviació estàndard dins la finestra que ens marca la magic size
    double standardDeviation = getStandardDeviation();
    
    // Calculem els llindars com el valor en el pixel +/- la desviació estàndard * magic factor
    double value = this->getVoxelValue(getStartVoxelIndex());
    double lowerLevel = value - m_magicFactor * standardDeviation;
    double upperLevel = value + m_magicFactor * standardDeviation;

    return {{lowerLevel, upperLevel}};
}

// Movements
enum { MoveRight, MoveLeft, MoveUp, MoveDown };

std::array<int, 2> MagicROITool::computeRegionMask()
{
    auto levelRange = this->computeLevelRange();
    double lowerLevel = levelRange[0];
    double upperLevel = levelRange[1];

    // Creem la màscara
    m_mask = QVector<bool>((m_maxX - m_minX + 1) * (m_maxY - m_minY + 1), false);

    VoxelIndex index = getStartVoxelIndex();   // slice oriented index
    double value = this->getVoxelValue(index);
    int x = index.x(), y = index.y(), z = index.z();

    if ((value >= lowerLevel) && (value <= upperLevel))
    {
        int maskIndex = getMaskVectorIndex(x, y);
        m_mask[maskIndex] = true;
    }
    else
    {
        DEBUG_LOG("Ha petat i sortim");
        return std::array<int, 2>();
    }

    // Keep indices of first true value
    std::array<int, 2> firstTrue{{x, y}};
    auto updateFirstTrue = [&firstTrue](int x, int y) {
        if (x < firstTrue[0] || (x == firstTrue[0] && y < firstTrue[1]))
        {
            firstTrue[0] = x;
            firstTrue[1] = y;
        }
    };

    // Comencem el Region Growing
    QStack<int> moves;

    // First movement \TODO Codi duplicat amb main loop
    int move = 0;
    bool found = false;

    while (move < 4 && !found)
    {
        this->doMovement(x, y, move);
        value = this->getVoxelValue(VoxelIndex(x, y, z));

        if ((value >= lowerLevel) && (value <= upperLevel))
        {
            int maskIndex = getMaskVectorIndex(x, y);
            m_mask[maskIndex] = true;
            updateFirstTrue(x, y);
            moves.push(move);
            found = true;
        }
        else
        {
            this->undoMovement(x, y, move);
        }

        ++move;
    }

    // Main loop
    move = 0;

    while (!moves.isEmpty())
    {
        found = false;

        while (move < 4 && !found)
        {
            this->doMovement(x, y, move);

            if ((x > m_minX) && (x < m_maxX) && (y > m_minY) && (y < m_maxY))
            {
                value = this->getVoxelValue(VoxelIndex(x, y, z));
                int maskIndex = getMaskVectorIndex(x, y);

                if ((value >= lowerLevel) && (value <= upperLevel) && (!m_mask[maskIndex]))
                {
                    m_mask[maskIndex] = true;
                    updateFirstTrue(x, y);
                    moves.push(move);
                    move = 0;
                    found = true;
                }
            }

            if (!found)
            {
                this->undoMovement(x, y, move);
                ++move;
            }
        }

        if (!found)
        {
            move = moves.pop();
            this->undoMovement(x, y, move);
            ++move;
        }
    }

    return firstTrue;
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

// Directions
enum { LeftDown, Down, RightDown, Right, RightUp, Up, LeftUp, Left };

void MagicROITool::computePolygon(const std::array<int, 2> &firstTrueInMask)
{
    m_roiPolygon->removeVertices();
    m_filledRoiPolygon->removeVertices();

    int x = firstTrueInMask[0];
    int y = firstTrueInMask[1];
    this->addPoint(Left, x, y);
    this->addPoint(Down, x, y);
    
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
    return (direction + 1) % 8;
}

int MagicROITool::getInverseDirection(int direction)
{
    return (direction + 4) % 8;
}

void MagicROITool::addPoint(int direction, int x, int y)
{
    int z = getStartVoxelIndex().z();
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
    constexpr int MagicSize = 3;

    const VoxelIndex &index = getStartVoxelIndex(); // slice oriented index
    int minX = qMax(index.x() - MagicSize, m_minX);
    int maxX = qMin(index.x() + MagicSize, m_maxX);
    int minY = qMax(index.y() - MagicSize, m_minY);
    int maxY = qMin(index.y() + MagicSize, m_maxY);
    int z = index.z();

    ROIData roiData;

    for (int i = minX; i <= maxX; ++i)
    {
        for (int j = minY; j <= maxY; ++j)
        {
            roiData.addVoxel(getPixelData().getVoxelValue(VoxelIndex(i, j, z)));
        }
    }

    return roiData.getStandardDeviation();
}

int MagicROITool::getMaskVectorIndex(int x, int y) const
{
    return (y - m_minY) * (m_maxX - m_minX + 1) + x - m_minX;
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
