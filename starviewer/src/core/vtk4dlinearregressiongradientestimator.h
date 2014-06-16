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

#ifndef VTK4DLINEARREGRESSIONGRADIENTESTIMATOR_H
#define VTK4DLINEARREGRESSIONGRADIENTESTIMATOR_H

#include "vtkEncodedGradientEstimator.h"

namespace udg {

/**
    És una subclasse de vtkEncodedGradientEstimator que fa servir la tècnica de la regressió lineal 4D per estimar el gradient.
  */
class Vtk4DLinearRegressionGradientEstimator : public vtkEncodedGradientEstimator {

public:
    vtkTypeMacro(Vtk4DLinearRegressionGradientEstimator, vtkEncodedGradientEstimator)

    /// Construeix un objecte Vtk4DLinearRegressionGradientEstimator.
    static Vtk4DLinearRegressionGradientEstimator* New();

    /// Retorna el radi d'aplicació del gradient.
    unsigned int getRadius() const;
    /// Assigna el radi d'aplicació del gradient.
    void setRadius(unsigned int getRadius);

protected:
    Vtk4DLinearRegressionGradientEstimator();
    virtual ~Vtk4DLinearRegressionGradientEstimator();

    /// Recalcula les normals codificades i les magnituds del gradient.
    virtual void UpdateNormals(void);

private:
    // Not implemented
    Vtk4DLinearRegressionGradientEstimator(const Vtk4DLinearRegressionGradientEstimator &);
    // Not implemented
    void operator =(const Vtk4DLinearRegressionGradientEstimator &);

private:
    /// Radi d'aplicació del gradient.
    unsigned int m_radius;

};

} // End namespace udg

#endif // VTK4DLINEARREGRESSIONGRADIENTESTIMATOR_H
