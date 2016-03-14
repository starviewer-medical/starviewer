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
#include <QList>
#include "externalapplication.h"

namespace udg {

/**
    @brief External application configuration GUI

    This GUI is used to edit the external applications list. When properly
    configured, the entries set here appear on a menu.
 */
class QExternalApplicationConfigurationScreen : public QWidget, private Ui::QExternalApplicationConfigurationScreenBase {
Q_OBJECT
public:
    /**
     * @brief Constructs the GUI and fills the list.
     * The constructor retrieves the information from the settings and fills the GUI list automatically using the setExternalApplications method.
     * @param parent
     */
    QExternalApplicationConfigurationScreen(QWidget *parent = 0);
    ~QExternalApplicationConfigurationScreen();

    /**
     * @brief Use it to show the applications
     * @param externalApplications List of external applications to show on the list.
     */
    void setExternalApplications(const QList<ExternalApplication> &externalApplications);
    /**
     * @brief Generates the list of the external applications edited by the user.
     * @return List of the applications on the GUI list.
     */
    QList<ExternalApplication> getExternalApplications() const;

    /**
     * @brief Adds applications to the GUI application list.
     * @param externalApplication Application to add.
     */
    void addApplication(const ExternalApplication &externalApplication);

protected:
    /**
     * @brief On close the configuration is saved.
     * Make sure that you call the close() function before destroying this class.
     * @param event
     */
    void closeEvent(QCloseEvent *event);

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
