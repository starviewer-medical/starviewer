#ifndef AUTOMATICSYNCHRONIZATIONWIDGET_H
#define AUTOMATICSYNCHRONIZATIONWIDGET_H

#include "ui_automaticsynchronizationwidgetbase.h"

#include "automaticsynchronizationmanager.h"

namespace udg {

/**
    Widget que ens permet mostrar l'estat de cada visor amb la sincronització automàtica i interactuar amb l'usuari
 */

class AutomaticSynchronizationWidget : public QFrame, private Ui::AutomaticSynchronizationWidgetBase {
Q_OBJECT
public:

    AutomaticSynchronizationWidget(QWidget *parent = 0);
    ~AutomaticSynchronizationWidget();

public:

    /// Tractament dels events
    bool event(QEvent *event);

    /// Posa la icona corresponent a l'estat del widget
    void setState(AutomaticSynchronizationManager::ViewerEditionState state);

    /// Posa la imatge de fons al widget
    void setBackgroundImage(QString urlImage);

signals:

    /// Signal que s'emet al seleccionar el widget
    void selectedItem(Q2DViewer *viewer);

};

};

#endif
