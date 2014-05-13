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

#include "qconfigurationscreen.h"

#include <QMessageBox>
#include <QDir>

#include "pacsconnection.h"
#include "pacsdevice.h"
#include "pacsdevicemanager.h"
#include "starviewerapplication.h"
#include "logging.h"
#include "localdatabasemanager.h"
#include "inputoutputsettings.h"
#include "echotopacs.h"
#include "portinuse.h"
#include "singleton.h"
#include "queryscreen.h"

namespace udg {

typedef SingletonPointer<QueryScreen> QueryScreenSingleton;

QConfigurationScreen::QConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);

    // Emplena el listview amb les dades dels pacs, que tenim configurats
    fillPacsListView();
    loadPacsDefaults();
    loadInstitutionInformation();

    createConnections();
    createLocalConfigurationTabConnections();

    configureInputValidator();

    Settings().restoreColumnsWidths(InputOutputSettings::ConfigurationScreenPACSList, m_PacsTreeView);

    // Amaguem la columna amb l'ID del PACS que és irrellevant per l'usuari
    m_PacsTreeView->setColumnHidden(0, true);
    // Ordenem per AETitle
    m_PacsTreeView->sortByColumn(1, Qt::AscendingOrder);

    checkIncomingConnectionsPortNotInUse();
}

QConfigurationScreen::~QConfigurationScreen()
{
    Settings().saveColumnsWidths(InputOutputSettings::ConfigurationScreenPACSList, m_PacsTreeView);
}

void QConfigurationScreen::createConnections()
{
    connect(m_checkBoxQueryRetrieveEnabled, SIGNAL(stateChanged(int)), SLOT(queryRetrieveServiceEnabledChanged()));
    connect(m_checkBoxStoreEnabled, SIGNAL(stateChanged(int)), SLOT(storeServiceEnabledChanged()));

    connect(m_textQueryRetrieveServicePort, SIGNAL(editingFinished()), SLOT(onQueryRetrieveServicePortChanged()));

    // Manteniment PACS
    connect(m_buttonAddPacs, SIGNAL(clicked()), SLOT(addPacs()));
    connect(m_buttonDeletePacs, SIGNAL(clicked()), SLOT(deletePacs()));
    connect(m_buttonUpdatePacs, SIGNAL(clicked()), SLOT(updatePacs()));
    connect(m_buttonTestPacs, SIGNAL(clicked()), SLOT(test()));
    connect(m_PacsTreeView, SIGNAL(itemSelectionChanged()), SLOT(updateSelectedPACSInformation()));
}

void QConfigurationScreen::createLocalConfigurationTabConnections()
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

void QConfigurationScreen::configureInputValidator()
{
    m_textQueryRetrieveServicePort->setValidator(new QIntValidator(0, 65535, m_textQueryRetrieveServicePort));
    m_textStoreServicePort->setValidator(new QIntValidator(0, 65535, m_textStoreServicePort));
    m_textLocalPort->setValidator(new QIntValidator(0, 65535, m_textLocalPort));
    m_textTimeout->setValidator(new QIntValidator(0, 99, m_textTimeout));
    m_textMaxConnections->setValidator(new QIntValidator(1, 15, m_textMaxConnections));
}

void QConfigurationScreen::loadPacsDefaults()
{
    Settings settings;

    m_textAETitleMachine->setText(settings.getValue(InputOutputSettings::LocalAETitle).toString());
    m_textLocalPort->setText(settings.getValue(InputOutputSettings::IncomingDICOMConnectionsPort).toString());
    m_textTimeout->setText(settings.getValue(InputOutputSettings::PACSConnectionTimeout).toString());
    m_textMaxConnections->setText(settings.getValue(InputOutputSettings::MaximumPACSConnections).toString());
}

void QConfigurationScreen::loadInstitutionInformation()
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

void QConfigurationScreen:: clear()
{
    m_textAETitle->clear();
    m_textAddress->clear();
    m_textInstitution->clear();
    m_textLocation->clear();
    m_textDescription->clear();
    m_checkDefault->setChecked(false);
    m_selectedPacsID = "";
    m_textQueryRetrieveServicePort->clear();
    m_textStoreServicePort->clear();
    m_checkBoxQueryRetrieveEnabled->setChecked(true);
    m_checkBoxStoreEnabled->setChecked(true);
}

