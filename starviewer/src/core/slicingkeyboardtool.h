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

#ifndef UDGSLICINGKEYBOARDTOOL_H
#define UDGSLICINGKEYBOARDTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class Volume;

/**
    Tool per la sincronització de llesques a partir de teclat
  */
class SlicingKeyboardTool : public Tool {
Q_OBJECT
public:
    SlicingKeyboardTool(QViewer *viewer, QObject *parent = 0);
    ~SlicingKeyboardTool();

    void handleEvent(unsigned long eventID);

private:
    /// Ens guardem aquest punter per ser més còmode
    Q2DViewer *m_2DViewer;
};

}

#endif
