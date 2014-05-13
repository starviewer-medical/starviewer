/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

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

class ApplyHangingProtocolQViewerCommand : public QViewerCommand {
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
