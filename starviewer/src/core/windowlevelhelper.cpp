#include "windowlevelhelper.h"

#include "volume.h"
#include "windowlevelpresetstooldata.h"
#include "image.h"
#include "logging.h"

namespace udg {

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

            if (i == 0)
            {
                windowLevelData->selectCurrentPreset(windowLevel.getName());
            }
        }
    }

    // Calculem un window level automàtic que sempre posarem disponible a l'usuari
    WindowLevel automaticWindowLevel = getCurrentAutomaticWindowLevel(volume);
    windowLevelData->addPreset(automaticWindowLevel, WindowLevelPresetsToolData::AutomaticPreset);
    // Si no hi ha window levels definits per defecte activarem l'automàtic
    if (windowLevelCount <= 0)
    {
        windowLevelData->selectCurrentPreset(automaticWindowLevel.getName());
    }
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

void WindowLevelHelper::selectDefaultPreset(WindowLevelPresetsToolData *windowLevelData)
{
    if (!windowLevelData)
    {
        return;
    }

    QList<WindowLevel> filePresets = windowLevelData->getPresetsFromGroup(WindowLevelPresetsToolData::FileDefined);

    if (!filePresets.isEmpty())
    {
        windowLevelData->setCurrentPreset(filePresets.first());
    }
    else
    {
        QList<WindowLevel> automaticPresets = windowLevelData->getPresetsFromGroup(WindowLevelPresetsToolData::AutomaticPreset);

        if (!automaticPresets.isEmpty())
        {
            windowLevelData->setCurrentPreset(automaticPresets.first());
        }
    }
}

QString WindowLevelHelper::getDefaultWindowLevelDescription(int index)
{
    const QString DefaultWindowLevelName = QString("Default");
    return QString("%1 %2").arg(DefaultWindowLevelName).arg(index);
}

WindowLevel WindowLevelHelper::getCurrentAutomaticWindowLevel(Volume *volume)
{
    WindowLevel automaticWindowLevel;
    automaticWindowLevel.setName(QString("Auto"));

    if (volume)
    {
        double range[2];
        volume->getScalarRange(range);
        automaticWindowLevel.setWidth(range[1] - range[0]);
        automaticWindowLevel.setCenter(range[0] + (automaticWindowLevel.getWidth() * 0.5));
    }
    else
    {
        DEBUG_LOG("No input to compute automatic ww/wl. Undefined values.");
    }

    return automaticWindowLevel;
}

} // namespace udg
