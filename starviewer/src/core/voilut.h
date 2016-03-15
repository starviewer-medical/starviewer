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

#ifndef UDG_VOILUT_H
#define UDG_VOILUT_H

#include "transferfunction.h"
#include "windowlevel.h"

namespace udg {

/**
 * @brief The VoiLut class represents the general concept of a VOI LUT. It can be either a simple window/level or a true LUT.
 */
class VoiLut
{
public:
    /// Creates a VOI LUT that represents a default WindowLevel.
    VoiLut();
    /// Creates a VOI LUT that represents the given window level.
    VoiLut(const WindowLevel &windowLevel);
    /// Creates a VOI LUT that represents the given LUT.
    VoiLut(const TransferFunction &lut);
    /// Creates a VOI LUT that represents the given LUT that was obtained from a window level operation to another LUT,
    /// whose explanation (or name) is given in the second parameter.
    VoiLut(const TransferFunction &lut, const QString &originalExplanation);

    /// Returns the window level contained in this VOI LUT. If this VOI LUT is indeed a LUT the returned window level is defined as explained in setLut().
    const WindowLevel& getWindowLevel() const;
    /// Makes this VOI LUT represent the given window level. The LUT is cleared.
    void setWindowLevel(const WindowLevel &windowLevel);

    /// Returns the LUT contained in this VOI LUT. If this VOI LUT is indeed a window level, an empty LUT is returned.
    const TransferFunction& getLut() const;
    /// Makes this VOI LUT represent the given LUT.
    /// The window level is set with a width equal to the last key minus the first key and a center equal to the mean of the first and last keys.
    void setLut(const TransferFunction &lut);

    /// Returns the explanation (or name) of this VOI LUT.
    const QString& getExplanation() const;
    /// Sets the explanation (or name) of this VOI LUT.
    void setExplanation(const QString &explanation);

    /// Returns the explanation (or name) of the LUT that was window-leveled to create this LUT. If this LUT is original, returns the same as getExplanation().
    const QString& getOriginalLutExplanation() const;

    /// Returns true if this VOI LUT represents a window level.
    bool isWindowLevel() const;
    /// Returns true if this VOI LUT represents a LUT.
    bool isLut() const;

    /// Returns true if this VOI LUT is equal to the given one, i.e. if it has the same window level and the same LUT.
    bool operator ==(const VoiLut &that) const;
    /// Returns true if this VOI LUT is not equal to the given one.
    bool operator !=(const VoiLut &that) const;

    /// Returns a VOI LUT that is the inverse of this one, with the same explanation (or name).
    VoiLut inverse() const;

private:
    /// The window level contained in this VOI LUT. If this VOI LUT represents a LUT the window level is defined as explained in setLut().
    WindowLevel m_windowLevel;
    /// The LUT contained in this VOI LUT. If this VOI LUT represents a window level the LUT is empty.
    TransferFunction m_lut;
    /// When a new LUT is created by applying a window level operation to another one, this will store the explanation (or name) of the original LUT.
    /// Otherwise, it will store the same explation (or name).
    QString m_originalLutExplanation;

};

} // namespace udg

#endif // UDG_VOILUT_H
