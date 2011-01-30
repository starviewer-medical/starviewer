#ifndef UDGAPPLYHANGINGPROTOCOLQVIEWERCOMMAND_H
#define UDGAPPLYHANGINGPROTOCOLQVIEWERCOMMAND_H

#include "qviewercommand.h"

namespace udg {

class Q2DViewerWidget;
class Q2DViewer;
class HangingProtocolDisplaySet;

/**
  Command de Q2Viewer que s'encarrega d'aplicar un hangin protocol a un viewer un cop aquest s'ha inicialitzat
  */

class ApplyHangingProtocolQViewerCommand : public QViewerCommand
{
    Q_OBJECT
public:
    ApplyHangingProtocolQViewerCommand(Q2DViewerWidget *viewer, HangingProtocolDisplaySet *displaySet, QObject *parent = 0);

public slots:
    void execute();

private:
    /// Aplicar les transformacions (rotacions, flips..) per mostrar per pantalla
    void applyDisplayTransformations(Q2DViewer *viewer, HangingProtocolDisplaySet *displaySet);

private:
    HangingProtocolDisplaySet *m_displaySet;
    Q2DViewerWidget *m_viewerWidget;
};

} // End namespace udg

#endif // UDGAPPLYHANGINGPROTOCOLQVIEWERCOMMAND_H
