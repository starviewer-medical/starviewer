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

#ifndef UDGSYNCCRITERION_H
#define UDGSYNCCRITERION_H

namespace udg {

class QViewer;

/**
    Interface to define a criterion for synchronization or propagation purposes.
    Each subclass must implement criterionIsMet() method which tells whether 
    the criterion is met between two viewers or not
 */
class SyncCriterion {
public:
    SyncCriterion();
    virtual ~SyncCriterion();

    /// Returns true when the criterion to propagate from \a sourceViewer to \a targetViewer is met and false otherwise.
    /// It checks that both viewers exist and have input and then calls \ref criterionIsMet.
    bool isCriterionSatisfied(QViewer *sourceViewer, QViewer *targetViewer);

protected:
    /// Returns true when the criterion to propagate from \a sourceViewer to \a targetViewer is met and false otherwise. Must be implemented by each subclass.
    virtual bool criterionIsMet(QViewer *sourceViewer, QViewer *targetViewer) = 0;
};

} // End namespace udg

#endif
