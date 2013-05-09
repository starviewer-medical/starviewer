#ifndef UDGMEASUREMENTMANAGER_H
#define UDGMEASUREMENTMANAGER_H

#include <QString>

namespace udg {

class DrawerLine;
class Image;

class MeasurementManager {
public:
    enum MeasurementUnitsType { Pixels, Millimetres, NotAvailable };
    
    /// Returns the corresponding measurement units for the given image
    static MeasurementUnitsType getMeasurementUnits(Image *image);
    static QString getMeasurementUnitsAsQString(Image *image);

    /// Given a coordinate, returns an amended coordinate depending on the spacing of the image data and the actual pixel spacing parameters from its image
    /// If there's no pixel spacing information on the image, coordinate will be normalized upon dataSpacing
    /// If image's pixel spacing and dataSpacing are different, coordinate will be normalized upon dataSpacing and multiplied by image's pixel spacing
    /// Otherwise, coordinate will remain the same
    static double* amendCoordinate(double coordinate[3], double dataSpacing[3], Image *image);

    /// Computes the distance relative to the line, the spacing of the volume it's laying in and the source image
    static double computeDistance(DrawerLine *line, Image *image, double dataSpacing[3]);
};

} // End namespace udg

#endif
