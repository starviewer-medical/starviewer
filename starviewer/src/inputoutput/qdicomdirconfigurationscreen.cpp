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

#include "qdicomdirconfigurationscreen.h"

#include <QFileDialog>

#include "inputoutputsettings.h"
#include "logging.h"
#include "converttodicomdir.h"

namespace udg {

QDICOMDIRConfigurationScreen::QDICOMDIRConfigurationScreen(QWidget *parent)
 : QWidget(parent)
{
    setupUi(this);
    m_burningApplicationPathValidationLabel->setVisible(false);
    m_burningApplicationPathValidationIcon->setVisible(false);
    m_copyFolderContentValidationLabel->setVisible(false);
    m_copyFolderContentValidationIcon->setVisible(false);
    createConnections();
    loadDICOMDIRDefaults();
    setupSettingsUpdatesConnections();
}

QDICOMDIRConfigurationScreen::~QDICOMDIRConfigurationScreen()
{
}

void QDICOMDIRConfigurationScreen::createConnections()
{
    // Connecta el boto examinar programa de gravació amb el dialog per escollir el path del programa
    connect(m_buttonExaminateBurningApplication, SIGNAL(clicked()), SLOT(examinateDICOMDIRBurningApplicationPath()));
    connect(m_buttonExaminateDICOMDIRFolderPathToCopy, SIGNAL(clicked()), SLOT(examinateDICOMDIRFolderPathToCopy()));
}

void QDICOMDIRConfigurationScreen::setupSettingsUpdatesConnections()
{
    // DICOMDIR creation configuration
    connect(m_checkBoxConvertDICOMDIRImagesToLittleEndian, SIGNAL(clicked(bool)), SLOT(updateExplicitLittleEndianSetting(bool)));
    // Burning application configuration
    connect(m_textBurningApplicationPath, SIGNAL(textChanged(QString)), SLOT(checkAndUpdateBurningApplicationPathSetting(QString)));
    connect(m_textBurningApplicationParameters, SIGNAL(editingFinished()), SLOT(updateBurningApplicationParametersSetting()));
    connect(m_checkBoxHasDifferentCDDVDParameteres, SIGNAL(clicked(bool)), SLOT(updateDifferentCDDVDParametersSetting(bool)));
    connect(m_textBurningApplicationCDParameters, SIGNAL(editingFinished()), SLOT(updateBurningApplicationCDParametersSetting()));
    connect(m_textBurningApplicationDVDParameters, SIGNAL(editingFinished()), SLOT(updateBurningApplicationDVDParametersSetting()));
    // Copy folder's content to DICOMDIR
    connect(m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk, SIGNAL(clicked(bool)), SLOT(updateCopyContentsFromUSBOrHardDiskSetting(bool)));
    connect(m_checkBoxCopyFolderContentToDICOMDIRCdDvd, SIGNAL(clicked(bool)), SLOT(updateCopyContentsFromCDOrDVDSetting(bool)));
    connect(m_textDICOMDIRFolderPathToCopy, SIGNAL(textChanged(QString)), SLOT(checkAndUpdateCopyFolderContentPathSetting(QString)));
}

void QDICOMDIRConfigurationScreen::loadDICOMDIRDefaults()
{
    Settings settings;

    m_textBurningApplicationPath->setText(settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString());
    m_textBurningApplicationParameters->setText(settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationParametersKey).toString());
    m_textBurningApplicationCDParameters->setText(settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey).toString());
    m_textBurningApplicationDVDParameters->setText(settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey).toString());

    bool hasDifferentCDDVDParameters = settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey).toBool();

    m_textBurningApplicationCDParameters->setEnabled(hasDifferentCDDVDParameters);
    m_textBurningApplicationDVDParameters->setEnabled(hasDifferentCDDVDParameters);
    m_checkBoxHasDifferentCDDVDParameteres->setChecked(hasDifferentCDDVDParameters);

    m_checkBoxConvertDICOMDIRImagesToLittleEndian->setChecked(settings.getValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey).toBool());

    if (m_textBurningApplicationPath->text().endsWith("k3b"))
    {
        m_burningApplicationDownloadURL->setText("<a href='http://k3b.plainblack.com'> k3b.plainblack.com </a>");
    }
    else
    {
        if (m_textBurningApplicationPath->text().endsWith("ImgBurn.exe"))
        {
            m_burningApplicationDownloadURL->setText("<a href='http://www.imgburn.com'> www.imgburn.com </a>");
        }
        else
        {
            if (m_textBurningApplicationPath->text().endsWith("Burn"))
            {
                m_burningApplicationDownloadURL->setText("<a href='http://burn-osx.sourceforge.net'> burn-osx.sourceforge.net </a>");
            }
            else
            {
                m_burningApplicationDownloadURLLabel->setVisible(false);
                m_burningApplicationDownloadURL->setVisible(false);
            }
        }
    }

    m_checkBoxCopyFolderContentToDICOMDIRCdDvd->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIROnCDOrDVD).toBool());
    m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk->setChecked(settings.getValue(InputOutputSettings::CopyFolderContentToDICOMDIROnUSBOrHardDisk).toBool());
    m_textDICOMDIRFolderPathToCopy->setText(settings.getValue(InputOutputSettings::DICOMDIRFolderPathToCopy).toString());
    
    // Validations
    validateCopyFolderContentPath(m_textDICOMDIRFolderPathToCopy->text());
    updateCopyFolderContentPathOptions();
}

