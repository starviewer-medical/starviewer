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

#include "qpacsdialog.h"

#include "echotopacs.h"
#include "logging.h"
#include "pacsdevicemanager.h"
#include "starviewerapplication.h"

#include <QMessageBox>
#include <QRegularExpressionValidator>

namespace udg {

QPacsDialog::QPacsDialog(PacsDevice::Type type, QWidget *parent)
    : QDialog(parent), m_pacsType(type)
{
    initialize();

    switch (m_pacsType)
    {
        case PacsDevice::Type::Dimse:
            setWindowTitle(tr("New DIMSE PACS server"));
            setupDimse();
            break;

        case PacsDevice::Type::Wado:
            setWindowTitle(tr("New WADO PACS server"));
            setupWado();
            break;
    }

    adjustSize();
}

QPacsDialog::QPacsDialog(QString pacsId, QWidget *parent)
    : QDialog(parent), m_pacsId(std::move(pacsId))
{
    initialize();
    setWindowTitle(tr("Edit PACS server"));
    reset();    // this will fill the dialog with the appropriate values
    adjustSize();
}

void QPacsDialog::initialize()
{
    setupUi(this);

    m_aeTitleLineEdit->setValidator(new QRegularExpressionValidator(QRegularExpression(R"([ !"#$%&'()*+,\-./0-9:;<=>?@A-Z[\]^_`{|}~]{1,16})",
                                                                                       QRegularExpression::CaseInsensitiveOption),
                                                                    m_aeTitleLineEdit));
    createConnections();
}

void QPacsDialog::setupDimse()
{
    m_aeTitleLabel->show();
    m_aeTitleLineEdit->show();
    m_addressLabel->show();
    m_addressLineEdit->show();
    m_qrServiceLabel->show();
    m_qrServiceEnabledCheckBox->show();
    m_qrServicePortLabel->show();
    m_qrServicePortSpinBox->show();
    m_sendServiceLabel->show();
    m_sendServiceEnabledCheckBox->show();
    m_sendServicePortLabel->show();
    m_sendServicePortSpinBox->show();
    m_baseUriLabel->hide();
    m_baseUriLineEdit->hide();
}

void QPacsDialog::setupWado()
{
    m_aeTitleLabel->hide();
    m_aeTitleLineEdit->hide();
    m_addressLabel->hide();
    m_addressLineEdit->hide();
    m_qrServiceLabel->hide();
    m_qrServiceEnabledCheckBox->hide();
    m_qrServicePortLabel->hide();
    m_qrServicePortSpinBox->hide();
    m_sendServiceLabel->hide();
    m_sendServiceEnabledCheckBox->hide();
    m_sendServicePortLabel->hide();
    m_sendServicePortSpinBox->hide();
    m_baseUriLabel->show();
    m_baseUriLineEdit->show();
}

void QPacsDialog::createConnections()
{
    connect(m_qrServiceEnabledCheckBox, &QCheckBox::toggled, m_qrServicePortSpinBox, &QWidget::setEnabled);
    connect(m_qrServiceEnabledCheckBox, &QCheckBox::toggled, [this](bool checked) {
        if (m_pacsType == PacsDevice::Type::Dimse)
        {
            if (checked) {
                m_defaultQueryPacsCheckBox->setEnabled(true);
            }
            else
            {
                m_defaultQueryPacsCheckBox->setEnabled(false);
                m_defaultQueryPacsCheckBox->setChecked(false);
            }
        }
    });
    connect(m_sendServiceEnabledCheckBox, &QCheckBox::toggled, m_sendServicePortSpinBox, &QWidget::setEnabled);

    connect(m_testPushButton, &QPushButton::clicked, this, &QPacsDialog::test);
    connect(m_buttonBox->button(QDialogButtonBox::Apply), &QPushButton::clicked, this, &QPacsDialog::save);
    connect(m_buttonBox->button(QDialogButtonBox::Reset), &QPushButton::clicked, this, &QPacsDialog::reset);
}

bool QPacsDialog::arePacsSettingsValid()
{
    if (m_pacsType == PacsDevice::Type::Dimse)
    {
        if (!m_aeTitleLineEdit->hasAcceptableInput() || m_aeTitleLineEdit->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, ApplicationNameString, tr("AE Title is empty or has an invalid value."), QMessageBox::Ok);
            return false;
        }

        if (m_addressLineEdit->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Address cannot be empty."), QMessageBox::Ok);
            return false;
        }

        if (!m_qrServiceEnabledCheckBox->isChecked() && !m_sendServiceEnabledCheckBox->isChecked())
        {
            QMessageBox::warning(this, ApplicationNameString, tr("At least one of the services Query/Retrieve or Send must be enabled."), QMessageBox::Ok);
            return false;
        }
    }
    else if (m_pacsType == PacsDevice::Type::Wado)
    {
        if (m_baseUriLineEdit->text().trimmed().isEmpty())
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Base URI cannot be empty."), QMessageBox::Ok);
            return false;
        }

        if (!QUrl(m_baseUriLineEdit->text()).isValid())
        {
            QMessageBox::warning(this, ApplicationNameString, tr("Base URI is not a valid URI."), QMessageBox::Ok);
            return false;
        }
    }

    return true;
}

