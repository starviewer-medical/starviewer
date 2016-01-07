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

#ifndef Q2DVIEWEREXTENSIONMEDIATOR_H
#define Q2DVIEWEREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "q2dviewerextension.h"

namespace udg {

class Q2DViewerExtensionMediator : public ExtensionMediator {
Q_OBJECT
public:
    Q2DViewerExtensionMediator(QObject *parent = 0);
    ~Q2DViewerExtensionMediator();

    virtual bool initializeExtension(QWidget *extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;

    /// Orders the extension to view newly loaded studies from the current patient.
    virtual void viewNewStudiesFromSamePatient(QWidget *extension, const QString &newStudyUID);

};

static InstallExtension<Q2DViewerExtension, Q2DViewerExtensionMediator> registerQ2DViewerExtension;

} // End udg namespace

#endif