void QConfigurationScreen::addPacs()
{
    if (validatePacsDeviceToSave())
    {
        PacsDeviceManager pacsDeviceManager;
        PacsDevice pacs = getPacsDeviceFromControls();

        INFO_LOG("Afegir PACS " + pacs.getAETitle());

        if (!pacsDeviceManager.addPACS(pacs))
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This PACS already exists."));
        }
        else
        {
            fillPacsListView();
            clear();
            QueryScreenSingleton::instance()->updateConfiguration("Pacs/ListChanged");
        }
    }
}

void QConfigurationScreen::updateSelectedPACSInformation()
{
    QTreeWidgetItem *selectedItem = 0;
    if (!m_PacsTreeView->selectedItems().isEmpty())
    {
        // Només en podem tenir un de seleccionat
        selectedItem = m_PacsTreeView->selectedItems().first();
        // TODO en comptes d'obtenir del manager, potser es podria obtenir la informació directament del tree widget i estalviar aquest pas de "query"
        // selectedItem->text(0) --> ID del pacs seleccionat al TreeWidget
        PacsDevice selectedPacs = PacsDeviceManager().getPACSDeviceByID(selectedItem->text(0));
        if (selectedPacs.isEmpty())
        {
            ERROR_LOG("No s'ha trobat cap PACS configurat amb l'ID: " + selectedItem->text(0));
            DEBUG_LOG("No s'ha trobat cap PACS configurat amb l'ID: " + selectedItem->text(0));
            clear();
        }
        else
        {
            // Emplenem els textos
            m_textAETitle->setText(selectedPacs.getAETitle());
            m_textAddress->setText(selectedPacs.getAddress());
            m_textInstitution->setText(selectedPacs.getInstitution());
            m_textLocation->setText(selectedPacs.getLocation());
            m_textDescription->setText(selectedPacs.getDescription());
            m_checkDefault->setChecked(selectedPacs.isDefault());
            m_checkBoxQueryRetrieveEnabled->setChecked(selectedPacs.isQueryRetrieveServiceEnabled());
            m_textQueryRetrieveServicePort->setText(selectedPacs.isQueryRetrieveServiceEnabled() ?
                                                    QString().setNum(selectedPacs.getQueryRetrieveServicePort()) : "");
            m_textQueryRetrieveServicePort->setEnabled(selectedPacs.isQueryRetrieveServiceEnabled());
            m_checkBoxStoreEnabled->setChecked(selectedPacs.isStoreServiceEnabled());
            m_textStoreServicePort->setText(selectedPacs.isStoreServiceEnabled() ? QString().setNum(selectedPacs.getStoreServicePort()) : "");
            m_textStoreServicePort->setEnabled(selectedPacs.isStoreServiceEnabled());
        }
        // Indiquem quin és l'ID del PACS seleccionat
        m_selectedPacsID = selectedPacs.getID();
    }
    else
    {
        m_selectedPacsID = "";
    }
}

void QConfigurationScreen::updatePacs()
{
    if (m_selectedPacsID == "")
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select a PACS to update."));
        return;
    }

    if (validatePacsDeviceToSave())
    {
        PacsDevice pacs = getPacsDeviceFromControls();

        INFO_LOG("Actualitzant dades del PACS: " + pacs.getAETitle());

        PacsDeviceManager().updatePACS(pacs);

        fillPacsListView();
        clear();
        QueryScreenSingleton::instance()->updateConfiguration("Pacs/ListChanged");
    }
}

void QConfigurationScreen::deletePacs()
{
    if (m_selectedPacsID == "")
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Select a PACS to delete."));
        return;
    }

    INFO_LOG("Esborrant el PACS: " + m_textAETitle->text());

    PacsDeviceManager().deletePACS(m_selectedPacsID);

    fillPacsListView();
    clear();
    QueryScreenSingleton::instance()->updateConfiguration("Pacs/ListChanged");
}

void QConfigurationScreen::fillPacsListView()
{
    m_PacsTreeView->clear();

    QList<PacsDevice> pacsList = PacsDeviceManager().getPACSList();

    foreach (PacsDevice pacs, pacsList)
    {
        QTreeWidgetItem *item = new QTreeWidgetItem(m_PacsTreeView);

        item->setText(0, pacs.getID());
        item->setText(1, pacs.getAETitle());
        item->setText(2, pacs.getAddress());
        item->setText(3, pacs.getInstitution());
        item->setText(4, pacs.isQueryRetrieveServiceEnabled() ? QString().setNum(pacs.getQueryRetrieveServicePort()) : "Disabled");
        item->setText(5, pacs.isStoreServiceEnabled() ? QString().setNum(pacs.getStoreServicePort()) : "Disabled");
        item->setText(6, pacs.isDefault() ? tr("Yes") : tr("No"));
    }
}

