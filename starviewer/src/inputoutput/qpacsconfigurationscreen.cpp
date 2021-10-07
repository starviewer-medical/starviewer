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

#include "qpacsconfigurationscreen.h"

#include "inputoutputsettings.h"
#include "localdatabasemanager.h"
#include "logging.h"
#include "pacsdevice.h"
#include "pacsdevicemanager.h"
#include "pacsdevicemodel.h"
#include "portinuse.h"
#include "qpacsdialog.h"
#include "queryscreen.h"
#include "singleton.h"
#include "starviewerapplication.h"

#include <QSortFilterProxyModel>

namespace udg {

typedef SingletonPointer<QueryScreen> QueryScreenSingleton;

QPacsConfigurationScreen::QPacsConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    m_pacsDeviceModel = new PacsDeviceModel(m_pacsTableView);
    QSortFilterProxyModel *filterModel = new QSortFilterProxyModel(m_pacsTableView);
    filterModel->setSourceModel(m_pacsDeviceModel);
    m_pacsTableView->setModel(filterModel);

    loadPacsDefaults();
    loadInstitutionInformation();

    createConnections();
    createLocalConfigurationTabConnections();

    configureInputValidator();

    Settings().restoreColumnsWidths(InputOutputSettings::ConfigurationScreenPACSList, m_pacsTableView);

    // Amaguem la columna amb l'ID del PACS que és irrellevant per l'usuari
    m_pacsTableView->setColumnHidden(0, true);
    // Ordenem per AETitle
    m_pacsTableView->sortByColumn(1, Qt::AscendingOrder);

    checkIncomingConnectionsPortNotInUse();
}

QPacsConfigurationScreen::~QPacsConfigurationScreen()
{
    Settings().saveColumnsWidths(InputOutputSettings::ConfigurationScreenPACSList, m_pacsTableView);
}

void QPacsConfigurationScreen::createConnections()
{
    // Manteniment PACS
    connect(m_buttonAddPacs, SIGNAL(clicked()), SLOT(addPacs()));
    connect(m_buttonEditPacs, &QPushButton::clicked, this, &QPacsConfigurationScreen::editPacs);
    connect(m_buttonDeletePacs, SIGNAL(clicked()), SLOT(deletePacs()));
    connect(m_pacsTableView->selectionModel(), &QItemSelectionModel::selectionChanged, this, &QPacsConfigurationScreen::updateButtonsState);
    connect(m_pacsDeviceModel, &QAbstractItemModel::modelReset, this, &QPacsConfigurationScreen::updateButtonsState);
}

void QPacsConfigurationScreen::createLocalConfigurationTabConnections()
{
    // Local config
    connect(m_textAETitleMachine, SIGNAL(editingFinished()), SLOT(updateAETitleSetting()));
    connect(m_textTimeout, SIGNAL(editingFinished()), SLOT(updateTimeoutSetting()));
    connect(m_textLocalPort, SIGNAL(textChanged(const QString&)), SLOT(checkIncomingConnectionsPortNotInUse()));
    connect(m_textLocalPort, SIGNAL(editingFinished()), SLOT(updateLocalPortSetting()));
    connect(m_textMaxConnections, SIGNAL(editingFinished()), SLOT(updateMaxConnectionsSetting()));
    // Institution info
    connect(m_textInstitutionName, SIGNAL(editingFinished()), SLOT(updateInstitutionNameSetting()));
    connect(m_textInstitutionAddress, SIGNAL(editingFinished()), SLOT(updateInstitutionAddressSetting()));
    connect(m_textInstitutionTown, SIGNAL(editingFinished()), SLOT(updateInstitutionTownSetting()));
    connect(m_textInstitutionZipCode, SIGNAL(editingFinished()), SLOT(updateInstitutionZipCodeSetting()));
    connect(m_textInstitutionCountry, SIGNAL(editingFinished()), SLOT(updateInstitutionCountrySetting()));
    connect(m_textInstitutionEmail, SIGNAL(editingFinished()), SLOT(updateInstitutionEmailSetting()));
    connect(m_textInstitutionPhoneNumber, SIGNAL(editingFinished()), SLOT(updateInstitutionPhoneNumberSetting()));
}

void QPacsConfigurationScreen::configureInputValidator()
{
    m_textLocalPort->setValidator(new QIntValidator(0, 65535, m_textLocalPort));
    m_textTimeout->setValidator(new QIntValidator(0, 99, m_textTimeout));
    m_textMaxConnections->setValidator(new QIntValidator(1, 15, m_textMaxConnections));
}

void QPacsConfigurationScreen::loadPacsDefaults()
{
    Settings settings;

    m_textAETitleMachine->setText(settings.getValue(InputOutputSettings::LocalAETitle).toString());
    m_textLocalPort->setText(settings.getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toString());
    m_textTimeout->setText(settings.getValue(InputOutputSettings::PACSConnectionTimeout).toString());
    m_textMaxConnections->setText(settings.getValue(InputOutputSettings::MaximumPACSConnections).toString());
}

