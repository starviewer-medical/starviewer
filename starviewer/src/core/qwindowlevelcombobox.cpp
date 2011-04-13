#include "qwindowlevelcombobox.h"
#include "qcustomwindowleveldialog.h"
#include "logging.h"
#include "windowlevelpresetstooldata.h"
#include "qcustomwindowleveleditwidget.h"
#include "coresettings.h"

namespace udg {

QWindowLevelComboBox::QWindowLevelComboBox(QWidget *parent)
 : QComboBox(parent), m_presetsData(0)
{
    m_customWindowLevelDialog = new QCustomWindowLevelDialog();
    m_currentSelectedPreset = "";
    connect(this, SIGNAL(activated(const QString &)), SLOT(setActiveWindowLevel(const QString &)));
    Settings settings;
    this->setMaxVisibleItems(settings.getValue(CoreSettings::MaximumNumberOfVisibleWindowLevelComboItems).toInt());
}

QWindowLevelComboBox::~QWindowLevelComboBox()
{
    delete m_customWindowLevelDialog;
}

void QWindowLevelComboBox::setPresetsData(WindowLevelPresetsToolData *windowLevelData)
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
    connect(m_presetsData, SIGNAL(presetAdded(QString)), SLOT(addPreset(QString)));
    connect(m_presetsData, SIGNAL(presetRemoved(QString)), SLOT(removePreset(QString)));
    connect(m_presetsData, SIGNAL(presetChanged(QString)), SLOT(selectPreset(QString)));
    connect(m_presetsData, SIGNAL(currentWindowLevel(double, double)), m_customWindowLevelDialog, SLOT(setDefaultWindowLevel(double, double)));

    // TODO Això es podria substituir fent que el CustomWindowLevelDialog també contingués les dades
    // de window level i directament li fes un setCustomWindowLevel() a WindowLevelPresetsToolData
    connect(m_customWindowLevelDialog, SIGNAL(windowLevel(double,double)), m_presetsData, SLOT(setCustomWindowLevel(double,double)));
}

void QWindowLevelComboBox::clearPresets()
{
    if (m_presetsData)
    {
        // desconectem tot el que teníem connectat aquí
        disconnect(m_presetsData, 0, this, 0);
        disconnect(m_presetsData, 0, m_customWindowLevelDialog, 0);
        disconnect(m_customWindowLevelDialog, 0, m_presetsData, 0);
    }
    this->clear();
    m_presetsData = 0;
}

void QWindowLevelComboBox::addPreset(const QString &preset)
{
    int group;
    if (m_presetsData->getGroup(preset, group))
    {
        int index;
        switch (group)
        {
            case WindowLevelPresetsToolData::AutomaticPreset:
                index = m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() - 1;
                break;
            
            case WindowLevelPresetsToolData::FileDefined:
                index = m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined).count() - 1;
                break;

            case WindowLevelPresetsToolData::StandardPresets:
                index = m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() - 1;
                break;

            case WindowLevelPresetsToolData::UserDefined:
                index = m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::UserDefined).count() - 1;
                break;

            case WindowLevelPresetsToolData::Other:
                index = m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() +            m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::UserDefined).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::Other).count() - 1;
                break;
            
            case WindowLevelPresetsToolData::CustomPreset:
                index = m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::StandardPresets).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::UserDefined).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::Other).count() + m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::CustomPreset).count() - 1;
                break;
        }
        this->insertItem(index, preset);
    }
    else
    {
        DEBUG_LOG("El preset " + preset + " no està present en les dades de window level proporcionades");
    }

    this->selectPreset(m_currentSelectedPreset);
}

void QWindowLevelComboBox::removePreset(const QString &preset)
{
    int index = this->findText(preset);
    if (index > -1)
    {
        this->removeItem(index);
    }
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
        this->setCurrentIndex(this->findText(tr("Custom")));
    }
}

void QWindowLevelComboBox::populateFromPresetsData()
{
    if (!m_presetsData)
    {
        return;
    }

    this->clear();
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::AutomaticPreset));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::FileDefined));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::StandardPresets));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::UserDefined));
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::Other));
    this->insertSeparator(this->count());
    this->addItems(m_presetsData->getDescriptionsFromGroup(WindowLevelPresetsToolData::CustomPreset));
    this->addItem(tr("Edit Custom WW/WL"));
}

void QWindowLevelComboBox::setActiveWindowLevel(const QString &text)
{
    if (text == tr("Custom"))
    {
        m_customWindowLevelDialog->exec();
    }
    else if (text == tr("Edit Custom WW/WL"))
    {
        // Reestablim el valor que hi havia perquè no quedi seleccionat la fila de l'editor.
        this->selectPreset(m_currentSelectedPreset);

        double width;
        double level;
        m_presetsData->getWindowLevelFromDescription(m_presetsData->getCurrentPreset(), width, level);

        QCustomWindowLevelEditWidget customWindowLevelEditWidget;
        customWindowLevelEditWidget.setDefaultWindowLevel(width, level);
        customWindowLevelEditWidget.exec();
    }
    else
    {
        m_presetsData->activatePreset(text);
    }
}

};

