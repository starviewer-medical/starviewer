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

#include "exampleextensionmediator.h"

#include "extensioncontext.h"

namespace udg {

ExampleExtensionMediator::ExampleExtensionMediator(QObject *parent)
    : ExtensionMediator(parent)
{
}

bool ExampleExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    QExampleExtension *exampleExtension;

    if (!(exampleExtension = qobject_cast<QExampleExtension*>(extension)))
    {
        return false;
    }

    exampleExtension->setPatient(extensionContext.getPatient());

    return true;
}

DisplayableID ExampleExtensionMediator::getExtensionID() const
{
    return DisplayableID("ExampleExtension", tr("Example"));
}

} // namespace udg
