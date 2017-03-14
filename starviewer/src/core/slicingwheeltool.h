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

#ifndef UDGSLICINGWHEELTOOL_H
#define UDGSLICINGWHEELTOOL_H

#include "slicingtool.h"

namespace udg {

class Q2DViewer;
class ToolProxy;

/**
    Tool que hereta de SlicingTool que serveix per fer slicing amb la rodeta en un visor 2D
  */
class SlicingWheelTool : public SlicingTool {
Q_OBJECT
public:
    explicit SlicingWheelTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingWheelTool();

    virtual void handleEvent(unsigned long eventID) override;
};

}

#endif
