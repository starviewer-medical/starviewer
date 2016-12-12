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

#include "editortool.h"
#include "editortooldata.h"
#include "q2dviewer.h"
#include "voilut.h"
#include "volume.h"
#include "volumepixeldataiterator.h"

// Vtk
#include <vtkCommand.h>
#include <vtkPoints.h>
#include <vtkUnstructuredGrid.h>
#include <vtkProperty.h>
#include <vtkRenderer.h>
#include <vtkActor.h>
#include <vtkDataSetMapper.h>
#include <vtkRenderWindowInteractor.h>
#include <vtkCellType.h>

namespace udg {

EditorTool::EditorTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_volumeCont(0), m_insideValue(255), m_outsideValue(0), m_isLeftButtonPressed(false)
{
    m_editorState = Paint;
    m_editorSize = 3;
    m_toolName = "EditorTool";
    m_squareActor = vtkActor::New();
    m_myData = new EditorToolData;

    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_2DViewer);

    m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/pencil.svg")));
    this->initialize();

    // \TODO:cada cop que canvïi l'input a l'overlay cal fer algunes inicialitzacions
    connect(m_2DViewer, SIGNAL(overlayChanged()), SLOT(initialize()));
}

EditorTool::~EditorTool()
{
    m_2DViewer->unsetCursor();
    m_squareActor -> Delete();
    delete m_myData;
}

void EditorTool::initialize()
{
    if (m_2DViewer->getOverlayInput())
    {
        double range[2];
        m_2DViewer->getOverlayInput()->getScalarRange(range);

        m_outsideValue = (int)range[0];
        if ((int)range[0] != (int)range[1])
        {
            m_insideValue = (int)range[1];
        }
        else
        {
            // En cas que siguin iguals
            double windowWidth = m_2DViewer->getCurrentVoiLut().getWindowLevel().getWidth();
            m_insideValue = (int)(range[0] + windowWidth);
        }
        int ext[6];
        int i, j, k;
        m_volumeCont = 0;
        m_2DViewer->getOverlayInput()->getExtent(ext);

        VolumePixelDataIterator it = m_2DViewer->getOverlayInput()->getIterator();
        for (i = ext[0]; i <= ext[1]; i++)
        {
            for (j = ext[2]; j <= ext[3]; j++)
            {
                for (k = ext[4]; k <= ext[5]; k++)
                {
                    if (it.get<int>() == m_insideValue)
                    {
                        m_volumeCont++;
                    }
                    ++it;
                }
            }
        }
        m_myData->setVolumeVoxels(m_volumeCont);
    }
}

void EditorTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
    case vtkCommand::LeftButtonPressEvent:
        if (m_2DViewer->getOverlayInput())
        {
            m_isLeftButtonPressed = true;
            this->setEditorPoint();
        }
        break;

    case vtkCommand::MouseMoveEvent:
        if (m_2DViewer->hasInput())
        {
            this->setPaintCursor();
        }
        break;

    case vtkCommand::LeftButtonReleaseEvent:
        m_isLeftButtonPressed = false;
        break;

    case vtkCommand::MouseWheelForwardEvent:
        this->increaseState();
        break;

    case vtkCommand::MouseWheelBackwardEvent:
        this->decreaseState();
        break;

    case vtkCommand::KeyPressEvent:
    {
        int key = m_viewer->getInteractor()->GetKeyCode();
        // '+' = key code 43
        // '-' = key code 45
        switch (key)
        {
            // '+'
            case 43:
                this->increaseEditorSize();
                break;

            // '-'
            case 45:
                this->decreaseEditorSize();
                break;
        }
    }
        break;

    default:
        break;
    }
}

void EditorTool::increaseState()
{
    switch (m_editorState)
    {
        case Paint:
            m_editorState = Erase;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/erasercursor.png")));
            break;

        case Erase:
            m_editorState = EraseRegion;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/translate-erase.svg")));
            m_squareActor->VisibilityOff();
            m_2DViewer->render();
            break;

        case EraseRegion:
            m_editorState = EraseSlice;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/slice-erase.svg")));
            break;

        case EraseSlice:
            m_editorState = Paint;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/pencil.svg")));
            this->setPaintCursor();
            break;

        default:
            break;
    }
}