PacsDevice QPacsDialog::getPacsDevice() const
{
    PacsDevice pacsDevice;
    pacsDevice.setID(m_pacsId);
    pacsDevice.setType(m_pacsType);
    pacsDevice.setAETitle(m_aeTitleLineEdit->text());
    pacsDevice.setAddress(m_addressLineEdit->text());
    pacsDevice.setQueryRetrieveServiceEnabled(m_qrServiceEnabledCheckBox->isChecked());
    pacsDevice.setQueryRetrieveServicePort(m_qrServicePortSpinBox->value());
    pacsDevice.setStoreServiceEnabled(m_sendServiceEnabledCheckBox->isChecked());
    pacsDevice.setStoreServicePort(m_sendServicePortSpinBox->value());
    pacsDevice.setBaseUri(m_baseUriLineEdit->text());
    pacsDevice.setInstitution(m_institutionLineEdit->text());
    pacsDevice.setLocation(m_locationLineEdit->text());
    pacsDevice.setDescription(m_descriptionLineEdit->text());
    pacsDevice.setDefault(m_defaultQueryPacsCheckBox->isChecked());

    return pacsDevice;
}

void QPacsDialog::test()
{
    if (!arePacsSettingsValid())
    {
        return;
    }

    // Agafem les dades del PACS que estan el textbox per testejar
    PacsDevice pacsDevice = getPacsDevice();
    EchoToPACS echoToPACS;

    QApplication::setOverrideCursor(QCursor(Qt::WaitCursor));
    INFO_LOG(QString("Doing C-ECHO to PACS with AE Title %1.").arg(pacsDevice.getAETitle()));
    bool success = echoToPACS.echo(pacsDevice);
    QApplication::restoreOverrideCursor();

    if (success)
    {
        QMessageBox::information(this, ApplicationNameString, tr("Test of PACS \"%1\" is correct").arg(pacsDevice.getAETitle()), QMessageBox::Ok);
    }
    else
    {
        QString message;

        switch (echoToPACS.getLastError())
        {
            case EchoToPACS::EchoFailed:
                message = tr("PACS \"%1\" did not respond correctly.\nMake sure its address and AE Title are correct.").arg(pacsDevice.getAETitle());
                break;
            case EchoToPACS::EchoCanNotConnectToPACS:
                message = tr("PACS \"%1\" did not respond.\nMake sure its address and AE Title are correct.").arg(pacsDevice.getAETitle());
                break;
            default:
                // Should never happen
                message = tr("PACS \"%1\" did not respond as expected, an unknown error has occurred.").arg(pacsDevice.getAETitle());
                break;
        }

        QMessageBox::information(this, ApplicationNameString, message, QMessageBox::Ok);
    }
}

void QPacsDialog::accept()
{
    if (save())
    {
        QDialog::accept();
    }
}

bool QPacsDialog::save()
{
    if (!arePacsSettingsValid())
    {
        return false;
    }

    PacsDevice pacsDevice = getPacsDevice();

    if (m_pacsId.isEmpty())
    {
        if (m_pacsType == PacsDevice::Type::Dimse)
        {
            INFO_LOG(QString("Adding new DIMSE PACS with AE Title %1.").arg(pacsDevice.getAETitle()));
        }
        else if (m_pacsType == PacsDevice::Type::Wado)
        {
            INFO_LOG(QString("Adding new WADO PACS with base URI %1.").arg(pacsDevice.getBaseUri().toString()));
        }

        if (PacsDeviceManager::addPacs(pacsDevice))
        {
            m_pacsId = pacsDevice.getID();
            return true;
        }
        else
        {
            QMessageBox::warning(this, ApplicationNameString, tr("This PACS already exists."), QMessageBox::Ok);
            return false;
        }
    }
    else
    {
        INFO_LOG(QString("Updating PACS with ID %1.").arg(pacsDevice.getID()));
        PacsDeviceManager::updatePacs(pacsDevice);
        return true;
    }
}

void QPacsDialog::reset()
{
    if (m_pacsId.isEmpty())
    {
        m_aeTitleLineEdit->clear();
        m_addressLineEdit->clear();
        m_qrServiceEnabledCheckBox->setChecked(true);
        m_qrServicePortSpinBox->setValue(11112);
        m_sendServiceEnabledCheckBox->setChecked(true);
        m_sendServicePortSpinBox->setValue(11112);
        m_baseUriLineEdit->clear();
        m_institutionLineEdit->clear();
        m_locationLineEdit->clear();
        m_descriptionLineEdit->clear();
        m_defaultQueryPacsCheckBox->setChecked(false);
    }
    else
    {
        PacsDevice pacsDevice = PacsDeviceManager::getPacsDeviceById(m_pacsId);
        m_pacsType = pacsDevice.getType();
        m_aeTitleLineEdit->setText(pacsDevice.getAETitle());
        m_addressLineEdit->setText(pacsDevice.getAddress());
        m_qrServiceEnabledCheckBox->setChecked(pacsDevice.isQueryRetrieveServiceEnabled());
        m_qrServicePortSpinBox->setValue(pacsDevice.getQueryRetrieveServicePort());
        m_sendServiceEnabledCheckBox->setChecked(pacsDevice.isStoreServiceEnabled());
        m_sendServicePortSpinBox->setValue(pacsDevice.getStoreServicePort());
        m_baseUriLineEdit->setText(pacsDevice.getBaseUri().toString());
        m_institutionLineEdit->setText(pacsDevice.getInstitution());
        m_locationLineEdit->setText(pacsDevice.getLocation());
        m_descriptionLineEdit->setText(pacsDevice.getDescription());
        m_defaultQueryPacsCheckBox->setChecked(pacsDevice.isDefault());

        if (m_pacsType == PacsDevice::Type::Dimse)
        {
            setupDimse();
        }
        else if (m_pacsType == PacsDevice::Type::Wado)
        {
            setupWado();
        }
    }
}

} // namespace udg
