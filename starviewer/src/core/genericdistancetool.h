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

#ifndef UDGGENERICDISTANCETOOL_H
#define UDGGENERICDISTANCETOOL_H

#include "measurementtool.h"

#include <QPointer>

namespace udg {

class DrawerLine;
class DrawerText;

/**
    Abstract class to be superclass of distance-kind tools
 */
class GenericDistanceTool : public MeasurementTool {
Q_OBJECT
public:
    GenericDistanceTool(QViewer *viewer, QObject *parent = 0);
    ~GenericDistanceTool();

    void handleEvent(long unsigned eventID);

protected:
    /// These methods should be implemented by its subclasses giving the desired behaviour for each event
    virtual void handleLeftButtonPress() = 0;
    virtual void handleMouseMove() = 0;
    /// Abstract method for aborting the current drawn shapes and remove them
    virtual void abortDrawing() = 0;

    /// Handles the key press event. Basically it invokes abortDrawing() when Esc key is pressed
    void handleKeyPress();
    
    MeasureComputer* getMeasureComputer() const override;
    
    /// Draws the corresponding measurment
    void drawMeasurement();
    
    /// Places the measurement text in an proper position relative to the drawn distance line
    void placeMeasurementText(DrawerText *text);

protected:
    /// DrawerLine that holds the drawn distance
    QPointer<DrawerLine> m_distanceLine;
};

} // End namespace udg

#endif