void EditorTool::decreaseState()
{
    switch (m_editorState)
    {
        case EraseRegion:
            m_editorState = Erase;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/erasercursor.png")));
            this->setPaintCursor();
            break;

        case EraseSlice:
            m_editorState = EraseRegion;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/translate-erase.svg")));
            break;

        case Paint:
            m_editorState = EraseSlice;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/slice-erase.svg")));
            m_squareActor->VisibilityOff();
            m_2DViewer->render();
            break;

        case Erase:
            m_editorState = Paint;
            m_2DViewer->setCursor(QCursor(QPixmap(":/images/cursors/pencil.svg")));
            break;

        default:
            break;
    }

}

void EditorTool::setErase()
{
    m_editorState = Erase;
}

void EditorTool::setPaint()
{
    m_editorState = Paint;
}

void EditorTool::setEraseSlice()
{
    m_editorState = EraseSlice;
}

void EditorTool::setEraseRegion()
{
    m_editorState = EraseRegion;
}

void EditorTool::setEditorPoint()
{
    double pos[3];
    if (m_editorState != NoEditor)
    {
        if (m_2DViewer->getCurrentCursorImageCoordinate(pos))
        {
            switch (m_editorState)
            {
                case Erase:
                {
                    this->eraseMask();
                    break;
                }
                case Paint:
                {
                    this->paintMask();
                    break;
                }
                case EraseSlice:
                {
                    this->eraseSliceMask();
                    break;
                }
                case EraseRegion:
                {
                    this->eraseRegionMask();
                    break;
                }
            }
            m_myData->setVolumeVoxels(m_volumeCont);
            m_2DViewer->updateOverlay();
            m_2DViewer->render();
        }
    }
}

void EditorTool::setPaintCursor()
{
    if (m_isLeftButtonPressed && m_2DViewer->getOverlayInput())
    {
        setEditorPoint();
    }

    double pos[3];
    if ((m_editorState == Erase || m_editorState == Paint) && m_2DViewer->getCurrentCursorImageCoordinate(pos))
    {
        int size = m_editorSize;
        vtkPoints *points = vtkPoints::New();
        points->SetNumberOfPoints(4);

        double spacing[3];
        m_2DViewer->getMainInput()->getSpacing(spacing);
        double sizeView[2];
        sizeView[0] = (double)(size + 0.5) * spacing[0];
        sizeView[1] = (double)(size + 0.5) * spacing[1];

        points->SetPoint(0, pos[0] - sizeView[0], pos[1] - sizeView[1], pos[2] - 1);
        points->SetPoint(1, pos[0] + sizeView[0], pos[1] - sizeView[1], pos[2] - 1);
        points->SetPoint(2, pos[0] + sizeView[0], pos[1] + sizeView[1], pos[2] - 1);
        points->SetPoint(3, pos[0] - sizeView[0], pos[1] + sizeView[1], pos[2] - 1);

        vtkIdType pointIds[4];
        pointIds[0] = 0;
        pointIds[1] = 1;
        pointIds[2] = 2;
        pointIds[3] = 3;

        vtkUnstructuredGrid *grid = vtkUnstructuredGrid::New();

        grid->Allocate(1);
        grid->SetPoints(points);

        grid->InsertNextCell(VTK_QUAD, 4, pointIds);

        m_squareActor->GetProperty()->SetColor(0.15, 0.83, 0.26);
        m_squareActor->GetProperty()->SetOpacity(0.2);

        vtkDataSetMapper *squareMapper = vtkDataSetMapper::New();
        squareMapper->SetInputData(grid);

        m_squareActor->SetMapper(squareMapper);

        m_squareActor->VisibilityOn();

        m_2DViewer->getRenderer()->AddViewProp(m_squareActor);
        m_2DViewer->getRenderer()->ResetCameraClippingRange();
        m_2DViewer->render();

        squareMapper->Delete();
        points->Delete();
        grid->Delete();
    }
    else
    {
        m_squareActor->VisibilityOff();
    }
}

void EditorTool::eraseMask()
{
    int i, j;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getMainInput()->getSpacing(spacing);
    m_2DViewer->getMainInput()->getOrigin(origin);
    centralIndex[0] = (int)((((double)pos[0] - origin[0]) / spacing[0]) + 0.5);
    centralIndex[1] = (int)((((double)pos[1] - origin[1]) / spacing[1]) + 0.5);
    index[2] = m_2DViewer->getCurrentSlice();

    for (i = -m_editorSize; i <= m_editorSize;i++)
    {
        for (j = -m_editorSize; j <= m_editorSize; j++)
        {
            index[0] = centralIndex[0] + i;
            index[1] = centralIndex[1] + j;
            VolumePixelDataIterator it = m_2DViewer->getOverlayInput()->getIterator(index[0], index[1], index[2]);
            if (!it.isNull() && (it.get<int>() == m_insideValue))
            {
                it.set(m_outsideValue);
                m_volumeCont--;
            }
        }
    }
}