void QPacsConfigurationScreen::loadInstitutionInformation()
{
    Settings settings;

    m_textInstitutionName->setText(settings.getValue(InputOutputSettings::InstitutionName).toString());
    m_textInstitutionAddress->setText(settings.getValue(InputOutputSettings::InstitutionAddress).toString());
    m_textInstitutionTown->setText(settings.getValue(InputOutputSettings::InstitutionTown).toString());
    m_textInstitutionZipCode->setText(settings.getValue(InputOutputSettings::InstitutionZipCode).toString());
    m_textInstitutionCountry->setText(settings.getValue(InputOutputSettings::InstitutionCountry).toString());
    m_textInstitutionEmail->setText(settings.getValue(InputOutputSettings::InstitutionEmail).toString());
    m_textInstitutionPhoneNumber->setText(settings.getValue(InputOutputSettings::InstitutionPhoneNumber).toString());
}

void QPacsConfigurationScreen::addPacs()
{
    QPacsDialog *dialog = new QPacsDialog(this);

    connect(dialog, &QDialog::finished, [=] {
        // Update always independently of the result because the dialog has an apply button
        refreshPacsList();
        delete dialog;
    });

    dialog->open();
}

void QPacsConfigurationScreen::editPacs()
{
    if (!m_pacsTableView->selectionModel()->hasSelection())
    {
        WARN_LOG("editPacs() called with no PACS selected. It should be impossible.");
        return;
    }

    QModelIndex index = m_pacsTableView->selectionModel()->selectedRows(0).first();
    QString pacsId = m_pacsTableView->model()->data(index).toString();

    QPacsDialog *dialog = new QPacsDialog(pacsId, this);

    connect(dialog, &QDialog::finished, [=] {
        // Update always independently of the result because the dialog has an apply button
        refreshPacsList();
        delete dialog;
        m_pacsTableView->selectRow(index.row());    // reselect the same row because it's deselected by the refresh
    });

    dialog->open();
}

void QPacsConfigurationScreen::deletePacs()
{
    if (!m_pacsTableView->selectionModel()->hasSelection())
    {
        WARN_LOG("deletePacs() called with no PACS selected. It should be impossible.");
        return;
    }

    QModelIndex index = m_pacsTableView->selectionModel()->selectedRows(0).first();
    QString pacsId = m_pacsTableView->model()->data(index).toString();

    INFO_LOG(QString("Deleting PACS with ID %1.").arg(pacsId));
    PacsDeviceManager::deletePacs(pacsId);

    refreshPacsList();
}

void QPacsConfigurationScreen::updateButtonsState()
{
    bool pacsSelected = m_pacsTableView->selectionModel()->hasSelection();
    m_buttonEditPacs->setEnabled(pacsSelected);
    m_buttonDeletePacs->setEnabled(pacsSelected);
}

void QPacsConfigurationScreen::updateAETitleSetting()
{
    if (m_textAETitleMachine->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::LocalAETitle, m_textAETitleMachine->text());
        INFO_LOG("Updated AETitle setting: " + m_textAETitleMachine->text());
    }
}

void QPacsConfigurationScreen::updateTimeoutSetting()
{
    if (m_textTimeout->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::PACSConnectionTimeout, m_textTimeout->text());
        INFO_LOG("Updated timeout setting: " + m_textTimeout->text());
    }
}

void QPacsConfigurationScreen::updateLocalPortSetting()
{
    if (m_textLocalPort->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::IncomingDICOMConnectionsPort, m_textLocalPort->text());
        INFO_LOG("Updated local port settings: " + m_textLocalPort->text());
    }
}

void QPacsConfigurationScreen::updateMaxConnectionsSetting()
{
    if (m_textMaxConnections->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::MaximumPACSConnections, m_textMaxConnections->text());
        INFO_LOG("Updated maximum connections setting: " + m_textMaxConnections->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionNameSetting()
{
    if (m_textInstitutionName->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionName, m_textInstitutionName->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionAddressSetting()
{
    if (m_textInstitutionAddress->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionAddress, m_textInstitutionAddress->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionTownSetting()
{
    if (m_textInstitutionTown->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionTown, m_textInstitutionTown->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionZipCodeSetting()
{
    if (m_textInstitutionZipCode->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionZipCode, m_textInstitutionZipCode->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionCountrySetting()
{
    if (m_textInstitutionCountry->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionCountry, m_textInstitutionCountry->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionEmailSetting()
{
    if (m_textInstitutionEmail->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionEmail, m_textInstitutionEmail->text());
    }
}

void QPacsConfigurationScreen::updateInstitutionPhoneNumberSetting()
{
    if (m_textInstitutionPhoneNumber->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionPhoneNumber, m_textInstitutionPhoneNumber->text());
    }
}

bool QPacsConfigurationScreen::isIncomingConnectionsPortInUseByAnotherApplication()
{
    // Comprovem que el port estigui o no en ús i que en el cas que estigui en ús, no sigui utilitzat per l'Starviewer
    return PortInUse().isPortInUse(m_textLocalPort->text().toInt()) && !LocalDatabaseManager().isAStudyBeingRetrieved();
}

void QPacsConfigurationScreen::checkIncomingConnectionsPortNotInUse()
{
    // Si està en ús el frame que conté el warning es fa visible
    m_warningFrameIncomingConnectionsPortInUse->setVisible(isIncomingConnectionsPortInUseByAnotherApplication());
}

void QPacsConfigurationScreen::refreshPacsList()
{
    m_pacsDeviceModel->refresh();
    QueryScreenSingleton::instance()->updateConfiguration("Pacs/ListChanged");
}

};