void QDICOMDIRConfigurationScreen::enableCopyFolderContentPathWidgets(bool enable)
{
    m_textDICOMDIRFolderPathToCopy->setEnabled(enable);
    m_buttonExaminateDICOMDIRFolderPathToCopy->setEnabled(enable);
    m_copyFolderContentValidationLabel->setEnabled(enable);
    m_copyFolderContentValidationIcon->setEnabled(enable);
}

void QDICOMDIRConfigurationScreen::updateCopyFolderContentPathOptions()
{
    if (m_checkBoxCopyFolderContentToDICOMDIRCdDvd->isChecked() || m_checkBoxCopyFolderContentToDICOMDIRUsbHardDisk->isChecked())
    {
        enableCopyFolderContentPathWidgets(true);
        validateCopyFolderContentPath(m_textDICOMDIRFolderPathToCopy->text());
    }
    else
    {
        enableCopyFolderContentPathWidgets(false);
        if (m_textDICOMDIRFolderPathToCopy->text().isEmpty())
        {
            m_copyFolderContentValidationLabel->setVisible(false);
            m_copyFolderContentValidationIcon->setVisible(false);
        }
    }
}

bool QDICOMDIRConfigurationScreen::validateBurningApplicationPath(const QString &path)
{
    if (!QFile::exists(path))
    {
        m_burningApplicationPathValidationLabel->setText(tr("Invalid burning application path."));
        m_burningApplicationPathValidationLabel->setVisible(true);
        m_burningApplicationPathValidationIcon->setVisible(true);
        return false;
    }

    QFileInfo burningApplicationPathInfo(path);
    if (!burningApplicationPathInfo.isFile() || !burningApplicationPathInfo.isExecutable())
    {
        m_burningApplicationPathValidationLabel->setText(tr("Burning application path has to point to an executable file."));
        m_burningApplicationPathValidationLabel->setVisible(true);
        m_burningApplicationPathValidationIcon->setVisible(true);
        return false;
    }
    
    m_burningApplicationPathValidationLabel->setVisible(false);
    m_burningApplicationPathValidationIcon->setVisible(false);
    return true;
}

bool QDICOMDIRConfigurationScreen::validateCopyFolderContentPath(const QString &path)
{
    if (!path.isEmpty())
    {
        if (!QFile::exists(path))
        {
            m_copyFolderContentValidationLabel->setText(tr("Invalid path."));
            m_copyFolderContentValidationLabel->setVisible(true);
            m_copyFolderContentValidationIcon->setVisible(true);
            return false;
        }
        else if (!ConvertToDicomdir().AreValidRequirementsOfFolderContentToCopyToDICOMDIR(path))
        {
            // TODO Add check path is a dir, not a file
            // Comprovem que el directori no tingui cap item que es digui DICOM o DICOMDIR
            m_copyFolderContentValidationLabel->setText(tr("This folder has invalid content. The folder cannot contain any item called DICOM or DICOMDIR."));
            m_copyFolderContentValidationLabel->setVisible(true);
            m_copyFolderContentValidationIcon->setVisible(true);
            return false;
        }
    }
    else
    {
        m_copyFolderContentValidationLabel->setText(tr("Please, enter the path of the folder to copy to DICOMDIR."));
        m_copyFolderContentValidationLabel->setVisible(true);
        m_copyFolderContentValidationIcon->setVisible(true);
        return false;
    }

    m_copyFolderContentValidationLabel->setVisible(false);
    m_copyFolderContentValidationIcon->setVisible(false);
    return true;
}

