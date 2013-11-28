#ifndef UDGFORMULACALCULATOR_H
#define UDGFORMULACALCULATOR_H

namespace udg {

class Image;
class DICOMTagReader;

/**
    Class to combine with Formula to compute a formula from the data provided from an Image or DICOMTagReader.

    When all the required data to compute the formula is available from the data stored on Image, provided image will be used.
    When Image data is not enough, the needed data should be retrieved via DICOMTagReader.

    Inherited classes should take into account wether Image or DICOMTagReader is provided.

    TODO Add pure virtual method to tell wether the formula needs data which is not present on Image (thus, DICOMTagReader must be used)
    This method could be used on setDataSource(Image*) then load automatically the related DICOMTagReader of the image.
 */
class FormulaCalculator {
public:
    FormulaCalculator();
    ~FormulaCalculator();

    /// Tells wether the formula can be computed or not with the provided data sources.
    virtual bool canCompute() = 0;

    /// Computes and returns the resulting value of running the formula with the given data
    /// If canCompute() returns false, the validity of the result is undefined.
    virtual double compute() = 0;

    /// Sets the data source of the formula
    void setDataSource(Image *image);
    void setDataSource(DICOMTagReader *tagReader);

protected:
    /// The possible data sources to compute the formula from
    Image *m_imageSource;
    DICOMTagReader *m_tagReaderSource;
};

} // End namespace udg

#endif
