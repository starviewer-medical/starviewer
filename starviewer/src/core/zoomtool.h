#ifndef UDGZOOMTOOL_H
#define UDGZOOMTOOL_H

#include "tool.h"

namespace udg {

class QViewer;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ZoomTool : public Tool {
Q_OBJECT
public:
    enum { None, Zooming };

    ZoomTool(QViewer *viewer, QObject *parent = 0);
    ~ZoomTool();

    void handleEvent(unsigned long eventID);

private slots:
    /// Comença el zoom
    void startZoom();

    /// Calcula el nou zoom
    void doZoom();

    /// Atura l'estat de zoom
    void endZoom();

private:
    int m_state;
};

}

#endif
