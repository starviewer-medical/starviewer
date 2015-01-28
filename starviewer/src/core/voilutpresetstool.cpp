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

#include "voilutpresetstool.h"
#include "q2dviewer.h"
#include "logging.h"
#include "voilutpresetstooldata.h"

// Vtk
#include <vtkCommand.h>
#include <vtkRenderWindowInteractor.h>

namespace udg {

VoiLutPresetsTool::VoiLutPresetsTool(QViewer *viewer, QObject *parent)
 : Tool(viewer, parent), m_myToolData(0), m_defaultPresetsIndex(0)
{
    m_toolName = "VoiLutPresetsTool";

    setToolData(m_viewer->getVoiLutData());
    m_characterIndexMap.insert('1', 0);
    m_characterIndexMap.insert('2', 1);
    m_characterIndexMap.insert('3', 2);
    m_characterIndexMap.insert('4', 3);
    m_characterIndexMap.insert('5', 4);
    m_characterIndexMap.insert('6', 5);
    m_characterIndexMap.insert('7', 6);
    m_characterIndexMap.insert('8', 7);
    m_characterIndexMap.insert('9', 8);
    m_characterIndexMap.insert('0', 9);
    // TODO això ara "va bé" en un teclat estàndard espanyol, però si el layout és per exemple anglès
    // té molts números per no funcionar (és a dir, les tecles estaran repartides per altres parts del teclat)

    // Shift + 1
    m_characterIndexMap.insert(33, 10);
    // Shift + 2
    m_characterIndexMap.insert(34, 11);
    // Shift + 3
    m_characterIndexMap.insert(-73, 12);
    // Shift + 4
    m_characterIndexMap.insert(36, 13);
    // Shift + 5
    m_characterIndexMap.insert(37, 14);
    // Shift + 6
    m_characterIndexMap.insert(38, 15);
    // Shift + 7
    m_characterIndexMap.insert(47, 16);
    // Shift + 8
    m_characterIndexMap.insert(40, 17);
    // Shift + 9
    m_characterIndexMap.insert(41, 18);
    // Shift + 0
    m_characterIndexMap.insert(61, 19);

    // Cada cop que es canvïi el volum cal actualitzar la llista de ww/wl per defecte d'aquell volum (definits al DICOM)
    connect(viewer, SIGNAL(volumeChanged(Volume*)), SLOT(updateVoiLutData()));
}

VoiLutPresetsTool::~VoiLutPresetsTool()
{
}

void VoiLutPresetsTool::handleEvent(unsigned long eventID)
{
    switch (eventID)
    {
        case vtkCommand::KeyPressEvent:
            // Cal controlar que les tecles Ctrl i Alt no estiguin pitjades, sinó ens podria activar un preset no desitjat
            // TODO Vtk només és capaç de detectar AltGr com a tecla Alt. La tecla Alt per sí sola no la detecta.
            if (!m_viewer->getInteractor()->GetControlKey() && !m_viewer->getInteractor()->GetAltKey())
            {
                applyPreset(m_viewer->getInteractor()->GetKeyCode());
            }
            break;

        default:
            break;
    }
}

void VoiLutPresetsTool::applyPreset(char key)
{
    if (m_standardPresets.isEmpty())
    {
        return;
    }

    QString preset;

    if (!m_characterIndexMap.contains(key))
    {
        // TODO de moment fem servir la tecla "º", que en un teclat espanyol queda a l'esquerra del nº 1, però en altres teclats pot estar a qualsevol lloc
        if (key == -70)
        {
            if (!m_defaultPresets.isEmpty())
            {
                if (m_defaultPresets.contains(m_myToolData->getCurrentPresetName()))
                {
                    m_defaultPresetsIndex = (m_defaultPresets.indexOf(m_myToolData->getCurrentPresetName()) + 1) % m_defaultPresets.count();
                }
                else
                {
                    m_defaultPresetsIndex = 0;
                }

                preset = m_defaultPresets.at(m_defaultPresetsIndex);

                DEBUG_LOG(QString("default Preset index: %1").arg(m_defaultPresetsIndex));
            }
        }
    }
    else
    {
        int presetIndex = m_characterIndexMap.value(key);
        if (presetIndex < m_standardPresets.count())
        {
            preset = m_standardPresets.at(presetIndex);
            m_defaultPresetsIndex = 0;
        }
    }

    m_myToolData->selectPreset(preset);
}

void VoiLutPresetsTool::updateVoiLutData()
{
    m_defaultPresetsIndex = 0;
    if (!m_myToolData)
    {
        DEBUG_LOG("No tenim tooldata de window level!");
    }
    else
    {
        m_defaultPresets.clear();
        m_defaultPresets = m_myToolData->getDescriptionsFromGroup(VoiLutPresetsToolData::FileDefined) +
                           m_myToolData->getDescriptionsFromGroup(VoiLutPresetsToolData::AutomaticPreset);
    }
}

void VoiLutPresetsTool::setToolData(ToolData *toolData)
{
    m_toolData = toolData;
    m_myToolData = qobject_cast<VoiLutPresetsToolData*>(toolData);
    if (!m_myToolData)
    {
        DEBUG_LOG("El tooldata proporcionat no és un WindwoLevelPresetsToolData que és l'esperat");
    }
    else
    {
        m_standardPresets.clear();
        m_standardPresets = m_myToolData->getDescriptionsFromGroup(VoiLutPresetsToolData::StandardPresets);

        m_defaultPresets.clear();
        m_defaultPresets = m_myToolData->getDescriptionsFromGroup(VoiLutPresetsToolData::FileDefined) +
                           m_myToolData->getDescriptionsFromGroup(VoiLutPresetsToolData::AutomaticPreset);
    }
}

}
