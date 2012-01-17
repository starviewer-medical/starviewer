#include "gridicon.h"
#include <QGridLayout>
#include <QLabel>
#include <QPixmap>
#include <QResizeEvent>

#include "logging.h"

namespace udg {

GridIcon::GridIcon(QWidget *parent, const QString &iconType)
 : QFrame(parent)
{
    QGridLayout *gridLayout = new QGridLayout(this);
    gridLayout->setSpacing(0);
    gridLayout->setMargin(1);
    m_label = new QLabel(this);
    m_label->setFrameShape(QFrame::StyledPanel);
    QPixmap pixmap;

    QString path;
    if (!iconType.isEmpty())
    {
        path = QString::fromUtf8(":/images/") + iconType + ".png";
    }
    else
    {
        path = QString::fromUtf8(":/images/axial.png");
    }
    
    pixmap.load(path);

    m_label->setPixmap(pixmap);
    gridLayout->addWidget(m_label, 0, 0, 1, 1);
}

GridIcon::~GridIcon()
{
}

void GridIcon::resizeEvent(QResizeEvent *event)
{
    QFrame::resizeEvent(event);

    m_label->setPixmap(m_label->pixmap()->scaled(event->size().width() - 2, event->size().height() - 2, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    m_label->setAlignment(Qt::AlignCenter);
}

}
