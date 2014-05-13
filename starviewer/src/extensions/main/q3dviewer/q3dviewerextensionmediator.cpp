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

#include "q3dviewerextensionmediator.h"

#include "extensioncontext.h"

// QT
#include <QMessageBox>

namespace udg {

Q3DViewerExtensionMediator::Q3DViewerExtensionMediator(QObject *parent)
    : ExtensionMediator(parent)
{
}

Q3DViewerExtensionMediator::~Q3DViewerExtensionMediator()
{
}

DisplayableID Q3DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Q3DViewerExtension", tr("3D Viewer"));
}

bool Q3DViewerExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    Q3DViewerExtension *q3DViewerExtension;

    if (!(q3DViewerExtension = qobject_cast<Q3DViewerExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if (!input)
    {
        QMessageBox::information(0, tr("Starviewer"), tr("The selected item is not an image"));
    }
    else
    {
        q3DViewerExtension->setInput(input);
    }

    return true;
}

}
