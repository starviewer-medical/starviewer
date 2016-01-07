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

#ifndef VOILUTHELPER_H
#define VOILUTHELPER_H

class QString;

namespace udg {

class Image;
class VoiLut;
class VoiLutPresetsToolData;
class Volume;
class WindowLevel;

class VoiLutHelper {
public:
    VoiLutHelper();

    /// Initialize the VOI LUT data according to the given volume.
    void initializeVoiLutData(VoiLutPresetsToolData *voiLutData, Volume *volume);

    /// Returns the n-th default VOI LUT from the given image and index. If index is out of range, a default VoiLut will be returned.
    VoiLut getDefaultVoiLutForPresentation(Image *image, int index);

    /// Selects the default preset to apply on the given VOI LUT data corresponding to the given volume.
    static void selectDefaultPreset(VoiLutPresetsToolData *voiLutData, Volume *volume);

private:
    /// Computes the automatic window level for the current input
    WindowLevel getCurrentAutomaticWindowLevel(Volume *volume);

    /// Gets a default name for the specified n-th VOI LUT. Used to give a default name for VOI LUTs without description.
    QString getDefaultVoiLutDescription(int index);

private:
    /// Default threshold to apply to window width on PET images
    static const double DefaultPETWindowWidthThreshold;
};

}

#endif // VOILUTHELPER_H
