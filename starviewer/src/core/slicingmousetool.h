/*************************************************************************************
  Copyright (C) 2017 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2017. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.
 *************************************************************************************/

#ifndef UDGSLICINGMOUSETOOL_H
#define UDGSLICINGMOUSETOOL_H

#include "slicingtool.h"

// Qt
#include <QPoint>

namespace udg {

class SlicingMouseTool : public SlicingTool {
Q_OBJECT
public:
    explicit SlicingMouseTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingMouseTool();

    virtual void handleEvent(unsigned long eventID) override;
    
public slots:
    virtual void reassignAxis() override;
    
private:
    void onMousePress(const QPoint &position);
    void onMouseMove(const QPoint &position);
    void onMouseRelease(const QPoint &position);
    
    void onCtrlPress();
    void onCtrlRelease();
    
    bool m_dragActive;
    bool m_verticalIsLikeHorizontal;
    
    enum class Direction {Undefined, Horizontal, Vertical, Oblique};
    Direction m_direction;
    
    QPoint m_lastMousePosition;
    
    static constexpr auto VERTICAL_AXIS = 0;
    static constexpr auto HORIZONTAL_AXIS = 1;
};

}

#endif //UDGSLICINGMOUSETOOL_H
