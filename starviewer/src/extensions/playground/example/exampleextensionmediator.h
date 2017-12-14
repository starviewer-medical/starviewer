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

#ifndef UDG_EXAMPLEEXTENSIONMEDIATOR_H
#define UDG_EXAMPLEEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qexampleextension.h"

namespace udg {

class ExampleExtensionMediator : public ExtensionMediator {

    Q_OBJECT

public:

    explicit ExampleExtensionMediator(QObject *parent = nullptr);

    bool initializeExtension(QWidget *extension, const ExtensionContext &extensionContext) override;
    DisplayableID getExtensionID() const override;

};

static InstallExtension<QExampleExtension, ExampleExtensionMediator> registerExampleExtension;

} // namespace udg

#endif // UDG_EXAMPLEEXTENSIONMEDIATOR_H
