/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qconfigurationdialog.h"

#ifndef STARVIEWER_LITE
#include "qconfigurationscreen.h"
#endif

#include "qlocaldatabaseconfigurationscreen.h"
#include "qlistenrisrequestsconfigurationscreen.h"
#include "starviewerapplication.h"

namespace udg {

QConfigurationDialog::QConfigurationDialog(QWidget *parent, Qt::WindowFlags f)
 : QDialog(parent, f)
{
    setupUi(this);
    setWindowTitle( tr("%1 Configuration").arg( ApplicationNameString ) );
    setWindowFlags( (this->windowFlags() | Qt::WindowMaximizeButtonHint)  ^ Qt::WindowContextHelpButtonHint  );

#ifndef STARVIEWER_LITE // no mostrem configuració del PACS
    QConfigurationScreen *pacsConfigurationScreen = new QConfigurationScreen(this);
    this->addConfigurationWidget(pacsConfigurationScreen, tr("PACS"), AdvancedConfiguration);
    connect(pacsConfigurationScreen, SIGNAL( configurationChanged(const QString &) ), this, SIGNAL( configurationChanged(const QString &) ));
    connect(m_okButton , SIGNAL(clicked()), pacsConfigurationScreen, SLOT(applyChanges()));
#endif

    // configuracions de la base de dades local
    QLocalDatabaseConfigurationScreen *localDatabaseScreen = new QLocalDatabaseConfigurationScreen(this);
    this->addConfigurationWidget(localDatabaseScreen, tr("Local Database"), AdvancedConfiguration);
    connect(localDatabaseScreen, SIGNAL( configurationChanged(const QString &) ), this, SIGNAL( configurationChanged(const QString &) ));
    connect(m_okButton , SIGNAL(clicked()), localDatabaseScreen, SLOT(applyChanges()));

#ifndef STARVIEWER_LITE // no mostrem configuració del servei que escolta les peticions del RIS
    QListenRisRequestsConfigurationScreen *qListenRisRequestsConfigurationScreen = new QListenRisRequestsConfigurationScreen(this);
    this->addConfigurationWidget(qListenRisRequestsConfigurationScreen, tr("Listen Ris Requests"), AdvancedConfiguration);
    connect(qListenRisRequestsConfigurationScreen, SIGNAL(configurationChanged(const QString &)), this, SIGNAL(configurationChanged(const QString &)));
    connect(m_okButton , SIGNAL(clicked()), qListenRisRequestsConfigurationScreen, SLOT(applyChanges()));
#endif
    
    connect(m_viewAdvancedOptions, SIGNAL(stateChanged(int)), SLOT(setViewAdvancedConfiguration()));

    m_optionsList->setCurrentRow(0);
    m_viewAdvancedOptions->setCheckState(Qt::Checked);
}

QConfigurationDialog::~QConfigurationDialog()
{
}

void QConfigurationDialog::setViewAdvancedConfiguration()
{
    foreach(QListWidgetItem *item, m_configurationListItems.values(AdvancedConfiguration))
    {
        item->setHidden( !m_viewAdvancedOptions->isChecked() );
    }
    m_optionsList->setCurrentRow(0);
}

void QConfigurationDialog::addConfigurationWidget(QWidget *widget, const QString &name, ConfigurationType type)
{
    QWidget *page = new QWidget();

    QVBoxLayout *verticalLayout = new QVBoxLayout(page);
    verticalLayout->setSpacing(6);
    verticalLayout->setMargin(9);

    QHBoxLayout *horizontalLayout = new QHBoxLayout();
    horizontalLayout->setSpacing(6);
    horizontalLayout->setMargin(0);

    QLabel *optionTitleIcon = new QLabel(page);
    optionTitleIcon->setPixmap( widget->windowIcon().pixmap(48, 48) );

    horizontalLayout->addWidget(optionTitleIcon);

    QLabel *optionTitle = new QLabel(page);
    optionTitle->setAlignment(Qt::AlignVCenter);
    optionTitle->setWordWrap(false);
    optionTitle->setText(widget->windowTitle());

    horizontalLayout->addWidget(optionTitle);

    QSpacerItem *spacerItem = new QSpacerItem(101, 20, QSizePolicy::Expanding, QSizePolicy::Minimum);

    horizontalLayout->addItem(spacerItem);

    verticalLayout->addLayout(horizontalLayout);

    QFrame *line = new QFrame(page);
    line->setFrameShape(QFrame::HLine);
    line->setFrameShadow(QFrame::Sunken);

    verticalLayout->addWidget(line);
    verticalLayout->addWidget(widget);

    m_optionsStack->addWidget(page);
    QListWidgetItem *item = new QListWidgetItem(m_optionsList);
    item->setText(name);
    item->setIcon(widget->windowIcon());

    m_configurationListItems.insert(type, item);
}

}
