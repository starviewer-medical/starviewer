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

    /// Returns true if the given viewers meet the specified criteria
    bool isCriterionSatisfied(QViewer *viewer1, QViewer *viewer2);

protected:
    /// To be implemented by each subclass
    virtual bool criterionIsMet(QViewer *viewer1, QViewer *viewer2) = 0;
};

} // End namespace udg

#endif