void QConfigurationScreen::test()
{
    if (validatePacsDeviceToEcho())
    {
        QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));

        // Agafem les dades del PACS que estan el textbox per testejar
        PacsDevice pacsDevice = getPacsDeviceFromControls();
        EchoToPACS echoToPACS;

        INFO_LOG("Es fa echoSCU al PACS amb AE Title " + pacsDevice.getAETitle());

        if (echoToPACS.echo(pacsDevice))
        {
            QApplication::restoreOverrideCursor();
            QMessageBox::information(this, ApplicationNameString, tr("Test of PACS \"%1\" is correct").arg(pacsDevice.getAETitle()));
        }
        else
        {
            QApplication::restoreOverrideCursor();
            QString message;

            switch (echoToPACS.getLastError())
            {
                case EchoToPACS::EchoFailed:
                    message = tr("PACS \"%1\" did not respond correctly.\nMake sure its IP and AE Title are correct.").arg(pacsDevice.getAETitle());
                    break;
                case EchoToPACS::EchoCanNotConnectToPACS:
                    message = tr("PACS \"%1\" did not respond.\nMake sure its IP and AE Title are correct.").arg(pacsDevice.getAETitle());
                    break;
                default:
                    // No hauria de passar mai
                    message = tr("PACS \"%1\" did not respond as expected, an unknown error has occurred.").arg(pacsDevice.getAETitle());
                    break;
            }

            QMessageBox::information(this, ApplicationNameString, message);
        }
    }
}

bool QConfigurationScreen::validatePacsDeviceToEcho()
{
    if (m_textAETitle->text().length() == 0)
    {
        QMessageBox::warning(this, ApplicationNameString, tr("AE Title field cannot be empty."));
        return false;
    }

    if (m_textAddress->text().length() == 0)
    {
        QMessageBox::warning(this, ApplicationNameString, tr ("Incorrect server address."));
        return false;
    }

    if (!m_checkBoxQueryRetrieveEnabled->isChecked() && !m_checkBoxStoreEnabled->isChecked())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("At least one of the services Query/Retrieve or Send has to be enabled."));
        return false;
    }

    if (m_checkBoxQueryRetrieveEnabled->isChecked() && m_textQueryRetrieveServicePort->text().isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Query/Retrieve port value has to be between 0 and 65535."));
        return false;
    }

    if (m_checkBoxStoreEnabled->isChecked() && m_textStoreServicePort->text().isEmpty())
    {
        QMessageBox::warning(this, ApplicationNameString, tr("Send port value has to be between 0 and 65535."));
        return false;
    }

    return true;
}

bool QConfigurationScreen::validatePacsDeviceToSave()
{
    if (validatePacsDeviceToEcho())
    {
        // La institució no pot estar en blanc
        if (m_textInstitution->text().length() == 0)
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Institution field cannot be empty."));
            return false;
        }

        return true;
    }
    else
    {
        return false;
    }
}

void QConfigurationScreen::updateAETitleSetting()
{
    if (m_textAETitleMachine->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::LocalAETitle, m_textAETitleMachine->text());
        INFO_LOG("Updated AETitle setting: " + m_textAETitleMachine->text());
    }
}

void QConfigurationScreen::updateTimeoutSetting()
{
    if (m_textTimeout->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::PACSConnectionTimeout, m_textTimeout->text());
        INFO_LOG("Updated timeout setting: " + m_textTimeout->text());
    }
}

void QConfigurationScreen::updateLocalPortSetting()
{
    if (m_textLocalPort->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::IncomingDICOMConnectionsPort, m_textLocalPort->text());
        INFO_LOG("Updated local port settings: " + m_textLocalPort->text());
    }
}

void QConfigurationScreen::updateMaxConnectionsSetting()
{
    if (m_textMaxConnections->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::MaximumPACSConnections, m_textMaxConnections->text());
        INFO_LOG("Updated maximum connections setting: " + m_textMaxConnections->text());
    }
}

void QConfigurationScreen::updateInstitutionNameSetting()
{
    if (m_textInstitutionName->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionName, m_textInstitutionName->text());
    }
}

