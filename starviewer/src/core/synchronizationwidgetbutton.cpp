#include "synchronizationwidgetbutton.h"
#include "logging.h"

#include <QMouseEvent>

namespace udg {

SynchronizationWidgetButton::SynchronizationWidgetButton(QWidget *parent)
 : QLabel(parent)
{
    setAutoFillBackground(true);    
}

SynchronizationWidgetButton::~SynchronizationWidgetButton()
{

}

void SynchronizationWidgetButton::mousePressEvent(QMouseEvent *mouseEvent)
{
    Q_UNUSED(mouseEvent);
    emit buttonPress();
}

}
