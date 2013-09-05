#ifndef UDGVOXEL_H
#define UDGVOXEL_H

#include <QVector>

namespace udg {

/**
    Class to represent a Voxel of any dimension.
    Initially, the voxel has no values. To set a monochrome voxel, set its value through addComponent() method.
    To set an RGB voxel, call addComponent() three times. To set a voxel with n-components, call addComponent() n times, an so on.
 */
class Voxel {
public:
    Voxel();
    ~Voxel();

    /// Adds a new component to the voxel. The number of components will be increaseed by one after this method has been called.
    void addComponent(double x);

    /// Gets the i-th component of the voxel. If i is out of range, NaN will be returned.
    double getComponent(int i) const;
    
    /// Returns the number of components of the voxel.
    int getNumberOfComponents() const;
    
    /// True when the number of components is 0, false otherwise.
    bool isEmpty() const;
    
    /// Resets the voxel to its initial state, so it has no components.
    void reset();
    
    /// Returns the voxel values properly formatted as a string.
    QString getAsQString();

private:
    /// Vector containing the values of each component of the voxel.
    QVector<double> m_values;
};

} // End namespace udg

#endif
