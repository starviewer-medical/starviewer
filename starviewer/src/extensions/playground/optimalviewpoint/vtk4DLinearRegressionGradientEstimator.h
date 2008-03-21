/*=========================================================================

  Module:    $RCSfile: vtk4DLinearRegressionGradientEstimator.h,v $

  Copyright (c) Marc Ruiz Altisent

=========================================================================*/

// .NAME vtk4DLinearRegressionGradientEstimator - Use 4D linear regression to
//  estimate gradient.
//
// .SECTION Description
//  vtk4DLinearRegressionGradientEstimator is a concrete subclass of
//  vtkEncodedGradientEstimator that uses a 4D linear regression technique to
//  estimate the gradient.
//
// .SECTION see also
// vtkEncodedGradientEstimator, vtkFiniteDifferenceGradientEstimator

#ifndef __vtk4DLinearRegressionGradientEstimator_h
#define __vtk4DLinearRegressionGradientEstimator_h

#include "vtkEncodedGradientEstimator.h"

class vtk4DLinearRegressionGradientEstimator : public vtkEncodedGradientEstimator
{
public:
  vtkTypeRevisionMacro(vtk4DLinearRegressionGradientEstimator,vtkEncodedGradientEstimator);
  void PrintSelf( ostream& os, vtkIndent indent );

  // Description:
  // Construct a vtk4DLinearRegressionGradientEstimator.
  static vtk4DLinearRegressionGradientEstimator *New();

protected:
  vtk4DLinearRegressionGradientEstimator();
  ~vtk4DLinearRegressionGradientEstimator();


  // Description:
  // Recompute the encoded normals and gradient magnitudes.
  void UpdateNormals( void );
private:
  vtk4DLinearRegressionGradientEstimator(const vtk4DLinearRegressionGradientEstimator&);  // Not implemented.
  void operator=(const vtk4DLinearRegressionGradientEstimator&);  // Not implemented.
}; 


#endif
