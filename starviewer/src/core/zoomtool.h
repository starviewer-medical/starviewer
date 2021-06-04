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

#ifndef UDGZOOMTOOL_H
#define UDGZOOMTOOL_H

#include "tool.h"

#include <QPoint>

namespace udg {

class QViewer;

class ZoomTool : public Tool {
Q_OBJECT
public:
    enum { None, Zooming };

    explicit ZoomTool(QViewer *viewer, QObject *parent = nullptr);
    ~ZoomTool() override;

    void handleEvent(unsigned long eventID) override;

private slots:
    /// Comença el zoom
    void startZoom();

    /// Calcula el nou zoom
    void doZoom();

    /// Atura l'estat de zoom
    void endZoom();

private:
    int m_state;
    /// Point around which zoom is centered.
    QPoint m_zoomCenter;

};

}

#endif
