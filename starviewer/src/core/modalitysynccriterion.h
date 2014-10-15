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

#ifndef UDGMODALITYSYNCCRITERION_H
#define UDGMODALITYSYNCCRITERION_H

#include "synccriterion.h"

class QStringList;

namespace udg {

class Q2DViewer;

/**
    Implements the SyncCriterion for the viewers input.
    The inputs have to be of the same modality to meet the criterion.
    The exception comes with the PT-CT case. In case we have PT and CT modalities we consider
    the criterion is met due to the case of PET-CT fusion.
 */
class ModalitySyncCriterion : public SyncCriterion {
public:
    ModalitySyncCriterion();
    ~ModalitySyncCriterion();

protected:
    bool criterionIsMet(QViewer *viewer1, QViewer *viewer2);

private:
    /// Returns the modalities of the viewer inputs
    QStringList getModalities(Q2DViewer *viewer) const;

    /// Checks whether we are in the case of a combination of PET-CT or NM-CT
    bool isFusionCase(const QStringList &modalities1, const QStringList &modalities2) const;
};

} // End namespace udg

#endif
