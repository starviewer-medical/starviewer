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

#ifndef MPREXTENSIONMEDIATOR_H
#define MPREXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qmprextension.h"

namespace udg {

class MPRExtensionMediator : public ExtensionMediator {
Q_OBJECT
public:
    MPRExtensionMediator(QObject *parent = 0);

    ~MPRExtensionMediator();

    virtual bool initializeExtension(QWidget *extension, const ExtensionContext &extensionContext);

    virtual DisplayableID getExtensionID() const;
};

static InstallExtension<QMPRExtension, MPRExtensionMediator> registerMPRExtension;

} // End udg namespace

#endif
