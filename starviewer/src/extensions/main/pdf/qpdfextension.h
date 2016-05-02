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

#ifndef UDG_QPDFEXTENSION_H
#define UDG_QPDFEXTENSION_H

#include <QWidget>
#include "ui_qpdfextensionbase.h"

namespace udg {

class Patient;

/**
 * @brief QPdfExtension is an extension that allows to open in the default PDF reader the encapsulated PDFs contained in a Patient.
 */
class QPdfExtension : public QWidget, private ::Ui::QPdfExtensionBase {

    Q_OBJECT

public:

    explicit QPdfExtension(QWidget *parent = 0);
    virtual ~QPdfExtension();

    /// Sets the given patient to the extension. All the PDF documents in this patient are shown.
    void setPatient(Patient *patient);

protected:
    virtual void showEvent(QShowEvent *event) override;

private slots:

    /// Desencapsulates and opens the PDF associated with the given item.
    void openPdf(QListWidgetItem *item);

};

} // namespace udg

#endif // UDG_QPDFEXTENSION_H
