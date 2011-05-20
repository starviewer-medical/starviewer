#ifndef VTK4DLINEARREGRESSIONGRADIENTESTIMATOR_H
#define VTK4DLINEARREGRESSIONGRADIENTESTIMATOR_H

#include "vtkEncodedGradientEstimator.h"

namespace udg {

/**
    És una subclasse de vtkEncodedGradientEstimator que fa servir la tècnica de la regressió lineal 4D per estimar el gradient.
  */
class Vtk4DLinearRegressionGradientEstimator : public vtkEncodedGradientEstimator {

public:
    vtkTypeRevisionMacro(Vtk4DLinearRegressionGradientEstimator, vtkEncodedGradientEstimator);

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
