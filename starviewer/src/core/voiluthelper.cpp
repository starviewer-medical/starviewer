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

#include "voiluthelper.h"

#include "volume.h"
#include "volumehelper.h"
#include "voilutpresetstooldata.h"
#include "image.h"
#include "logging.h"

namespace udg {

const double VoiLutHelper::DefaultPETWindowWidthThreshold = 0.5;

VoiLutHelper::VoiLutHelper()
{
}

void VoiLutHelper::initializeVoiLutData(VoiLutPresetsToolData *voiLutData, Volume *volume)
{
    if (!volume)
    {
        return;
    }

    voiLutData->removePresetsFromGroup(VoiLutPresetsToolData::FileDefined);
    voiLutData->removePresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset);

    // Agafem el window level de la imatge central per evitar problemes
    // de que tinguem diferents windows levels a cada imatge i el de la
    // primera imatge sigui massa diferent a la resta. No deixa de ser un hack cutre.
    int index = volume->getNumberOfSlicesPerPhase() / 2;

    int voiLutCount = 0;
    Image *image = volume->getImage(index);
    if (image)
    {
        voiLutCount = image->getNumberOfVoiLuts();
    }
    else
    {
        DEBUG_LOG(QString("Índex [%1] fora de rang. No s'ha pogut obtenir la imatge indicada del volum actual.").arg(index));
    }

    if (voiLutCount > 0)
    {
        for (int i = 0; i < voiLutCount; i++)
        {
            VoiLut voiLut = getDefaultVoiLutForPresentation(image, i);
            voiLutData->addPreset(voiLut, VoiLutPresetsToolData::FileDefined);
        }
    }

    // Calculem un window level automàtic que sempre posarem disponible a l'usuari
    WindowLevel automaticWindowLevel = getCurrentAutomaticWindowLevel(volume);
    voiLutData->addPreset(automaticWindowLevel, VoiLutPresetsToolData::AutomaticPreset);

    selectDefaultPreset(voiLutData, volume);
}

VoiLut VoiLutHelper::getDefaultVoiLutForPresentation(Image *image, int index)
{
    if (!image)
    {
        return VoiLut();
    }

    VoiLut voiLut = image->getVoiLut(index);
    if (voiLut.isLut() || voiLut.getWindowLevel().isValid())
    {
        if (voiLut.getExplanation().isEmpty())
        {
            voiLut.setExplanation(getDefaultVoiLutDescription(index));
        }
    }

    return voiLut;
}

void VoiLutHelper::selectDefaultPreset(VoiLutPresetsToolData *voiLutData, Volume *volume)
{
    if (!voiLutData)
    {
        return;
    }

    QList<VoiLut> filePresets = voiLutData->getPresetsFromGroup(VoiLutPresetsToolData::FileDefined);

    // Usually we want the first file preset, if existent, as the default preset, but for primary PET volumes the automatic is preferred
    if (!filePresets.isEmpty() && !VolumeHelper::isPrimaryPET(volume))
    {
        voiLutData->setCurrentPreset(filePresets.first());
    }
    else
    {
        QList<VoiLut> automaticPresets = voiLutData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset);
        voiLutData->setCurrentPreset(automaticPresets.first());
    }
}

QString VoiLutHelper::getDefaultVoiLutDescription(int index)
{
    return QObject::tr("Default %1").arg(index);
}

WindowLevel VoiLutHelper::getCurrentAutomaticWindowLevel(Volume *volume)
{
    WindowLevel automaticWindowLevel;
    automaticWindowLevel.setName(QObject::tr("Auto"));

    if (volume)
    {
        double range[2];
        volume->getScalarRange(range);
        
        double windowWidth = range[1] - range[0];
        if (VolumeHelper::isPrimaryPET(volume) || VolumeHelper::isPrimaryNM(volume))
        {
            windowWidth *= DefaultPETWindowWidthThreshold;
        }

        double windowCenter = range[0] + (windowWidth * 0.5);

        automaticWindowLevel.setWidth(windowWidth);
        automaticWindowLevel.setCenter(windowCenter);
    }
    else
    {
        DEBUG_LOG("No input to compute automatic ww/wl. Undefined values.");
    }

    return automaticWindowLevel;
}

} // namespace udg
