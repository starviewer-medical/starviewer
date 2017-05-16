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

#ifndef UDGCHANGEPHASEQVIEWERCOMMAND_H
#define UDGCHANGEPHASEQVIEWERCOMMAND_H

#include "qviewercommand.h"

namespace udg {

class Q2DViewer;

/**
    Command de Q2DViewer que permet especificar un canvi de fase.
  */
class ChangePhaseQViewerCommand : public QViewerCommand {
Q_OBJECT
public:
    enum PhasePosition { MinimumPhase, MiddlePhase, MaximumPhase, CustomPhase };

    /// Constructor que permet especificar en quina posició volem situar el viewer sense haver de saber quantes imatges té el volume
    ChangePhaseQViewerCommand(Q2DViewer *viewer, PhasePosition phase, QObject *parent = 0);

    /// Constructor que permet especificar lliurement en quina fase volem situar el volum. En cas que la fase indicada estigui per sota del mínim
    /// la fase assignada serà la mínima, i en cas que estigui per sobre del màxim, s'assignarà la màxima
    ChangePhaseQViewerCommand(Q2DViewer *viewer, int phase, QObject *parent = 0);

public slots:
    void execute();

private:
    Q2DViewer *m_viewer;
    PhasePosition m_phasePosition;
    int m_customPhaseNumber;
};

}

#endif // UDGCHANGEPHASEQVIEWERCOMMAND_H
