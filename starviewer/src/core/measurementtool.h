#ifndef UDGMEASUREMENTTOOL_H
#define UDGMEASUREMENTTOOL_H

#include "tool.h"

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
    virtual MeasureComputer* getMeasureComputer() = 0;
    
    /// Gets the measurement string to display. The results will dependend on the specific MeasureComputer returned by the subclass
    QString getMeasurementString();
    
private:
    /// Returns the image that should be used to compute the measurements
    Image* getImageForMeasurement() const;

protected:
    /// Q2DViewer where the tool operates
    Q2DViewer *m_2DViewer;
};

} // End namespace udg

#endif
