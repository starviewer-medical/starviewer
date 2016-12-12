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

#include "transdifferencetool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "transdifferencetooldata.h"
#include "voilut.h"
#include "volume.h"
#include "volumepixeldataiterator.h"

#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

TransDifferenceTool::TransDifferenceTool(QViewer *viewer, QObject *parent)
: Tool(viewer, parent)
{
    m_state = None;
    m_toolName = "TransDifferenceTool";
    m_hasPersistentData = true;

    m_myData = new TransDifferenceToolData;

    //DEBUG_LOG("Tool creada");

    // Ens assegurem que desde la creació tenim un viewer vàlid
    Q_ASSERT(m_viewer);
    m_2DViewer = Q2DViewer::castFromQViewer(viewer);
}

TransDifferenceTool::~TransDifferenceTool()
{
    m_viewer->unsetCursor();
}

void TransDifferenceTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::LeftButtonPressEvent:
            if (m_myData->getInputVolume() != 0 && m_myData->getDifferenceVolume() != 0)
            {
                this->startTransDifference();
            }
            break;

        case vtkCommand::MouseMoveEvent:
            if (m_state == Moving && m_myData->getInputVolume() != 0 && m_myData->getDifferenceVolume() != 0)
            {
                this->doTransDifference();
            }
            break;

        case vtkCommand::LeftButtonReleaseEvent:
            if (m_myData->getInputVolume() != 0 && m_myData->getDifferenceVolume() != 0)
            {
                this->endTransDifference();
            }
            break;
        case vtkCommand::KeyPressEvent:
        {
            if (m_myData->getInputVolume() != 0 && m_myData->getDifferenceVolume() != 0)
            {
                int key = m_viewer->getInteractor()->GetKeyCode();
                switch (key)
                {
                    // 'R'
                    case 114:
                    // 'r'
                    case 82:
                        this->increaseSingleDifferenceImage(0, -1);
                        m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(), -1);
                        break;
                    // 'C'
                    case 99:
                    // 'c'
                    case 67:
                        this->increaseSingleDifferenceImage(0, 1);
                        m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(), 1);
                        break;
                    // 'F'
                    case 102:
                    // 'f'
                    case 70:
                        this->increaseSingleDifferenceImage(1, 0);
                        m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(), 1);
                        break;
                    // 'D'
                    case 100:
                    // 'd'
                    case 68:
                        this->increaseSingleDifferenceImage(-1, 0);
                        m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(), -1);
                        break;
                }
            }
        }
            break;
        default:
            break;
    }
}

ToolData *TransDifferenceTool::getToolData() const
{
    return m_myData;
}

void TransDifferenceTool::setToolData(ToolData *data)
{
    // Fem aquesta comparació perquè a vegades ens passa la data que ja tenim a m_myData
    if (m_myData != data)
    {
        // Creem de nou les dades
        m_toolData = data;
        m_myData = qobject_cast<TransDifferenceToolData*>(data);
    }
}

void TransDifferenceTool::startTransDifference()
{
    m_state = Moving;
    m_startPosition = m_viewer->getEventPosition();
    m_dx = 0;
    m_dy = 0;
}

void TransDifferenceTool::doTransDifference()
{
    // TODO: canviar imatge
    m_viewer->setCursor(QCursor(QPixmap(":/images/icons/transform-move-difference.svg")));

    QPoint currentPosition = m_viewer->getEventPosition();

    // Compute normalized delta
    m_dx = currentPosition.x() - m_startPosition.x();
    m_dy = m_startPosition.y() - currentPosition.y();

    this->increaseSingleDifferenceImage(m_dx, m_dy);

    // Necessari perquè es torni a renderitzar a alta resolució en el 3D
    m_viewer->render();
}

void TransDifferenceTool::endTransDifference()
{
    //DEBUG_LOG("End moving");
    m_viewer->unsetCursor();
    m_state = None;
    m_myData->increaseSliceTranslationX(m_2DViewer->getCurrentSlice(), m_dx);
    m_myData->increaseSliceTranslationY(m_2DViewer->getCurrentSlice(), m_dy);
    // Necessari perquè es torni a renderitzar a alta resolució en el 3D
    m_viewer->render();
}

void TransDifferenceTool::initializeDifferenceImage()
{
    if (m_myData->getInputVolume() == 0)
    {
        DEBUG_LOG("ERROR: Estem inicialitzant la diferència sense tenir input!");
        return;
    }

    // Actualitzem el vector de translacions (inicialitzem a 0)
    // Potser caldria aplicar les mateixes translacions que ja hi ha?
    //m_sliceTranslations = QVector<QPair<int,int > >(mainVolume->getDimensions()[2],QPair<int,int>(0,0));

    Volume *mainVolume = m_myData->getInputVolume();
    Volume *differenceVolume = m_myData->getDifferenceVolume();

    int ext[6];
    mainVolume->getExtent(ext);

    // Si no hi ha volume diferència
    if (differenceVolume == 0)
    {
        // Allocating memory for the output image
        vtkImageData *imdif = vtkImageData::New();
        imdif->DeepCopy(mainVolume->getVtkData());
        imdif->SetExtent(ext);

        // Converting the VTK data to volume
        differenceVolume = new Volume();
        differenceVolume->setImages(mainVolume->getImages());
        differenceVolume->setData(imdif);

        m_myData->setDifferenceVolume(differenceVolume);
    }

    int k;
    for (k = ext[4]; k <= ext[5]; k++)
    {
        this->computeSingleDifferenceImage(0, 0, k);
    }

    double range[2];
    differenceVolume->getScalarRange(range);
    int max;
    if (-range[0] > range[1])
    {
        max = -range[0];
    }
    else
    {
        max = range[1];
    }

    m_2DViewer->setInput(differenceVolume);
    m_2DViewer->setVoiLut(WindowLevel(2 * max, 0.0));

    m_2DViewer->render();
}

