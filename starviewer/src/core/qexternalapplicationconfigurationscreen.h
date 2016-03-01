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

#ifndef QEXTERNALAPPLICATIONCONFIGURATIONSCREEN_H
#define QEXTERNALAPPLICATIONCONFIGURATIONSCREEN_H

#include "ui_qexternalapplicationconfigurationscreenbase.h"

namespace udg {

/**
    @brief External application configuration GUI

    This GUI is used to edit the external applications list. When properly
    configured, the entries set here appear on a menu.
 */
class QExternalApplicationConfigurationScreen : public QWidget, private Ui::QExternalApplicationConfigurationScreenBase {
Q_OBJECT
public:
    QExternalApplicationConfigurationScreen(QWidget *parent = 0);
    ~QExternalApplicationConfigurationScreen();

private:
    /**
     * @brief Moves a table row up or down according to a given shift.
     * @param shift Amount of
     */
    void moveItem(int shift = -1);

    /**
     * @brief Enables or disables the buttons.
     *
     * Checks the situation with the table widget and enables or disables up,
     * down and delete buttons.
     * They are only enabled when needed.
     *
     * You should invoke this method when the table data changes.
     */
    void checkGrayeds();

private slots:
    void on_btnUp_clicked();
    void on_btnDown_clicked();
    void on_btnAdd_clicked();
    void on_btnDelete_clicked();
    void on_tableWidget_currentItemChanged(QTableWidgetItem* current, QTableWidgetItem* previous);

};

} // End namespace udg

#endif
