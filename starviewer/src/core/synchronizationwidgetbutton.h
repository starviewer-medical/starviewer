#ifndef SYNCHRONIZATIONWIDGETBUTTON_H
#define SYNCHRONIZATIONWIDGETBUTTON_H

#include <QLabel>
#include <QWidget>

namespace udg {

/**
    Botó de la interficie del widget de sincronització automàtica
 */

class SynchronizationWidgetButton : public QLabel{
Q_OBJECT
public:

    SynchronizationWidgetButton(QWidget *parent = 0);
    ~SynchronizationWidgetButton();

public:

    /// Tractament dels events
    bool event(QEvent *event);

signals:

    /// Signal que s'emet al seleccionar el widget
    void buttonPress();

};

};

#endif