void TransDifferenceTool::increaseSingleDifferenceImage(int dx, int dy)
{
    int tx = m_myData->getSliceTranslationX(m_2DViewer->getCurrentSlice()) + dx;
    int ty = m_myData->getSliceTranslationY(m_2DViewer->getCurrentSlice()) + dy;
    this->computeSingleDifferenceImage(tx, ty);
}

void TransDifferenceTool::setSingleDifferenceImage(int dx, int dy)
{
    this->computeSingleDifferenceImage(dx, dy);
    m_myData->setSliceTranslationX(m_2DViewer->getCurrentSlice(), dx);
    m_myData->setSliceTranslationY(m_2DViewer->getCurrentSlice(), dy);
}

void TransDifferenceTool::computeSingleDifferenceImage(int dx, int dy, int slice)
{
    // \TODO: Fer-ho amb un filtre

    // Simplifiquem dient que la translació només pot ser per múltiples del píxel
    // Pintem la diferència al volume a la llesca "slice"
    // Ho fem amb vtk pq és més ràpid
    int indexRef[3];
    int indexMov[3];
    int indexDif[3];

    int currentSlice;
    // Si no ens han posat slice agafem la que està el visor
    if (slice == -1)
    {
        currentSlice = m_2DViewer->getCurrentSlice();
    }
    else
    {
        currentSlice = slice;
    }

    Volume *mainVolume = m_myData->getInputVolume();
    Volume *differenceVolume = m_myData->getDifferenceVolume();

    // Les translacions són les que ja hi havia a la llesca més el que ens hem mogut amb el cursor
    int tx = dx;
    int ty = dy;

    // Restem 1 al reference slice perquè aquest considera la primera llesca com la 1
    indexRef[2] = m_myData->getReferenceSlice() - 1;
    indexMov[2] = currentSlice;
    indexDif[2] = currentSlice;

    int size[3];
    mainVolume->getDimensions(size);

    int i, j;
    int imax, imin;
    int jmax, jmin;

    imin = tx < 0 ? 0 : tx;
    imax = tx < 0 ? size[0] + tx : size[0];
    jmin = ty < 0 ? 0 : ty;
    jmax = ty < 0 ? size[1] + ty : size[1];

    if (imin - tx < size[0] && imin < size[0])
    {
        indexRef[0] = imin;
        indexMov[0] = imin - tx;
        // L'index de la diferència el posem a 0 perquè assignem valors a tota la fila
        indexDif[0] = 0;
        for (j = jmin; j < jmax; j++)
        {
            indexRef[1] = j;
            indexMov[1] = j - ty;
            indexDif[1] = j;
            VolumePixelDataIterator itRef = mainVolume->getIterator(indexRef[0], indexRef[1], indexRef[2]);
            VolumePixelDataIterator itMov = mainVolume->getIterator(indexMov[0], indexMov[1], indexMov[2]);
            VolumePixelDataIterator itDif = differenceVolume->getIterator(indexDif[0], indexDif[1], indexDif[2]);
            for (i = 0; i < imin; i++)
            {
                itDif.set(0);
                ++itDif;
            }
            for (i = imin; i < imax; i++)
            {
                itDif.set(itMov.get<int>() - itRef.get<int>());
                ++itRef;
                ++itMov;
                ++itDif;
            }
            for (i = imax; i < size[0]; i++)
            {
                itDif.set(0);
                ++itDif;
            }
        }
    }
    // Posem 0 a les files que no hem fet perquè es visualitzi més bonic
    indexDif[0] = 0;
    // Per evitar que se'ns en vagi de rang
    if (jmin > size[1])
    {
        jmin = size[1];
    }

    for (j = 0; j < jmin; j++)
    {
        indexDif[1] = j;
        VolumePixelDataIterator itDif = differenceVolume->getIterator(indexDif[0], indexDif[1], indexDif[2]);
        for (i = 0; i < size[0]; i++)
        {
            itDif.set(0);
            ++itDif;
        }
    }
    if (jmax < 0)
    {
        jmax = 0;
    }
    for (j = jmax; j < size[1]; j++)
    {
        indexDif[1] = j;
        VolumePixelDataIterator itDif = differenceVolume->getIterator(indexDif[0], indexDif[1], indexDif[2]);
        for (i = 0; i < size[0]; i++)
        {
            itDif.set(0);
            ++itDif;
        }
    }

    // Això ho fem perquè ens refresqui la imatge diferència que hem modificat
    if (slice == -1)
    {
        // HACK Així obliguem a que es torni a executar el pipeline en el viewer i es renderitzi la nova imatge calculada
        // TODO Caldria canviar la manera en com modifiquem les dades del volum perquè la notificació de modificació
        // fos transparent i no ho haguem de fer una crida tant explícita com aquesta
        differenceVolume->getVtkData()->Modified();
    }
}

}
