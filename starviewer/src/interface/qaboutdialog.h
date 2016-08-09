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

#ifndef UDGQABOUTDIALOG_H
#define UDGQABOUTDIALOG_H

#include "ui_qaboutdialogbase.h"

namespace udg {

/**
    Diàleg d'about de l'aplicació
  */
class QAboutDialog : public QDialog, private Ui::QAboutDialogBase {
Q_OBJECT
public:
    QAboutDialog(QWidget *parent = 0);
    ~QAboutDialog();

private slots:
    /// Shows a dialog with the license information.
    void showLicenseInformation();
    void on_btnCrashClicked();

};

}

#endif
