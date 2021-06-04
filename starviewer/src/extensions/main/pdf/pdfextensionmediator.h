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

#ifndef UDG_PDFEXTENSIONMEDIATOR_H
#define UDG_PDFEXTENSIONMEDIATOR_H

#include "extensionmediator.h"

#include "installextension.h"
#include "qpdfextension.h"

namespace udg {

/**
 * @brief The PdfExtensionMediator class is the mediator for the PDF extension.
 */
class PdfExtensionMediator : public ExtensionMediator {

    Q_OBJECT

public:

    explicit PdfExtensionMediator(QObject *parent = nullptr);
    ~PdfExtensionMediator() override;

    bool initializeExtension(QWidget *extension, const ExtensionContext &extensionContext) override;
    DisplayableID getExtensionID() const override;
    DicomEntityFlags getSupportedDicomEntities() const override;

};

static InstallExtension<QPdfExtension, PdfExtensionMediator> registerPdfExtension;

} // namespace udg

#endif // UDG_PDFEXTENSIONMEDIATOR_H
