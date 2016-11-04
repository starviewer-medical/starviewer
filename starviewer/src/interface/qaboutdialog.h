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
  @brief Application about dialog.

  This application dialog shows a hidden button to crash the application on purpose, to show it the user must hold and release the Ok button for a significant
  amount of time.

  */
class QAboutDialog : public QDialog, private Ui::QAboutDialogBase {
Q_OBJECT
public:
    QAboutDialog(QWidget *parent = 0);
    ~QAboutDialog();

private slots:
    /// Shows a dialog with the license information.
    void showLicenseInformation();
    void btnCrashClicked();
    void btnOkClicked();
    void btnOkPressed();
    void btnOkReleased();

private:
    static constexpr unsigned int msecsToShowCrash = 5000;
    qint64 m_longClickStart;
    QPushButton* m_crashBtn;
    QPushButton* m_okBtn;

};

}

#endif
