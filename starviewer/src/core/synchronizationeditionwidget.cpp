#include "synchronizationeditionwidget.h"

#include "q2dviewer.h"

namespace udg {

SynchronizationEditionWidget::SynchronizationEditionWidget(QWidget *parent)
 : QFrame(parent)
{
    setupUi(this);
    setAutoFillBackground(true);

    m_icon->setAttribute(Qt::WA_TranslucentBackground);
    m_icon->setEnabled(false);

    connect(m_icon, SIGNAL(buttonPress()), SLOT(buttonPress()));
}

SynchronizationEditionWidget::~SynchronizationEditionWidget()
{

}

void SynchronizationEditionWidget::setBackgroundImage(const QString &urlImage)
{
    setStyleSheet(QString::fromUtf8("background-color: rgba(75, 75, 75, 0);\n background-image:url(\"%1\");").arg(urlImage));
}

void SynchronizationEditionWidget::setState(ViewerEditionState state)
{
    QPixmap pixmap;

    switch (state)
    {
        case ToQuit:
            pixmap.load(QString::fromUtf8(":/images/minus.png"));
            break;

        case ToAdd:
            pixmap.load(QString::fromUtf8(":/images/plus.png"));
            break;

        case Selected:
            pixmap.load(QString::fromUtf8(":/images/lock.png"));
            break;

        case AddedInOtherGroup:
            pixmap.load(QString::fromUtf8(":/images/minuslock.png"));
            break;
    }

    m_icon->setPixmap(pixmap.scaled(width() * 0.4, height() * 0.4, Qt::KeepAspectRatio, Qt::SmoothTransformation));
}

bool SynchronizationEditionWidget::event(QEvent *event)
{
    if (event->type() == QEvent::Enter)
    {
        m_icon->setEnabled(true);
    }
    if (event->type() == QEvent::Leave)
    {
        m_icon->setEnabled(false);
    }
    else if (event->type() == QEvent::MouseButtonPress)
    {
        Q2DViewer *viewer = dynamic_cast<Q2DViewer*>(parent());
        emit selectedItem(viewer);
    }

    return QFrame::event(event);
}

void SynchronizationEditionWidget::buttonPress()
{
    Q2DViewer *viewer = dynamic_cast<Q2DViewer*>(parent());
    emit buttonPress(viewer);
}

}