void EditorTool::paintMask()
{
    // DEBUG_LOG(QString("Màxim = %1 // Mínim = %2").arg(m_outsideValue).arg(m_insideValue));
    int i, j;
    double pos[3];
    double origin[3];
    double spacing[3];
    int centralIndex[3];
    int index[3];
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getMainInput()->getSpacing(spacing);
    m_2DViewer->getMainInput()->getOrigin(origin);
    centralIndex[0] = (int)((((double)pos[0] - origin[0]) / spacing[0]) + 0.5);
    centralIndex[1] = (int)((((double)pos[1] - origin[1]) / spacing[1]) + 0.5);
    index[2] = m_2DViewer->getCurrentSlice();
    for (i = -m_editorSize; i <= m_editorSize; i++)
    {
        for (j = -m_editorSize; j <= m_editorSize; j++)
        {
            index[0] = centralIndex[0] + i;
            index[1] = centralIndex[1] + j;
            VolumePixelDataIterator it = m_2DViewer->getOverlayInput()->getIterator(index[0], index[1], index[2]);
            if (!it.isNull() && (it.get<int>() != m_insideValue))
            {
                it.set(m_insideValue);
                m_volumeCont++;
            }
        }
    }
}

void EditorTool::eraseSliceMask()
{
    int i, j;
    int index[3];
    int ext[6];
    m_2DViewer->getMainInput()->getExtent(ext);
    index[2] = m_2DViewer->getCurrentSlice();
    for (i = ext[0]; i <= ext[1]; i++)
    {
        for (j = ext[2]; j <= ext[3]; j++)
        {
            index[0] = i;
            index[1] = j;
            VolumePixelDataIterator it = m_2DViewer->getOverlayInput()->getIterator(index[0], index[1], index[2]);
            if (it.get<int>() == m_insideValue)
            {
                it.set(m_outsideValue);
                m_volumeCont--;
            }
        }
    }
}

void EditorTool::eraseRegionMask()
{
    double pos[3];
    double origin[3];
    double spacing[3];
    int index[3];
    int ext[6];
    m_2DViewer->getMainInput()->getExtent(ext);
    m_2DViewer->getCurrentCursorImageCoordinate(pos);
    m_2DViewer->getMainInput()->getSpacing(spacing);
    m_2DViewer->getMainInput()->getOrigin(origin);
    index[0] = (int)((((double)pos[0] - origin[0]) / spacing[0]) + 0.5);
    index[1] = (int)((((double)pos[1] - origin[1]) / spacing[1]) + 0.5);
    index[2] = m_2DViewer->getCurrentSlice();
    eraseRegionMaskRecursive(index[0], index[1], index[2]);
}

void EditorTool::eraseRegionMaskRecursive(int a, int b, int c)
{
    int ext[6];
    m_2DViewer->getMainInput()->getExtent(ext);
    if ((a >= ext[0]) && (a <= ext[1]) && (b >= ext[2]) && (b <= ext[3]) && (c >= ext[4]) && (c <= ext[5]))
    {
        VolumePixelDataIterator it = m_2DViewer->getOverlayInput()->getIterator(a, b, c);
        if (it.get<int>() == m_insideValue)
        {
            it.set(m_outsideValue);
            m_volumeCont--;
            eraseRegionMaskRecursive(a + 1, b, c);
            eraseRegionMaskRecursive(a - 1, b, c);
            eraseRegionMaskRecursive(a, b + 1, c);
            eraseRegionMaskRecursive(a, b - 1, c);
        }
    }
}

void EditorTool::increaseEditorSize()
{
    m_editorSize++;
    this->setPaintCursor();
}

void EditorTool::decreaseEditorSize()
{
    if (m_editorSize > 0)
    {
        m_editorSize--;
        this->setPaintCursor();
    }
}

ToolData* EditorTool::getToolData() const
{
    return m_myData;
}

}
