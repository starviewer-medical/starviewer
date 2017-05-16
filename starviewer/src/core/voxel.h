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
    QString getAsQString() const;

    /// Equality operator
    bool operator==(const Voxel &voxel) const;

    /// Substraction operator. The resulting voxel will have as many components as the Voxel with the most number of components.
    /// The missing components on the corresponding side will be treated as its value is 0.
    Voxel operator+(const Voxel &value) const;

    /// Adds each component of the given voxel to this voxel. The resulting voxel will have as many components as the Voxel with the most number of components.
    /// The missing components on the corresponding side will be treated as its value is 0.
    Voxel& operator+=(const Voxel &voxel);
    
    /// Substraction operator. The resulting voxel will have as many components as the Voxel with the most number of components.
    /// The missing components on the corresponding side will be treated as its value is 0.
    Voxel operator-(const Voxel &value) const;

    /// Substracts each component of the given voxel to this voxel. The resulting voxel will have as many components as the Voxel with the most number of components.
    /// The missing components on the corresponding side will be treated as its value is 0.
    Voxel& operator-=(const Voxel &voxel);
    
    /// Multiply operator. The resulting voxel will have as many components as the Voxel with the most number of components.
    /// Each component will be multiplied by the component with the same index.
    /// The missing components on the corresponding side will be treated as its value is 1.
    Voxel operator*(const Voxel &value) const;
    
    /// Divides each component by the given value
    Voxel operator/(double value) const;

private:
    /// Vector containing the values of each component of the voxel.
    QVector<double> m_values;
};

} // End namespace udg

#endif
