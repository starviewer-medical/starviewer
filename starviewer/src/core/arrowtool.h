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

#ifndef UDG_ARROWTOOL_H
#define UDG_ARROWTOOL_H

#include "tool.h"

#include <QPointer>

namespace udg {

class DrawerArrow;
class Q2DViewer;

/**
 * @brief The ArrowTool class allows to draw an arrow.
 */
class ArrowTool : public Tool
{
    Q_OBJECT

public:
    explicit ArrowTool(QViewer *viewer, QObject *parent = nullptr);
    ~ArrowTool() override;

    void handleEvent(unsigned long eventId) override;

private slots:
    /// Resets the tool state.
    void initialize();

private:
    void onClick();
    void onMouseMove();
    void onKeyPress();

    /// Sets both points of the arrow to the same depth.
    void equalizeDepth();

private:
    /// 2D viewer where the tool is working.
    Q2DViewer *m_2DViewer;
    /// Holds the arrow while the tool is drawing.
    QPointer<DrawerArrow> m_arrow;
    /// True if the tool is currently drawing and false otherwise.
    bool m_drawing;
};

} // namespace udg

#endif // UDG_ARROWTOOL_H
