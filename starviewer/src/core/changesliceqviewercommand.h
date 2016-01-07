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

#ifndef UDGCHANGESLICEQVIEWERCOMMAND_H
#define UDGCHANGESLICEQVIEWERCOMMAND_H

#include "qviewercommand.h"

namespace udg {

class Q2DViewer;

/**
    Command de Q2DViewer que permet especificar un canvi de llesca.
  */
class ChangeSliceQViewerCommand : public QViewerCommand {
Q_OBJECT
public:
    enum SlicePosition { MinimumSlice, MiddleSlice, MaximumSlice, CustomSlice };

    /// Constructor que permet especificar en quina posició volem situar el viewer sense haver de saber quantes imatges té el volume
    ChangeSliceQViewerCommand(Q2DViewer *viewer, SlicePosition slice, QObject *parent = 0);

    /// Constructor que permet especificar lliurement en quina llesca volem situar el volum. En cas que la llesca indicada estigui per sota del mínim
    /// la llesca assignada serà la mínima, i en cas que estigui per sobre del màxim, s'assignarà la màxima
    ChangeSliceQViewerCommand(Q2DViewer *viewer, int slice, QObject *parent = 0);

public slots:
    void execute();

private:
    Q2DViewer *m_viewer;
    SlicePosition m_slicePosition;
    int m_customSliceNumber;
};

}

#endif // UDGCHANGESLICEQVIEWERCOMMAND_H
