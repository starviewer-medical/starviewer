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

#ifndef UDGRENDERQVIEWERCOMMAND_H
#define UDGRENDERQVIEWERCOMMAND_H

#include "qviewercommand.h"

namespace udg {

class Q2DViewer;

/**
    Command de Q2Viewer que realitza un render() del viewer.
  */
class RenderQViewerCommand : public QViewerCommand {
Q_OBJECT
public:
    RenderQViewerCommand(Q2DViewer *viewer, QObject *parent = 0);

public slots:
    void execute();

private:
    Q2DViewer *m_viewer;
};

}

#endif // UDGRENDERQVIEWERCOMMAND_H
