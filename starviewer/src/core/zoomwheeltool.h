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

#ifndef UDGZOOMWHEELTOOL_H
#define UDGZOOMWHEELTOOL_H

#include "tool.h"

#include <QPoint>

class QTimer;

namespace udg {

class QViewer;

class ZoomWheelTool : public Tool {
Q_OBJECT
public:
    enum { None, Zooming };

    explicit ZoomWheelTool(QViewer *viewer, QObject *parent = nullptr);
    ~ZoomWheelTool() override;

    void handleEvent(unsigned long eventID) override;

private slots:
    /// \brief Unsets the cursor icon.
    void resetTool();

    /// \brief Zooms in or out at a given time (during zoom animation).
    void scalingTime();

    /// \brief Ends the zooming effect.
    void animFinished();

private:
    /// \brief Process angular wheel move event.
    void onWheelMoved(int angleDelta);

private:
    QTimer* m_timer;

    /// Some mouses produce unwanted scrolls when the wheel is clicked. This is used to cancel them until the wheel is released.
    bool m_ignoreWheelMovement;

    /// Point around which zoom is centered.
    QPoint m_zoomCenter;

    /// Number of mouse wheel increments.
    int m_increment;

};

}

#endif //UDGZOOMWHEELTOOL_H
