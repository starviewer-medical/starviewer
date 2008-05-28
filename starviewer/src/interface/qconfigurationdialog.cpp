/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "qconfigurationdialog.h"

#include "qconfigurationscreen.h"

namespace udg {

QConfigurationDialog::QConfigurationDialog(QWidget *parent, Qt::WindowFlags f)
 : QDialog(parent, f)
{
    setupUi(this);
    setWindowFlags( (this->windowFlags() | Qt::WindowMaximizeButtonHint)  ^ Qt::WindowContextHelpButtonHint  );
    QConfigurationScreen *screen = new QConfigurationScreen(this);
    this->addConfigurationWidget(screen, "PACS", AdvancedConfiguration);

    connect(screen, SIGNAL( configurationChanged(const QString &) ), this, SIGNAL( configurationChanged(const QString &) ));
    connect(m_viewAdvancedOptions, SIGNAL(stateChanged(int)), SLOT(setViewAdvancedConfiguration()));
    connect(m_okButton , SIGNAL(clicked()), screen, SLOT(applyChanges()));

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
