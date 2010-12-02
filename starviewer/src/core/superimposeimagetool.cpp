/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "superimposeimagetool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "volume.h"
#include "drawerimage.h"
#include "drawer.h"
#include "superimposeimagetoolrepresentation.h"
#include "representationslayer.h"

//vtk
#include <vtkCommand.h>
#include <vtkImageFlip.h>
#include <vtkImageTranslateExtent.h>
#include <vtkMath.h>
//Qt

namespace udg {

SuperimposeImageTool::SuperimposeImageTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "SuperimposeImageTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if(!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_done = false;

    // Default compatibility
    this->setEditionCompatibility(false);
}

SuperimposeImageTool::~SuperimposeImageTool()
{
}

void SuperimposeImageTool::handleEvent(long unsigned eventID)
{
    if(!m_done)
    {
        m_done = true;
        this->superimposeImage();
    }
}

void SuperimposeImageTool::superimposeImage()
{
    QList<double *> bounding;
    m_image = new DrawerImage();
    m_image->setWindowLevel(m_2DViewer->getCurrentColorWindow(), m_2DViewer->getCurrentColorLevel());
    flipImage();
    m_image->addImage(m_vtkFlip->GetOutput());
    m_image->setBoundingPolyline(bounding);
    m_image->setView(m_2DViewer->getView());
    m_image->setOpacity(0.6);

    m_2DViewer->getDrawer()->drawWorkInProgress(m_image);

    m_superimposeImageToolRepresentation = new SuperimposeImageToolRepresentation(m_2DViewer->getDrawer());
    m_superimposeImageToolRepresentation->setImageData(m_image);
    m_superimposeImageToolRepresentation->setParams(m_2DViewer->getView());
    m_superimposeImageToolRepresentation->calculate();

    m_2DViewer->getRepresentationsLayer()->addRepresentation(m_superimposeImageToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    //emit finished();
}

void SuperimposeImageTool::flipImage()
{
    m_vtkFlip = vtkImageFlip::New();

    int xCoordinate, yCoordinate, zCoordinate;

    switch(m_2DViewer->getView())
    {
    case Q2DViewer::Axial:
        xCoordinate = 0;
        yCoordinate = 1;
        zCoordinate = 2;
        break;
    case Q2DViewer::Sagital:
        xCoordinate = 1;
        yCoordinate = 2;
        zCoordinate = 0;
        break;
    case Q2DViewer::Coronal:
        xCoordinate = 0;
        yCoordinate = 2;
        zCoordinate = 1;
        break;
    }

    m_vtkFlip->SetInput(m_2DViewer->getInput()->getVtkData());
    m_vtkFlip->SetFilteredAxis(xCoordinate);
    m_vtkFlip->Update();
}

}
