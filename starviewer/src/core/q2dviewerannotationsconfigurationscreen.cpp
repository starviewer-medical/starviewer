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

#include "q2dviewerannotationsconfigurationscreen.h"

#include "coresettings.h"
#include "q2dviewerannotationhandler.h"
#include "q2dviewerannotationsconfigurationscreenhelp.h"
#include "q2dviewerannotationsconfigurationwidget.h"
#include "q2dviewerannotationssettingshelper.h"
#include "starviewerapplication.h"

#include <QAction>
#include <QDesktopWidget>
#include <QMessageBox>
#include <QTabBar>
#include <QToolButton>

namespace udg {

Q2DViewerAnnotationsConfigurationScreen::Q2DViewerAnnotationsConfigurationScreen(QWidget *parent)
    : QWidget(parent), m_forceTabIndex(-1), m_help(nullptr)
{
    setupUi(this);

    initialize();
    createConnections();
}

Q2DViewerAnnotationsConfigurationScreen::~Q2DViewerAnnotationsConfigurationScreen()
{
    Q2DViewerAnnotationsSettingsHelper helper;
    helper.setDefaultSettings(static_cast<Q2DViewerAnnotationsConfigurationWidget*>(m_tabWidget->widget(0))->getAnnotationsSettings());

    for (int i = 1; i < m_tabWidget->count(); i++)
    {
        helper.setSettings(m_tabWidget->tabText(i), static_cast<Q2DViewerAnnotationsConfigurationWidget*>(m_tabWidget->widget(i))->getAnnotationsSettings());
    }
}

void Q2DViewerAnnotationsConfigurationScreen::initialize()
{
    Settings settings;
    m_showViewersTextualInformationCheckBox->setChecked(settings.getValue(CoreSettings::ShowViewersTextualInformation).toBool());

    Q2DViewerAnnotationsSettingsHelper helper;
    addTab("Default");
    m_tabWidget->setTabText(0, tr("Default"));
    m_tabWidget->tabBar()->setTabButton(0, QTabBar::LeftSide, nullptr); // hide close button (both sides)
    m_tabWidget->tabBar()->setTabButton(0, QTabBar::RightSide, nullptr);
    addTab("MG");
    m_tabWidget->tabBar()->setTabButton(1, QTabBar::LeftSide, nullptr);
    m_tabWidget->tabBar()->setTabButton(1, QTabBar::RightSide, nullptr);

    QStringList otherModalities = helper.getModalitiesWithSpecificSettings();
    otherModalities.removeOne("MG");

    for (const QString &modality : qAsConst(otherModalities))
    {
        addTab(modality);
    }

    QToolButton *addTabButton = new QToolButton();
    addTabButton->setDefaultAction(new QAction("+"));
    m_tabWidget->setCornerWidget(addTabButton); // button to add a new tab

    connect(addTabButton, &QToolButton::clicked, this, &Q2DViewerAnnotationsConfigurationScreen::addNewTab);
}

void Q2DViewerAnnotationsConfigurationScreen::createConnections()
{
    connect(m_showViewersTextualInformationCheckBox, &QCheckBox::toggled, [](bool checked) {
        Settings settings;
        settings.setValue(CoreSettings::ShowViewersTextualInformation, checked);
    });

    connect(m_tabWidget, &QTabWidget::currentChanged, [this] {
        if (m_forceTabIndex >= 0)
        {
            int forceTabIndex = m_forceTabIndex;
            m_forceTabIndex = -1;
            m_tabWidget->setCurrentIndex(forceTabIndex);
        }
    });

    connect(m_tabWidget, &QTabWidget::tabCloseRequested, [this](int index) {
        Q2DViewerAnnotationsSettingsHelper helper;
        helper.deleteSettings(m_tabWidget->tabText(index));
        m_tabWidget->removeTab(index);
    });
}

Q2DViewerAnnotationsConfigurationWidget *Q2DViewerAnnotationsConfigurationScreen::addTab(const QString &modality)
{
    auto tab = new Q2DViewerAnnotationsConfigurationWidget(modality);
    m_tabWidget->addTab(tab, modality);

    connect(tab, &Q2DViewerAnnotationsConfigurationWidget::modalityChanged, [=](const QString &newModality) {
        validateModalityChange(tab, newModality);
    });
    connect(tab, &Q2DViewerAnnotationsConfigurationWidget::helpRequested, this, &Q2DViewerAnnotationsConfigurationScreen::showHelp);

    return tab;
}

void Q2DViewerAnnotationsConfigurationScreen::validateModalityChange(Q2DViewerAnnotationsConfigurationWidget *tab, const QString &newModality)
{
    QString errorMessage;

    if (newModality.isEmpty())
    {
        errorMessage = tr("Modality can't be empty.");
    }
    else if (newModality.compare("Default", Qt::CaseInsensitive) == 0)
    {
        errorMessage = tr("The modality name “%1” is reserved.").arg(newModality);
    }
    else
    {
        for (int i = 1; i < m_tabWidget->count(); i++)
        {
            if (m_tabWidget->widget(i) != tab && newModality.compare(m_tabWidget->tabText(i), Qt::CaseInsensitive) == 0)
            {
                errorMessage = tr("Modality “%1” is already specified.").arg(newModality);
                break;
            }
        }
    }

    if (errorMessage.isEmpty())
    {
        int index = m_tabWidget->indexOf(tab);
        Q2DViewerAnnotationsSettingsHelper helper;
        helper.deleteSettings(m_tabWidget->tabText(index));
        helper.setSettings(newModality, tab->getAnnotationsSettings());
        m_tabWidget->setTabText(m_tabWidget->indexOf(tab), newModality);
        tab->updateModality();
    }
    else
    {
        QMessageBox::warning(this, ApplicationNameString, errorMessage);
        m_forceTabIndex = m_tabWidget->indexOf(tab);
        tab->restoreModality();
        tab->editModality();
    }
}

void Q2DViewerAnnotationsConfigurationScreen::addNewTab()
{
    auto tab = addTab("XX");
    m_tabWidget->setCurrentWidget(tab);
    tab->editModality();
}

void Q2DViewerAnnotationsConfigurationScreen::showHelp()
{
    bool firstShow = false;

    // The help window is created the first time. Afterwards, if the user closes the window and then requests help again, the same window is shown.
    if (!m_help)
    {
        firstShow = true;
        m_help = new Q2DViewerAnnotationsConfigurationScreenHelp(this);
        m_help->setWindowFlags(Qt::Tool);

        for (const QPair<QString, QString> &pair : Q2DViewerAnnotationHandler::getSupportedAnnotations())
        {
            m_help->addVariable("{%" + pair.first + "%}", pair.second);
        }
    }

    m_help->show();

    // Move window to top right of the screen on first show
    if (firstShow)
    {
        const QRect &screenRect = qApp->desktop()->availableGeometry(m_help);
        m_help->move(screenRect.width() - m_help->frameGeometry().width(), 0);
    }
}

} // namespace udg
