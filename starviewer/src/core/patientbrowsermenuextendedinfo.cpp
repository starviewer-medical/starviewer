#include "patientbrowsermenuextendedinfo.h"

#include "patientbrowsermenuextendeditem.h"

#include <QVBoxLayout>
#include <QEvent>

namespace udg {

PatientBrowserMenuExtendedInfo::PatientBrowserMenuExtendedInfo(QWidget *parent)
: QFrame(parent)
{
    m_layout = new QVBoxLayout(this);
    m_layout->addWidget(new PatientBrowserMenuExtendedItem(this));
    m_layout->setMargin(0);
}

PatientBrowserMenuExtendedInfo::~PatientBrowserMenuExtendedInfo()
{
}

void PatientBrowserMenuExtendedInfo::setItems(const QList<PatientBrowserMenuExtendedItem*> &items)
{
    clearLayout(m_layout);

    int maxWidth = 0;
    int accumulatedHeight = 0;
    foreach(PatientBrowserMenuExtendedItem *item, items)
    {
        item->adjustSize();
        m_layout->addWidget(item);
        maxWidth = qMax(maxWidth, item->width());
        accumulatedHeight += item->height();
    }

    // Size needs to be explicitly defined
    setFixedHeight(accumulatedHeight);
    setFixedWidth(maxWidth);
}


bool PatientBrowserMenuExtendedInfo::event(QEvent *event)
{
    // Si s'ha pulsat l'escape
    if (event->type() == QEvent::Close)
    {
        emit closed();
    }
    return QFrame::event(event);
}

void PatientBrowserMenuExtendedInfo::clearLayout(QLayout* layout, bool deleteWidgets)
{
    while (QLayoutItem *item = layout->takeAt(0))
    {
        if (deleteWidgets)
        {
            if (QWidget *widget = item->widget())
            {
                delete widget;
            }
        }
        if (QLayout *childLayout = item->layout())
        {
            clearLayout(childLayout, deleteWidgets);
        }
        delete item;
    }
}

}
