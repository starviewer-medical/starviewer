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

#include "q2dviewerextensionmediator.h"

#include "extensioncontext.h"
#include "starviewerapplication.h"
// QT
#include <QMessageBox>

namespace udg {

Q2DViewerExtensionMediator::Q2DViewerExtensionMediator(QObject *parent)
 : ExtensionMediator(parent)
{
}

Q2DViewerExtensionMediator::~Q2DViewerExtensionMediator()
{
}

DisplayableID Q2DViewerExtensionMediator::getExtensionID() const
{
    return DisplayableID("Q2DViewerExtension", tr("2D Viewer"));
}

bool Q2DViewerExtensionMediator::initializeExtension(QWidget *extension, const ExtensionContext &extensionContext)
{
    Q2DViewerExtension *q2dviewerExtension;

    if (!(q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension)))
    {
        return false;
    }

    Volume *input = extensionContext.getDefaultVolume();
    // Si no tenim cap volum, alertem a l'usuari
    if (!input)
    {
        QMessageBox::information(0, ApplicationNameString, tr("There is not any viewable or supported Series in the selected Studies"));
    }

    q2dviewerExtension->setPatient(extensionContext.getPatient());
#ifdef STARVIEWER_LITE
    // Assignem l'input per defecte al visor
    q2dviewerExtension->setInput(input);
#endif

    return true;
}

void Q2DViewerExtensionMediator::viewNewStudiesFromSamePatient(QWidget *extension, const QString &newStudyUID)
{
    if (Q2DViewerExtension *q2dviewerExtension = qobject_cast<Q2DViewerExtension*>(extension))
    {
        q2dviewerExtension->setCurrentStudy(newStudyUID);
    }
}

} // End udg namespace
