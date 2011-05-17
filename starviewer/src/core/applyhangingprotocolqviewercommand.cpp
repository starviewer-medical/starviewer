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

    this->applyDisplayTransformations(m_viewerWidget->getViewer(), m_displaySet);

    if (!m_displaySet->getToolActivation().isEmpty())
    {
        // Tenim tools activades per defecte des del hanging protocol
        if (m_displaySet->getToolActivation() == "synchronization")
        {
            // S'activa la tool de sincronització
            m_viewerWidget->enableSynchronization(true);
        }
    }
}

void ApplyHangingProtocolQViewerCommand::applyDisplayTransformations(Q2DViewer *viewer, HangingProtocolDisplaySet *displaySet)
{
    viewer->enableRendering(false);

    QString reconstruction = displaySet->getReconstruction();
    if (!reconstruction.isEmpty())
    {
        if (reconstruction == "SAGITAL")
        {
            viewer->resetViewToSagital();
        }
        else if (reconstruction == "CORONAL")
        {
            viewer->resetViewToCoronal();
        }
        else if (reconstruction == "AXIAL")
        {
            viewer->resetViewToAxial();
        }
        else
        {
            DEBUG_LOG("Field reconstruction in XML hanging protocol has an error");
        }
    }

    int phase = displaySet->getPhase();
    if (phase > -1)
    {
        viewer->setPhase(phase);
    }

    // Comprovem si s'ha modificat el número de llesca pel fet de tenir la imatge dins un volum
    // Si s'ha aplicat una reconstrucció, aquest número de llesca no la tenim en compte
    if (displaySet->getSliceModifiedForVolumes() != -1 && reconstruction.isEmpty())
    {
        viewer->setSlice(displaySet->getSliceModifiedForVolumes());
    }
    else if (displaySet->getSlice() != -1)
    {
        viewer->setSlice(displaySet->getSlice());
    }

    // Apliquem la orientació desitjada
    viewer->setImageOrientation(displaySet->getPatientOrientation());
    QString alignment = displaySet->getAlignment();
    if (!alignment.isEmpty())
    {
        if (alignment == "right")
        {
            viewer->alignRight();
        }
        else if (alignment == "left")
        {
            viewer->alignLeft();
        }
    }
    else
    {
        viewer->setAlignPosition(Q2DViewer::AlignCenter);
    }

    if (displaySet->getWindowCenter() != -1 && displaySet->getWindowWidth() != -1)
    {
        viewer->setWindowLevel(displaySet->getWindowWidth(), displaySet->getWindowCenter());
    }

    viewer->enableRendering(true);
    viewer->render();
}

} // End namespace udg
