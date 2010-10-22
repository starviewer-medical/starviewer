/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "extractimagetool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "drawer.h"
#include "drawerpolyline.h"
#include "drawerimage.h"
#include "polylineroioutliner.h"
#include "extractimagetoolrepresentation.h"
#include "representationslayer.h"
#include "shutterfilter.h"
#include "volume.h"
// vtk
#include <vtkImageData.h>

namespace udg {

ExtractImageTool::ExtractImageTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent)
{
    m_toolName = "ExtractImageTool";
    m_hasSharedData = false;

    m_2DViewer = qobject_cast<Q2DViewer *>(viewer);
    if (!m_2DViewer)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que viewer no sigui un Q2DViewer!!!-> ") + viewer->metaObject()->className());
    }

    m_polylineROIOutliner = new PolylineROIOutliner(m_2DViewer);

    connect(this, SIGNAL(forwardEvent(long unsigned)), m_polylineROIOutliner, SLOT(handleEvent(long unsigned)));
    connect(m_polylineROIOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    m_polyline = NULL;

    // Default compatibility
    this->setEditionCompatibility(false);
}

ExtractImageTool::~ExtractImageTool()
{
}

void ExtractImageTool::handleEvent(long unsigned eventID)
{
    emit forwardEvent(eventID);
}

void ExtractImageTool::extractImage()
{
    m_image = new DrawerImage();
    ShutterFilter *filter = new ShutterFilter();
    filter->setInput(m_2DViewer->getInput());
    filter->setView(m_2DViewer->getView());
    filter->setBackgroundTransparent();
    // Extreu l'exterior de la figura
    filter->setReverseStencilOff();
    filter->setPolygonalShutter(m_polyline->getPointsList());
    m_image->setWindowLevel(m_2DViewer->getCurrentColorWindow(), m_2DViewer->getCurrentColorLevel());
    m_image->addImage(filter->getOutput());
    m_image->setView(m_2DViewer->getView());
	//m_image->setWindowLevelLutMapper(m_2DViewer->getWindowLevelMapper());
    m_image->setBoundingPolyline(m_polyline->getPointsList());
    m_image->setOpacity(0.5);
    //m_image->applyMask();
    m_2DViewer->getDrawer()->draw(m_image);

    // Pintant de negre l'imatge extreta
    /*ShutterFilter *viewerFilter = new ShutterFilter();
    viewerFilter->setInput(m_2DViewer->getInput());
    viewerFilter->setView(m_2DViewer->getView());
    // Extreu l'interior de la figura
    viewerFilter->setReverseStencilOn();
    viewerFilter->setPolygonalShutter(m_polyline->getPointsList());
    m_2DViewer->setInput(viewerFilter->getOutput()); // Ha de ser un Volume.
    m_2DViewer->refresh();*/

    /*Volume *vol = new Volume;
    vol->setImages(m_2DViewer->getInput()->getImages());
    vol->setData(filter->getOutput());
    m_2DViewer->setInput(vol);
    m_2DViewer->refresh();*/
}

void ExtractImageTool::outlinerFinished(DrawerPrimitive *primitive)
{
    disconnect(this, SIGNAL(forwardEvent(long unsigned)), m_polylineROIOutliner, SLOT(handleEvent(long unsigned)));
    disconnect(m_polylineROIOutliner, SIGNAL(finished(DrawerPrimitive *)), this, SLOT(outlinerFinished(DrawerPrimitive *)));

    delete m_polylineROIOutliner;

    m_polyline = qobject_cast<DrawerPolyline *>(primitive);
    if (!m_polyline)
    {
        DEBUG_LOG(QString("El casting no ha funcionat!!! És possible que primitive no sigui un DrawerPolyline!!!-> ") + primitive->metaObject()->className());
    }

    extractImage();

    m_extractImageToolRepresentation = new ExtractImageToolRepresentation(m_2DViewer->getDrawer());
    m_extractImageToolRepresentation->setPolyline(m_polyline);
    m_extractImageToolRepresentation->setImageData(m_image);
    m_extractImageToolRepresentation->setParams(m_2DViewer->getView(), m_2DViewer->getInput()->getOrigin());
    m_extractImageToolRepresentation->calculate();
    m_2DViewer->getRepresentationsLayer()->addRepresentation(m_extractImageToolRepresentation, m_2DViewer->getView(), m_2DViewer->getCurrentSlice());
    emit finished();
}

}