void QDICOMDIRConfigurationScreen::examinateDICOMDIRBurningApplicationPath()
{
    Settings settings;

    // A la pàgina de QT indica que en el cas que nomes deixem seleccionar un fitxer, agafar el primer element de la llista i punt, no hi ha cap mètode que
    // te retornin directament el fitxer selccionat
    QFileDialog *dialog = new QFileDialog(0, QFileDialog::tr("Open"),
                                          settings.getValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey).toString(), "");
    dialog->setFileMode(QFileDialog::ExistingFile);

    if (dialog->exec() == QDialog::Accepted)
    {
        if (!dialog->selectedFiles().empty())
        {
            QString burningApplicationPath = dialog->selectedFiles().takeFirst();
            QFileInfo infoBurningApplicationFile(burningApplicationPath);
            // Es comprova si es tracta d'una aplicació de Mac i en cas afirmatiu es modifica el path per tal d'indicar exactament on és l'executable
            if (infoBurningApplicationFile.isBundle())
            {
                // El path es treu una mica a lo "bruto". Per fer-ho bé s'hauria de llegir el Bundle i extreure'n
                // la localització de l'executable (CFBundleExecutable):
                // http://developer.apple.com/documentation/CoreFoundation/Conceptual/CFBundles/BundleTypes/BundleTypes.html
                burningApplicationPath = burningApplicationPath + "/Contents/MacOS/" + infoBurningApplicationFile.bundleName();
            }

            m_textBurningApplicationPath->setText(burningApplicationPath);
        }
    }
    delete dialog;
}

void QDICOMDIRConfigurationScreen::examinateDICOMDIRFolderPathToCopy()
{
    QString folderPathToCopy = QFileDialog::getExistingDirectory(0, tr("Folder to copy to DICOMDIR..."), m_textDICOMDIRFolderPathToCopy->text());

    if (!folderPathToCopy.isEmpty())
    {
        m_textDICOMDIRFolderPathToCopy->setText(QDir::toNativeSeparators(folderPathToCopy));
    }
}

void QDICOMDIRConfigurationScreen::updateExplicitLittleEndianSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::ConvertDICOMDIRImagesToLittleEndianKey, enable);
}

void QDICOMDIRConfigurationScreen::checkAndUpdateBurningApplicationPathSetting(const QString &text)
{
    if (validateBurningApplicationPath(text))
    {
        Settings settings;
        settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationPathKey, text);
        INFO_LOG("Updated burning application path: " + text);
    }
}

void QDICOMDIRConfigurationScreen::updateBurningApplicationParametersSetting()
{
    Settings settings;
    settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationParametersKey, m_textBurningApplicationParameters->text());
    INFO_LOG("Updated burning application parameters: " + m_textBurningApplicationParameters->text());
}

void QDICOMDIRConfigurationScreen::updateDifferentCDDVDParametersSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationHasDifferentCDDVDParametersKey, enable);
}

void QDICOMDIRConfigurationScreen::updateBurningApplicationCDParametersSetting()
{
    Settings settings;
    settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationCDParametersKey, m_textBurningApplicationCDParameters->text());
    INFO_LOG("Updated burning application parameters (CD specific): " + m_textBurningApplicationCDParameters->text());
}

void QDICOMDIRConfigurationScreen::updateBurningApplicationDVDParametersSetting()
{
    Settings settings;
    settings.setValue(InputOutputSettings::DICOMDIRBurningApplicationDVDParametersKey, m_textBurningApplicationDVDParameters->text());
    INFO_LOG("Updated burning application parameters (DVD specific): " + m_textBurningApplicationDVDParameters->text());
}

void QDICOMDIRConfigurationScreen::checkAndUpdateCopyFolderContentPathSetting(const QString &text)
{
    if (validateCopyFolderContentPath(text))
    {
        Settings settings;
        settings.setValue(InputOutputSettings::DICOMDIRFolderPathToCopy, text);
        INFO_LOG("Updated folder to add content from to DICOMDIR: " + text);
    }
}

void QDICOMDIRConfigurationScreen::updateCopyContentsFromUSBOrHardDiskSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::CopyFolderContentToDICOMDIROnUSBOrHardDisk, enable);
    validateCopyFolderContentPath(m_textDICOMDIRFolderPathToCopy->text());
    updateCopyFolderContentPathOptions();
}

void QDICOMDIRConfigurationScreen::updateCopyContentsFromCDOrDVDSetting(bool enable)
{
    Settings settings;
    settings.setValue(InputOutputSettings::CopyFolderContentToDICOMDIROnCDOrDVD, enable);
    validateCopyFolderContentPath(m_textDICOMDIRFolderPathToCopy->text());
    updateCopyFolderContentPathOptions();
}

};
