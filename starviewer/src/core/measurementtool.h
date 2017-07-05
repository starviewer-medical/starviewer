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

#ifndef UDGMEASUREMENTTOOL_H
#define UDGMEASUREMENTTOOL_H

#include "tool.h"

#include "measurementmanager.h"

namespace udg {

class QViewer;
class Q2DViewer;
class Image;
class MeasureComputer;

/**
    Tool superclass for measurement tools
 */
class MeasurementTool : public Tool {
Q_OBJECT
public:
    MeasurementTool(QViewer *viewer, QObject *parent = 0);
    ~MeasurementTool();

protected:
    /// Returns the specific measure computer for the implemented measurement tool
    virtual MeasureComputer* getMeasureComputer() const = 0;
    
    /// Gets the measurement string to display. The results will dependend on the specific MeasureComputer returned by the subclass
    QString getMeasurementString();

    /// Returns the value of the measurement.
    double getMeasurement() const;

    /// Returns the units of the measurement.
    MeasurementManager::MeasurementUnitsType getMeasurementUnits() const;
    
private:
    /// Returns the image that should be used to compute the measurements
    Image* getImageForMeasurement() const;

protected:
    /// Q2DViewer where the tool operates
    Q2DViewer *m_2DViewer;
};

} // End namespace udg

#endif
