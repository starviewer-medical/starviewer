#ifndef SYNCHRONIZATIONEDITIONWIDGET_H
#define SYNCHRONIZATIONEDITIONWIDGET_H

#include "ui_synchronizationeditionwidgetbase.h"

namespace udg {

class Q2DViewer;

/**
    Widget que ens permet mostrar l'estat de cada visor amb la sincronització automàtica i interactuar amb l'usuari
 */
class SynchronizationEditionWidget : public QFrame, private Ui::SynchronizationEditionWidgetBase {
Q_OBJECT
public:
    /// Possibles estats del widget durant el procés d'edició
    enum ViewerEditionState { Selected, ToQuit, ToAdd, AddedInOtherGroup };
    
    SynchronizationEditionWidget(QWidget *parent = 0);
    ~SynchronizationEditionWidget();

public:
    /// Tractament dels events
    bool event(QEvent *event);

    /// Posa la icona corresponent a l'estat del widget
    void setState(ViewerEditionState state);

    /// Posa la imatge de fons al widget
    void setBackgroundImage(const QString &urlImage);

signals:
    /// Signal que s'emet al seleccionar el widget
    void selectedItem(Q2DViewer *viewer);

    /// Signal que s'emet al clicar el boto
    void buttonPress(Q2DViewer *viewer);

private slots:

    // Slot que tracta l'event que s'ha clicat el botó del widget.
    void buttonPress();
};

};

#endif
