#include "applyhangingprotocolqviewercommand.h"

#include "q2dviewerwidget.h"
#include "hangingprotocoldisplayset.h"
#include "hangingprotocolimageset.h"
#include "q2dviewer.h"
#include "logging.h"

namespace udg {

ApplyHangingProtocolQViewerCommand::ApplyHangingProtocolQViewerCommand(Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet, QObject *parent)
    : QViewerCommand(parent)
{
    m_viewerWidget = viewer;
    m_displaySet = displaySet;
}

void ApplyHangingProtocolQViewerCommand::execute()
{
    // HACK Així evitem el bug del ticket 1249 i tenim el mateix comportament que abans
    // Deshabilitem inicialment la sincronització i només l'activem si és necessari pel hanging protocol
    m_viewerWidget->enableSynchronization(false);

    this->applyDisplayTransformations(m_viewerWidget, m_displaySet);

    if (!m_displaySet->getToolActivation().isEmpty()) // Tenim tools activades per defecte des del hanging protocol
    {
        if (m_displaySet->getToolActivation() == "synchronization") // S'activa la tool de sincronització
        {
            m_viewerWidget->enableSynchronization(true);
        }
    }
}

void HangingProtocolManager::applyDisplayTransformations(Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet)
{
    viewer->getViewer()->enableRendering(false);

    QString reconstruction = displaySet->getReconstruction();
    if (!reconstruction.isEmpty())
    {
        if (reconstruction == "SAGITAL")
        {
            viewer->getViewer()->resetViewToSagital();
        }
        else if (reconstruction == "CORONAL")
        {
            viewer->getViewer()->resetViewToCoronal();
        }
        else if (reconstruction == "AXIAL")
        {
            viewer->getViewer()->resetViewToAxial();
        }
        else
        {
            DEBUG_LOG("Field reconstruction in XML hanging protocol has an error");
        }
    }

    // Apliquem la orientació desitjada
    viewer->getViewer()->setImageOrientation(displaySet->getPatientOrientation());

    int phase = displaySet->getPhase();
    if (phase > -1)
    {
        viewer->getViewer()->setPhase(phase);
    }

    int sliceNumber = displaySet->getSlice();
    if (sliceNumber != -1)
    {
        //Comprovem si s'ha modificat el número de llesca pel fet de tenir la imatge dins un volum
        if (displaySet->getSliceModifiedForVolumes() != -1)
        {
            viewer->getViewer()->setSlice(displaySet->getSliceModifiedForVolumes());
        }
        else
        {
            viewer->getViewer()->setSlice(sliceNumber);
        }
    }

    QString alignment = displaySet->getAlignment();
    if (!alignment.isEmpty())
    {
        if (alignment == "right")
        {
            viewer->getViewer()->alignRight();
        }
        else if (alignment == "left")
        {
            viewer->getViewer()->alignLeft();
        }
    }
    else
    {
        viewer->getViewer()->setAlignPosition(Q2DViewer::AlignCenter);
    }

    viewer->getViewer()->enableRendering(true);
    viewer->getViewer()->render();
}

} // End namespace udg
