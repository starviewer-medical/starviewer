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

#ifndef UDG_QPACSDIALOG_H
#define UDG_QPACSDIALOG_H

#include <QDialog>
#include "ui_qpacsdialogbase.h"

#include "pacsdevice.h"

namespace udg {

/**
 * @brief The QPacsDialog class allows the user to edit the settings of a PACS server.
 */
class QPacsDialog : public QDialog, private ::Ui::QPacsDialogBase
{
    Q_OBJECT
public:
    /// Creates the dialog with empty fields, to add a new PACS of the given type.
    explicit QPacsDialog(PacsDevice::Type type, QWidget *parent = nullptr);
    /// Creates the dialog and fills it with the information of the PACS with the given id.
    explicit QPacsDialog(QString pacsId, QWidget *parent = nullptr);

private:
    /// Common initialization for both constructors.
    void initialize();

    /// Leaves the dialog ready to configure a DIMSE PACS.
    void setupDimse();

    /// Leaves the dialog ready to configure a WADO PACS.
    void setupWado();

    /// Creates the needed connections.
    void createConnections();

    /// Returns true if the current values in all fields are valid and false otherwise.
    bool arePacsSettingsValid();

    /// Creates and returns a PacsDevice with the current values in the dialog.
    PacsDevice getPacsDevice() const;

private slots:
    /// Tests the PACS defined by the current values in the dialog with a C-ECHO.
    void test();

    /// Calls save() and if that returns true closes the dialog.
    void accept() override;

    /// If the current values are valid, saves the PACS (adding or updating it). Otherwise, shows a warning. Returns true if the PACS was correctly saved.
    bool save();

    /// Resets the values of all fields to their initial values: empty for a new PACS and the values from settings when editing one.
    void reset();

private:
    /// Type for a new PACS.
    PacsDevice::Type m_pacsType;
    /// Id of the edited PACS, if any.
    QString m_pacsId;
};

} // namespace udg

#endif // UDG_QPACSDIALOG_H
