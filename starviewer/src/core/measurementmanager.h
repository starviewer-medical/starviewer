#ifndef UDGMEASUREMENTMANAGER_H
#define UDGMEASUREMENTMANAGER_H

#include <QString>

namespace udg {

class Image;

class MeasurementManager {
public:
    enum MeasurementUnitsType { Pixels, Millimetres, NotAvailable };
    
    /// Returns the corresponding measurement units for the given image
    static MeasurementUnitsType getMeasurementUnits(Image *image);
    static QString getMeasurementUnitsAsQString(Image *image);
};

} // End namespace udg

#endif
