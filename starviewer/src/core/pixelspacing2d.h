#ifndef UDGPIXELSPACING2D_H
#define UDGPIXELSPACING2D_H

namespace udg {

/**
    Represents a 2D pixel spacing
 */
class PixelSpacing2D {
public:
    PixelSpacing2D();
    PixelSpacing2D(double x, double y);
    ~PixelSpacing2D();
    
    /// Methods to set the values
    void setX(double x);
    void setY(double y);

    /// Methods to get the values
    double x() const;
    double y() const;

    /// Tells if it contains valid pixel spacing values.
    /// Returns false if any of its components is negative or equals 0.0, true otherwise.
    bool isValid() const;

    /// Returns true if its values are equal to the given spacing
    /// By default only precision will be until 3 decimals
    /// If decimalPrecision == -1 a standard double comparision will be performed
    bool isEqual(const PixelSpacing2D &spacing, int decimalPrecision = 3) const;

private:
    /// Spacing values
    double m_x;
    double m_y;
};

} // namespace udg

#endif // UDGPIXELSPACING2D_H
