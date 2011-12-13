#ifndef UDGAUTOMATICSYNCHRONIZATIONMANAGER_H
#define UDGAUTOMATICSYNCHRONIZATIONMANAGER_H

#include "tool.h"
#include "synchronizationeditionwidget.h"
namespace udg {

class AutomaticSynchronizationToolData;
class ViewersLayout;
class Q2DViewer;

/**
    Tool de sincronització automatica entre visualitzadors.
*/
class AutomaticSynchronizationManager : public QObject {
Q_OBJECT

public:
    /// Constructor i destructor
    AutomaticSynchronizationManager(AutomaticSynchronizationToolData *toolData, ViewersLayout *layout, QObject *parent = 0);
    ~AutomaticSynchronizationManager();

    /// Activar l'edicio de la sincronització automàtica
    void enableEditor(bool enable);

    /// Inicialitza la sincronització autmàtica
    void initialize();

private:
    /// Posa l'estat corresponent al widget d'edicio
    void setWidgetEditionState(Q2DViewer *viewer, SynchronizationEditionWidget::ViewerEditionState state);

private slots:
    /// Canvi l'estat del widget en edicio segons el seu estat actual
    void changeEditionState(Q2DViewer *viewer);

    /// Actualitza l'estat del widget d'edicio dels diferents viewers segons les dades acutals de la tool
    void updateEditionStateOfViewers();

    /// Actualitza l'estat del widget d'edicio del viewer segons les dades actuals de la tool
    void updateEditionStateOfViewer(Q2DViewer *viewer);

private:
    /// Dades compartides de la tool automatica de sincronitzacio
    AutomaticSynchronizationToolData *m_toolData;
    
    /// Layaout que conté tots els viewers de l'extensio
    ViewersLayout *m_viewersLayout;
};

}

#endif
