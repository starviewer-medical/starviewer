#ifndef UDGAUTOMATICSYNCHRONIZATIONMANAGER_H
#define UDGAUTOMATICSYNCHRONIZATIONMANAGER_H

#include "tool.h"

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

    /// Inicialitza la sincronització autmàtica
    void initialize();

private slots:
    void setActiveWidget(Q2DViewer *viewer);

private:
    /// Dades compartides de la tool automatica de sincronitzacio
    AutomaticSynchronizationToolData *m_toolData;
    
    /// Layaout que conté tots els viewers de l'extensio
    ViewersLayout *m_viewersLayout;
};

}

#endif
