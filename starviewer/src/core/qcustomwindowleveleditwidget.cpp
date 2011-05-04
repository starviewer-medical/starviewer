#include "qcustomwindowleveleditwidget.h"
#include "customwindowlevelsrepository.h"
#include "customwindowlevelswriter.h"
#include "logging.h"

#include <QMessageBox>
#include <QDoubleSpinBox>

namespace udg {

QCustomWindowLevelEditWidget::QCustomWindowLevelEditWidget(QWidget *parent)
 : QDialog(parent)
{
    setupUi(this);
    createConnections();
    loadCustomWindowLevelPresets();
}

QCustomWindowLevelEditWidget::~QCustomWindowLevelEditWidget()
{
}

void QCustomWindowLevelEditWidget::createConnections()
{
    connect(m_addWindowLevelPushButton, SIGNAL(clicked()), SLOT(addNewWindowLevel()));
    connect(m_deleteWindowLevelPushButton, SIGNAL(clicked()), SLOT(removeWindowLevel()));
    connect(m_okPushButton, SIGNAL(clicked()), SLOT(updatePresetsIfAreValid()));
    connect(m_cancelPushButton, SIGNAL(clicked()), SLOT(close()));
}

void QCustomWindowLevelEditWidget::setDefaultWindowLevel(double width, double level)
{
    m_defaultWindowWidth = width;
    m_defaultWindowLevel = level;
}

void QCustomWindowLevelEditWidget::loadCustomWindowLevelPresets()
{
    // Eliminem el que hi ha.
    int items = m_customWindowLevelTreeWidget->topLevelItemCount();
    for (int i = 0; i < items; i++)
    {
        delete m_customWindowLevelTreeWidget->takeTopLevelItem(0);
    }

    foreach (CustomWindowLevel *customWindowLevel, CustomWindowLevelsRepository::getRepository()->getItems())
    {
        addWindowLevelItem(customWindowLevel->getName(), customWindowLevel->getWidth(), customWindowLevel->getLevel());
    }
    m_addWindowLevelPushButton->setFocus();
}

void QCustomWindowLevelEditWidget::addNewWindowLevel()
{
    QTreeWidgetItem* item = addWindowLevelItem("", m_defaultWindowWidth, m_defaultWindowLevel);
    m_customWindowLevelTreeWidget->editItem(item, 2);
    m_customWindowLevelTreeWidget->scrollToItem(item);
}

QTreeWidgetItem* QCustomWindowLevelEditWidget::addWindowLevelItem(const QString &description, double width, double level)
{
    QTreeWidgetItem* item = new QTreeWidgetItem(m_customWindowLevelTreeWidget);
    item->setFlags(Qt::ItemIsEditable | Qt::ItemIsEnabled | Qt::ItemIsSelectable);

    QDoubleSpinBox *widthSpinBox = new QDoubleSpinBox(this);
    QDoubleSpinBox *levelSpinBox = new QDoubleSpinBox(this);
    widthSpinBox->setRange(-131070, 131070);
    levelSpinBox->setRange(-131070, 131070);
    widthSpinBox->setValue(width);
    levelSpinBox->setValue(level);

    m_customWindowLevelTreeWidget->setItemWidget(item, 0, widthSpinBox);
    m_customWindowLevelTreeWidget->setItemWidget(item, 1, levelSpinBox);
    item->setText(2, description);

    return item;
}

void QCustomWindowLevelEditWidget::removeWindowLevel()
{
    if (m_customWindowLevelTreeWidget->selectedItems().size() > 0)
    {
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText(tr("Selected Custom WW/WL will be deleted."));
        messageBox.setInformativeText(tr("Do you want to continue?"));
        messageBox.setStandardButtons(QMessageBox::Ok | QMessageBox::Cancel);
        messageBox.setDefaultButton(QMessageBox::Cancel);
        int answer = messageBox.exec();
        if (answer == QMessageBox::Ok)
        {
            QTreeWidgetItemIterator iterator(m_customWindowLevelTreeWidget);
            int i = 0;
            while (*iterator)
            {
                if ((*iterator)->isSelected())
                {
                    delete m_customWindowLevelTreeWidget->takeTopLevelItem(i);
                }
                else
                {
                    //Només incrementem el contador quan no l'eliminem perquè els indexs es redueixen.
                    i++;
                    ++iterator;
                }
            }
         }
    }
}

void QCustomWindowLevelEditWidget::updatePresetsIfAreValid()
{
    if (validate())
    {
        CustomWindowLevelsRepository::getRepository()->cleanUp();
        INFO_LOG("S'actualitza el repositori de Window Level amb els següents WW/WL:");

        QTreeWidgetItemIterator iterator(m_customWindowLevelTreeWidget);
        while (*iterator)
        {
            CustomWindowLevel *customWindowLevel = new CustomWindowLevel();
            customWindowLevel->setWidth(qobject_cast<QDoubleSpinBox*>(m_customWindowLevelTreeWidget->itemWidget((*iterator), 0))->value());
            customWindowLevel->setLevel(qobject_cast<QDoubleSpinBox*>(m_customWindowLevelTreeWidget->itemWidget((*iterator), 1))->value());
            customWindowLevel->setName((*iterator)->text(2));
            CustomWindowLevelsRepository::getRepository()->addItem(customWindowLevel);
            INFO_LOG(QString("-> Descripcio: %1, WW/WL: %2 / %3").arg(customWindowLevel->getName()).arg(customWindowLevel->getWidth()).arg(customWindowLevel->getLevel()));
            ++iterator;
        }
        INFO_LOG("Fi de llistat del repositori de WW/WL");

        // TODO Caldria pensar bé a on s'ha de fer això.
        CustomWindowLevelsWriter customWindowLevelsWriter;
        customWindowLevelsWriter.write();
        this->close();
    }
    else
    {
        QMessageBox messageBox;
        messageBox.setIcon(QMessageBox::Warning);
        messageBox.setText("Empty or duplicated descriptions are not allowed. Please, correct it and try again.");
        messageBox.exec();
    }
}

bool QCustomWindowLevelEditWidget::validate() const
{
    bool valid = true;
    QTreeWidgetItemIterator iterator(m_customWindowLevelTreeWidget);

    while ((*iterator) && valid)
    {
        QString itemDescription = (*iterator)->text(2);
        QRegExp emptyOrJustSpacesExpression("\\s*");

        if (emptyOrJustSpacesExpression.exactMatch(itemDescription))
        {
            valid = false;
        }
        else
        {
            QTreeWidgetItemIterator items(m_customWindowLevelTreeWidget);
            while ((*items) && valid)
            {
                if ((*items) != (*iterator))
                {
                    if (QString::compare(itemDescription, (*items)->text(2)) == 0)
                    {
                        valid = false;
                    }
                }
                ++items;
            }
        }
        ++iterator;
    }
    return valid;
}

};
