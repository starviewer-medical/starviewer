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

#include "qwindowlevelcombobox.h"
#include "qcustomwindowleveldialog.h"
#include "logging.h"
#include "voilutpresetstooldata.h"
#include "qcustomwindowleveleditwidget.h"
#include "coresettings.h"
#include "windowlevel.h"

namespace udg {

QWindowLevelComboBox::QWindowLevelComboBox(QWidget *parent)
 : QComboBox(parent), m_presetsData(0)
{
    m_customWindowLevelDialog = new QCustomWindowLevelDialog();
    m_currentSelectedPreset = "";
    connect(this, SIGNAL(activated(const QString&)), SLOT(setActiveWindowLevel(const QString&)));
    Settings settings;
    this->setMaxVisibleItems(settings.getValue(CoreSettings::MaximumNumberOfVisibleWindowLevelComboItems).toInt());
}

QWindowLevelComboBox::~QWindowLevelComboBox()
{
    delete m_customWindowLevelDialog;
}

void QWindowLevelComboBox::setPresetsData(VoiLutPresetsToolData *windowLevelData)
{
    if (m_presetsData)
    {
        // Desconectem tot el que teníem connectat aquí
        disconnect(m_presetsData, 0, this, 0);
        disconnect(m_presetsData, 0, m_customWindowLevelDialog, 0);
        disconnect(m_customWindowLevelDialog, 0, m_presetsData, 0);
    }
    m_presetsData = windowLevelData;
    populateFromPresetsData();
    connect(m_presetsData, SIGNAL(presetAdded(WindowLevel)), SLOT(addPreset(WindowLevel)));
    connect(m_presetsData, SIGNAL(presetRemoved(WindowLevel)), SLOT(removePreset(WindowLevel)));
    connect(m_presetsData, SIGNAL(presetSelected(WindowLevel)), SLOT(selectPreset(WindowLevel)));

    // TODO Això es podria substituir fent que el CustomWindowLevelDialog també contingués les dades
    // de window level i directament li fes un setCustomWindowLevel() a WindowLevelPresetsToolData
    connect(m_customWindowLevelDialog, SIGNAL(windowLevel(double, double)), m_presetsData, SLOT(setCustomWindowLevel(double, double)));
}

void QWindowLevelComboBox::clearPresets()
{
    if (m_presetsData)
    {
        // Desconectem tot el que teníem connectat aquí
        disconnect(m_presetsData, 0, this, 0);
        disconnect(m_presetsData, 0, m_customWindowLevelDialog, 0);
        disconnect(m_customWindowLevelDialog, 0, m_presetsData, 0);
    }
    this->clear();
    m_presetsData = 0;
}

void QWindowLevelComboBox::addPreset(const WindowLevel &preset)
{
    int group;
    if (m_presetsData->getGroup(preset, group))
    {
        int index;
        switch (group)
        {
            case VoiLutPresetsToolData::AutomaticPreset:
                index = m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset).count() - 1;
                break;

            case VoiLutPresetsToolData::FileDefined:
                index = m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::FileDefined).count() - 1;
                break;

            case VoiLutPresetsToolData::StandardPresets:
                index = m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::StandardPresets).count() - 1;
                break;

            case VoiLutPresetsToolData::UserDefined:
                index = m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::StandardPresets).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::UserDefined).count() - 1;
                break;

            case VoiLutPresetsToolData::Other:
                index = m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::StandardPresets).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::UserDefined).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::Other).count() - 1;
                break;

            case VoiLutPresetsToolData::CustomPreset:
                index = m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::AutomaticPreset).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::FileDefined).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::StandardPresets).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::UserDefined).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::Other).count() +
                        m_presetsData->getPresetsFromGroup(VoiLutPresetsToolData::CustomPreset).count() - 1;
                break;
        }
        this->insertItem(index, preset.getName());
    }
    else
    {
        DEBUG_LOG("El preset " + preset.getName() + " no està present en les dades de window level proporcionades");
    }

    this->selectPreset(m_currentSelectedPreset);
}

void QWindowLevelComboBox::removePreset(const WindowLevel &preset)
{
    removePreset(preset.getName());
}

void QWindowLevelComboBox::removePreset(const QString &preset)
{
    int index = this->findText(preset);
    if (index > -1)
    {
        this->removeItem(index);
    }
}

void QWindowLevelComboBox::selectPreset(const WindowLevel &preset)
{
    selectPreset(preset.getName());
}

void QWindowLevelComboBox::selectPreset(const QString &preset)
{
    int index = this->findText(preset);
    if (index > -1)
    {
        m_currentSelectedPreset = preset;
        this->setCurrentIndex(index);
    }
    else
    {
        this->setCurrentIndex(this->findText(VoiLutPresetsToolData::getCustomPresetName()));
    }
}

void QWindowLevelComboBox::populateFromPresetsData()
{
    if (!m_presetsData)
    {
        return;
    }

    this->clear();
    this->addItems(m_presetsData->getDescriptionsFromGroup(VoiLutPresetsToolData::AutomaticPreset));
    this->addItems(m_presetsData->getDescriptionsFromGroup(VoiLutPresetsToolData::FileDefined));
    this->addItems(m_presetsData->getDescriptionsFromGroup(VoiLutPresetsToolData::StandardPresets));
    this->addItems(m_presetsData->getDescriptionsFromGroup(VoiLutPresetsToolData::UserDefined));
    this->addItems(m_presetsData->getDescriptionsFromGroup(VoiLutPresetsToolData::Other));
    this->insertSeparator(this->count());
    this->addItems(m_presetsData->getDescriptionsFromGroup(VoiLutPresetsToolData::CustomPreset));
    this->addItem(tr("Edit Custom WW/WL"));
}

void QWindowLevelComboBox::setActiveWindowLevel(const QString &text)
{
    if (text == VoiLutPresetsToolData::getCustomPresetName())
    {
        // Reestablim el valor que hi havia perquè no quedi seleccionat la fila de l'editor.
        this->selectPreset(m_currentSelectedPreset);
        WindowLevel preset = m_presetsData->getCurrentPreset();
        m_customWindowLevelDialog->setDefaultWindowLevel(preset.getWidth(), preset.getCenter());
        m_customWindowLevelDialog->exec();
    }
    else if (text == tr("Edit Custom WW/WL"))
    {
        // Reestablim el valor que hi havia perquè no quedi seleccionat la fila de l'editor.
        this->selectPreset(m_currentSelectedPreset);
        WindowLevel preset = m_presetsData->getCurrentPreset();

        QCustomWindowLevelEditWidget customWindowLevelEditWidget;
        customWindowLevelEditWidget.setDefaultWindowLevel(preset);
        customWindowLevelEditWidget.exec();
    }
    else
    {
        m_presetsData->selectCurrentPreset(text);
    }
}

};
