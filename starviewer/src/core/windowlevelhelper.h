#ifndef WINDOWLEVELHELPER_H
#define WINDOWLEVELHELPER_H

#include "windowlevel.h"

namespace udg {

class WindowLevelPresetsToolData;
class Volume;
class Image;

class WindowLevelHelper {
public:
    WindowLevelHelper();

    /// Initialize the window level data according to the given volume
    void initializeWindowLevelData(WindowLevelPresetsToolData *windowLevelData, Volume *volume);

    /// Gets the n-th default window level from the given image and index, prepared for display,
    /// i.e. if image is MONOCHROME1, it will invert values and give a proper name if no description is available
    /// If index is out of range, a non-valid WindowLevel will be returned
    WindowLevel getDefaultWindowLevelForPresentation(Image *image, int index);

    /// Selects the default preset to apply on the given window level data. File defined presets come first, then automatic.
    static void selectDefaultPreset(WindowLevelPresetsToolData *windowLevelData);

private:
    /// Computes the automatic window level for the current input
    WindowLevel getCurrentAutomaticWindowLevel(Volume *volume);

    /// Gets a default name for the specified n-th window level. Used to give a default name for window levels without description.
    QString getDefaultWindowLevelDescription(int index);
};

}

#endif // WINDOWLEVELHELPER_H
