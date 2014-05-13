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

#include "dicomprintextensionmediator.h"

#include "extensioncontext.h"
#include "series.h"
#include "volume.h"
#include "image.h"
#include "starviewerapplication.h"

// QT
#include <QMessageBox>

namespace udg {

DicomPrintExtensionMediator::DicomPrintExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

DicomPrintExtensionMediator::~DicomPrintExtensionMediator()
{
}

DisplayableID DicomPrintExtensionMediator::getExtensionID() const
{
    return DisplayableID("DicomPrintExtension", tr("DICOM Print"));
}

bool DicomPrintExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    QDicomPrintExtension *dicomPrintExtension;

    if (!(dicomPrintExtension = qobject_cast<QDicomPrintExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    if (!input)
    {
        QMessageBox::information(0, udg::ApplicationNameString, tr("The selected item is not an image"));
    }
    else
    {
        dicomPrintExtension->setInput(input);
    }

    return true;
}
}
