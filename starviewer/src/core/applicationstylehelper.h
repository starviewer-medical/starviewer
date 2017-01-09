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

#ifndef UDGAPPLICATIONSTYLEHELPER_H
#define UDGAPPLICATIONSTYLEHELPER_H


namespace udg {

/**
  Returns system's default font sizes efficiently by the querying system only one time.
*/

class ApplicationStyleHelper {
public:
    /// If true, the results in Pt are multiplied by the Qt's scale factor.
    /// This should be true when you know that contents are not scaled by this factor, for example in VTK (at least in 7.0).
    ApplicationStyleHelper(bool applyScaleFactor = false);

    /// Returns default font size in Pt multiplied by an scale factor.
    int getScaledFontSize(double multiplier) const;

    /// Returns font size in Pt.
    int getToolsFontSize() const;

    /// Returns font size in Pt.
    int getCornerAnnotationFontSize() const;

    /// Returns font size in Pt.
    int getOrientationAnnotationFontSize() const;


    /// Returns font size in Pt.
    int getDefaultFontSize() const;

private:
    static double m_systemFontSize;
    bool m_applyScaleFactor;
};

} // End namespace udg

#endif // UDGAPPLICATIONSTYLEHELPER_H
