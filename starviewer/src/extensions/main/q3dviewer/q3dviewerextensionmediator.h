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

#ifndef UDGQ3DVIEWEREXTENSIONMEDIATOR_H
#define UDGQ3DVIEWEREXTENSIONMEDIATOR_H

#include "extensionmediator.h"
#include "installextension.h"
#include "q3dviewerextension.h"

namespace udg {

/**
    \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
  */
class Q3DViewerExtensionMediator : public ExtensionMediator {
Q_OBJECT
public:
    Q3DViewerExtensionMediator(QObject *parent = 0);
    ~Q3DViewerExtensionMediator();

    virtual DisplayableID getExtensionID() const;

    virtual bool initializeExtension(QWidget *extension, const ExtensionContext &extensionContext);

};

static InstallExtension<Q3DViewerExtension, Q3DViewerExtensionMediator> registerQ3DViewerExtensionMediator;

}

#endif
