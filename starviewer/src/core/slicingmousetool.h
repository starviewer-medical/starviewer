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
    
    // Que miri l'estat del control... i depenent d'aixo faci una cosa o una altra...
    // Sempre hi han 2 exios
    virtual void reassignAxis() override;

protected:
    enum class Direction {Undefined, Vertical, Horizontal};
   
private:
    
    void onMousePress(const QPoint &position);
    void onMouseMove(const QPoint &position);
    void onMouseRelease(const QPoint &position);
    
    void updateCursorIcon(const QPoint &position);
    void unsetCursorIcon();
    
    Direction directionDetection(const QPoint& startPosition, const QPoint& currentPosition) const;
    void beginDirectionDetection(const QPoint& startPosition);
    
    double scroll(const QPoint& startPosition, const QPoint& currentPosition);
    void beginScroll(const QPoint& startPosition);
    
    Direction getDirection(const QPointF &startPosition, const QPointF &currentPosition, double stepLength = 0, double xWeight = 1, double yWeight = 1) const;
    
    
    bool m_dragActive = false;
    bool m_loopEnabled = false;
    bool m_cursorWrapAroundEnabled = false;
    
    bool m_cursorWrappedAroundToLeft = false;
    bool m_cursorWrappedAroundToRight = false;
    bool m_cursorWrappedAroundToTop = false;
    bool m_cursorWrappedAroundToBottom = false;
    QPoint m_positionBeforeWrappingAround;
    
    QPoint m_cursorIcon_lastPosition = QPoint(0,0);
    int m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
    /// Default value to avoid a cursor icon change.
    static constexpr int CURSOR_ICON_DONT_UPDATE = -1;
    
    double m_stepLength = 0;
    QPoint m_startPosition = QPoint(0,0);
    double m_startLocation = 0;
    
    double m_directionStepLength = 0;
    QPoint m_directionStartPosition = QPoint(0,0);
    
    Direction m_currentDirection = Direction::Undefined;
    
    static constexpr auto VERTICAL_AXIS = 0;
    static constexpr auto HORIZONTAL_AXIS = 1;
    
    static constexpr unsigned int DEFAULT_MINIMUM_STEP_LENGTH = 2;
    static constexpr unsigned int DEFAULT_MAXIMUM_STEP_LENGTH = 64;
    static constexpr unsigned int DEFAULT_DETECTION_STEP_LENGTH = 16;
};

}

#endif //UDGSLICINGMOUSETOOL_H
