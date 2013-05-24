#ifndef UDGMEASUREMENTMANAGER_H
#define UDGMEASUREMENTMANAGER_H

#include <QString>

namespace udg {

class DrawerLine;
class DrawerPolygon;
class Image;

/**
    This is a helper class to manage measurements safely.
    This class should be used when we want to give the right measurement for diagnostics.
    It handles the complexity that could be internally between DICOM and view spacings and coordinates.
*/
class MeasurementManager {
public:

    enum MeasurementType { Detector, Magnified, Calibrated, Physical, NoDefinedUnits, UnknownMeaning };

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

    /// Computes the area relative to the polygon, the spacing of the volume it's laying in and the source image
    static double computeArea(DrawerPolygon *polygon, Image *image, double dataSpacing[3]);

    /// Returns the default meaurement type regarding image properties, such as pixel spacing tags and modality
    static MeasurementType getDefaultMeasurementType(Image *image);
};

} // End namespace udg

#endif
