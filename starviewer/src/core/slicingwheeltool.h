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

class QTimer;

namespace udg {

class SlicingWheelTool : public SlicingTool {
    Q_OBJECT
public:
    explicit SlicingWheelTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingWheelTool();

    virtual void handleEvent(unsigned long eventID) override;

public slots:
    virtual void reassignAxis() override;
    
private slots:
    void timeout();
    
private:
    bool readConfiguration();
    
    void onWheelMoved(int angleDelta);
    void onCtrlPress();
    void onCtrlRelease();
    void onMiddleButtonPress();
    void onMiddleButtonRelease();
    
    void cursorIcon(double increment);
    void unsetCursorIcon();
    
    double scroll(double increment);
    void beginScroll();
    
    QTimer* m_timer = 0;
    
    bool m_config_sliceScrollLoop = false;
    bool m_config_phaseScrollLoop  = false;
    bool m_config_volumeScroll  = false;
    
    int m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
    /// Default value to avoid a cursor icon change.
    static constexpr int CURSOR_ICON_DONT_UPDATE = -1;
    
    /** Some mouses produce unwanted scrolls when the wheel is clicked. This is
     *  used to cancel them until the wheel is released.
     */
    bool m_ignoreWheelMovement = false;
    bool m_ctrlPressed = false;
    bool m_middleButtonToggle = false;
    
    double m_increment = 0;
    unsigned int m_currentAxis = MAIN_AXIS;
    bool m_scrollLoop = false;
    bool m_volumeScroll = false;
    
    static constexpr unsigned int MAIN_AXIS = 0;
    static constexpr unsigned int SECONDARY_AXIS = 1;
};

}

#endif //UDGSLICINGWHEELTOOL_H