void QConfigurationScreen::updateInstitutionAddressSetting()
{
    if (m_textInstitutionAddress->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionAddress, m_textInstitutionAddress->text());
    }
}

void QConfigurationScreen::updateInstitutionTownSetting()
{
    if (m_textInstitutionTown->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionTown, m_textInstitutionTown->text());
    }
}

void QConfigurationScreen::updateInstitutionZipCodeSetting()
{
    if (m_textInstitutionZipCode->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionZipCode, m_textInstitutionZipCode->text());
    }
}

void QConfigurationScreen::updateInstitutionCountrySetting()
{
    if (m_textInstitutionCountry->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionCountry, m_textInstitutionCountry->text());
    }
}

void QConfigurationScreen::updateInstitutionEmailSetting()
{
    if (m_textInstitutionEmail->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionEmail, m_textInstitutionEmail->text());
    }
}

void QConfigurationScreen::updateInstitutionPhoneNumberSetting()
{
    if (m_textInstitutionPhoneNumber->isModified())
    {
        Settings settings;
        settings.setValue(InputOutputSettings::InstitutionPhoneNumber, m_textInstitutionPhoneNumber->text());
    }
}

bool QConfigurationScreen::isIncomingConnectionsPortInUseByAnotherApplication()
{
    // Comprovem que el port estigui o no en ús i que en el cas que estigui en ús, no sigui utilitzat per l'Starviewer
    return PortInUse().isPortInUse(m_textLocalPort->text().toInt()) && !LocalDatabaseManager().isStudyRetrieving();
}

void QConfigurationScreen::checkIncomingConnectionsPortNotInUse()
{
    // Si està en ús el frame que conté el warning es fa visible
    m_warningFrameIncomingConnectionsPortInUse->setVisible(isIncomingConnectionsPortInUseByAnotherApplication());
}

void QConfigurationScreen::queryRetrieveServiceEnabledChanged()
{
    m_textQueryRetrieveServicePort->setEnabled(m_checkBoxQueryRetrieveEnabled->isChecked());
    m_textQueryRetrieveServicePort->setText("");
    // "Default query PACS" no té sentit que estigui activat si no es pot fer query en el PACS
    m_checkDefault->setEnabled(m_checkBoxQueryRetrieveEnabled->isChecked());

    if (!m_checkBoxQueryRetrieveEnabled->isChecked())
    {
        m_checkDefault->setChecked(false);
    }
}

void QConfigurationScreen::storeServiceEnabledChanged()
{
    m_textStoreServicePort->setEnabled(m_checkBoxStoreEnabled->isChecked());
    // Si ens indiquen que el servei d'Store està permés li donem el mateix port que del Query/Retrieve, ja que la majoria de
    // PACS utilitzen el mateix port per Q/R que per store
    m_textStoreServicePort->setText(m_checkBoxStoreEnabled->isChecked() ? m_textQueryRetrieveServicePort->text() : "");
}

void QConfigurationScreen::onQueryRetrieveServicePortChanged()
{
    if (!m_textQueryRetrieveServicePort->text().isEmpty())
    {
        if (m_checkBoxStoreEnabled->isChecked() && m_textStoreServicePort->text().isEmpty())
        {
            // Si s'ha indicat que el servei d'store està permés i aquest no té el port configurat li donem per defecte el valor del port de Q/R
            m_textStoreServicePort->setText(m_textQueryRetrieveServicePort->text());
        }
    }
}

PacsDevice QConfigurationScreen::getPacsDeviceFromControls()
{
    PacsDevice pacsDevice;

    pacsDevice.setAETitle(m_textAETitle->text());
    pacsDevice.setAddress(m_textAddress->text());
    pacsDevice.setInstitution(m_textInstitution->text());
    pacsDevice.setLocation(m_textLocation->text());
    pacsDevice.setDescription(m_textDescription->text());
    pacsDevice.setID(m_selectedPacsID);
    pacsDevice.setQueryRetrieveServiceEnabled(m_checkBoxQueryRetrieveEnabled->isChecked());
    pacsDevice.setQueryRetrieveServicePort(m_textQueryRetrieveServicePort->text().toInt());
    pacsDevice.setStoreServiceEnabled(m_checkBoxStoreEnabled->isChecked());
    pacsDevice.setStoreServicePort(m_textStoreServicePort->text().toInt());
    pacsDevice.setDefault(m_checkDefault->isChecked());

    return pacsDevice;
}

};
