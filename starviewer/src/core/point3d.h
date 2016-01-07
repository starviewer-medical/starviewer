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

#ifndef UDGPOINT3D_H
#define UDGPOINT3D_H

class QString;

namespace udg {

/**
    Class to represent a 3D point. Double precision.
 */
class Point3D {
public:
    Point3D();
    Point3D(double point[3]);
    Point3D(double x, double y, double z);
    ~Point3D();

    /// Sets the i-th value of the point. i must be between 0 and 2, does nothing if out of range.
    void set(int i, double value);

    /// Returns the i-th value of the point. i must be between 0 and 2, returns NaN if out of range.
    double at(int i) const;

    /// Array subscript operators to access point values
    double& operator[](int index);
    const double& operator[](int index) const;

    /// Equality operator (fuzzy)
    bool operator==(const Point3D &point);

    /// Returns the point formatted as a human readable string
    const QString getAsQString() const;
    
    /// Gets the values as a double array. Implemented for convenience.
    double* getAsDoubleArray();

private:
    /// Array with the point values
    double m_point[3];
};

} // End namespace udg

#endif
