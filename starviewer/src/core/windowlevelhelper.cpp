#include "windowlevelhelper.h"

#include "volume.h"
#include "volumehelper.h"
#include "windowlevelpresetstooldata.h"
#include "image.h"
#include "logging.h"

namespace udg {

const double WindowLevelHelper::DefaultPETWindowWidthThreshold = 0.5;

WindowLevelHelper::WindowLevelHelper()
{
}

void WindowLevelHelper::initializeWindowLevelData(WindowLevelPresetsToolData *windowLevelData, Volume *volume)
{
    if (!volume)
    {
        return;
    }

    windowLevelData->removePresetsFromGroup(WindowLevelPresetsToolData::FileDefined);
    windowLevelData->removePresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset);

    // Agafem el window level de la imatge central per evitar problemes
    // de que tinguem diferents windows levels a cada imatge i el de la
    // primera imatge sigui massa diferent a la resta. No deixa de ser un hack cutre.
    int index = volume->getNumberOfSlicesPerPhase() / 2;

    int windowLevelCount = 0;
    Image *image = volume->getImage(index);
    if (image)
    {
        windowLevelCount = image->getNumberOfWindowLevels();
    }
    else
    {
        DEBUG_LOG(QString("Índex [%1] fora de rang. No s'ha pogut obtenir la imatge indicada del volum actual.").arg(index));
    }

    if (windowLevelCount > 0)
    {
        for (int i = 0; i < windowLevelCount; i++)
        {
            WindowLevel windowLevel = getDefaultWindowLevelForPresentation(image, i);
            windowLevelData->addPreset(windowLevel, WindowLevelPresetsToolData::FileDefined);
        }
    }

    // Calculem un window level automàtic que sempre posarem disponible a l'usuari
    WindowLevel automaticWindowLevel = getCurrentAutomaticWindowLevel(volume);
    windowLevelData->addPreset(automaticWindowLevel, WindowLevelPresetsToolData::AutomaticPreset);

    selectDefaultPreset(windowLevelData, volume);
}

WindowLevel WindowLevelHelper::getDefaultWindowLevelForPresentation(Image *image, int index)
{
    if (!image)
    {
        return WindowLevel();
    }

    WindowLevel windowLevel = image->getWindowLevel(index);
    if (windowLevel.isValid())
    {
        if (image->getPhotometricInterpretation() == PhotometricInterpretation::Monochrome1)
        {
            windowLevel.setWidth(-windowLevel.getWidth());
        }

        if (windowLevel.getName().isEmpty())
        {
            windowLevel.setName(getDefaultWindowLevelDescription(index));
        }
    }

    return windowLevel;
}

void WindowLevelHelper::selectDefaultPreset(WindowLevelPresetsToolData *windowLevelData, Volume *volume)
{
    if (!windowLevelData)
    {
        return;
    }

    QList<WindowLevel> filePresets = windowLevelData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined);

    // Usually we want the first file preset, if existent, as the default preset, but for primary PET volumes the automatic is preferred
    if (!filePresets.isEmpty() && !VolumeHelper::isPrimaryPET(volume))
    {
        windowLevelData->setCurrentPreset(filePresets.first());
    }
    else
    {
        QList<WindowLevel> automaticPresets = windowLevelData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset);
        windowLevelData->setCurrentPreset(automaticPresets.first());
    }
}

QString WindowLevelHelper::getDefaultWindowLevelDescription(int index)
{
    return QObject::tr("Default %1").arg(index);
}

WindowLevel WindowLevelHelper::getCurrentAutomaticWindowLevel(Volume *volume)
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

        // If MONOCHROME1 image, window/level is inverted
        Image *image = volume->getImage(0);
        if (image && image->getPhotometricInterpretation() == PhotometricInterpretation::Monochrome1)
        {
            windowWidth = -windowWidth;
        }
        
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
