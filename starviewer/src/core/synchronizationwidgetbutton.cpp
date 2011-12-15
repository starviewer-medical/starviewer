#include "synchronizationwidgetbutton.h"
#include "logging.h"

#include <QEvent>

namespace udg {

SynchronizationWidgetButton::SynchronizationWidgetButton(QWidget *parent)
 : QLabel(parent)
{
    setAutoFillBackground(true);    
}

SynchronizationWidgetButton::~SynchronizationWidgetButton()
{

}

bool SynchronizationWidgetButton::event(QEvent *event)
{
    if (event->type() == QEvent::MouseButtonPress)
    {
        emit buttonPress();
        event->accept();
    }
    else
    {
        return QFrame::event(event);
    }
}

}
