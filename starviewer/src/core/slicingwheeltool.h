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

class SlicingWheelTool : public SlicingTool {
    Q_OBJECT
public:
    explicit SlicingWheelTool(QViewer *viewer, QObject *parent = 0);
    virtual ~SlicingWheelTool();

    virtual void handleEvent(unsigned long eventID) override;

public slots:
    virtual void reassignAxis() override;
    
private:
    void scroll(int steps);

    void onCtrlPress();
    void onCtrlRelease();

    void onMiddleButtonPress();
    void onMiddleButtonRelease();
    
    void updateCursorIcon(unsigned int axis, double increment);
    void unsetCursorIcon();

    bool m_sliceScrollLoop = false;
    bool m_phaseScrollLoop  = false;
    bool m_volumeScroll  = false;
    
    /** Some mouses produce unwanted scrolls when the wheel is clicked. This is
     *  used to cancel them until the wheel is released.
     */
    bool m_scrollDisabled;
    bool m_ctrlPressed;
    bool m_middleButtonToggled;
    
    int m_cursorIcon_lastIndex = CURSOR_ICON_DONT_UPDATE;
    /// Default value to avoid a cursor icon change.
    static constexpr int CURSOR_ICON_DONT_UPDATE = -1;
        
    static constexpr auto MAIN_AXIS = 0;
    static constexpr auto SECONDARY_AXIS = 1;
};

}

#endif //UDGSLICINGWHEELTOOL_H
