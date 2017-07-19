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
 * @brief Application about dialog.
 *
 * This application dialog shows a hidden button to crash the application on purpose,
 * to show it the user must hold the Close button for a significant amount of time.
 */
class QAboutDialog : public QDialog, private Ui::QAboutDialogBase {
Q_OBJECT
public:
    explicit QAboutDialog(QWidget *parent = nullptr);
    virtual ~QAboutDialog();

public slots:
    /// Overridden to avoid closing the dialog just after showing the crash button.
    void reject() override;

private slots:
    /// Shows a dialog with the license information.
    void showLicenseInformation();

    /// Crashes the application.
    void crash();

    /// Registers the time to later check if enough time has passed to show the crash button.
    void onCloseButtonPressed();
    /// Computes how much time has passed since the close button was pressed and if it's over a certain threshold shows the crash button.
    void onCloseButtonReleased();

private:
    /// The crash button, hidden at start.
    QPushButton *m_crashButton;
    /// Time (milliseconds from epoch) when the close button was pressed.
    qint64 m_longClickStart;
    /// True if the dialog should not close, false otherwise.
    bool m_dontClose;

};

}

#endif
