/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkThresholdSegmentationLevelSetFunction.txx,v $
  Language:  C++
  Date:      $Date: 2004/09/27 18:37:55 $
  Version:   $Revision: 1.7 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkErfcLevelSetFunction_txx_
#define __itkErfcLevelSetFunction_txx_

#include "itkErfcLevelSetFunction.h"
#include "itkImageRegionIterator.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkLaplacianImageFilter.h"
#include "itkImageFileWriter.h"

namespace itk {

template <class TImageType, class TFeatureImageType, class TMaskImageType>
void ErfcLevelSetFunction<TImageType, TFeatureImageType, TMaskImageType>
::CalculateSpeedImage()
{
  typename GradientAnisotropicDiffusionImageFilter<TFeatureImageType, TFeatureImageType>::Pointer
    diffusion  = GradientAnisotropicDiffusionImageFilter<TFeatureImageType, TFeatureImageType>::New();
  typename LaplacianImageFilter<TFeatureImageType, TFeatureImageType>::Pointer
    laplacian = LaplacianImageFilter<TFeatureImageType, TFeatureImageType>::New();

  ImageRegionIterator<FeatureImageType> lit;
  ImageRegionConstIterator<FeatureImageType>
    fit(this->GetFeatureImage(), this->GetFeatureImage()->GetRequestedRegion());
  ImageRegionConstIterator<MaskImageType>
    mit(this->GetMaskImage(), this->GetMaskImage()->GetRequestedRegion());
  ImageRegionIterator<ImageType>
    sit(this->GetSpeedImage(), this->GetFeatureImage()->GetRequestedRegion());

  if (m_EdgeWeight != 0.0)
    {
    diffusion->SetInput(this->GetFeatureImage());
    diffusion->SetConductanceParameter(m_SmoothingConductance);
    diffusion->SetTimeStep(m_SmoothingTimeStep);
    diffusion->SetNumberOfIterations(m_SmoothingIterations);

    laplacian->SetInput(diffusion->GetOutput());
    laplacian->Update();

    lit = ImageRegionIterator<FeatureImageType>(laplacian->GetOutput(),
                                          this->GetFeatureImage()->GetRequestedRegion());
    lit.GoToBegin();
    }

  // Copy the meta information (spacing and origin) from the feature image
  this->GetSpeedImage()->CopyInformation(this->GetFeatureImage());

  // Calculate the speed image
  ScalarValueType upper_threshold = static_cast<ScalarValueType>(m_UpperThreshold);
  ScalarValueType lower_threshold = static_cast<ScalarValueType>(m_LowerThreshold);
  ScalarValueType threshold=0;
  const float sqrt2 = 1.41421356;
  for ( fit.GoToBegin(), sit.GoToBegin(), mit.GoToBegin(); ! fit.IsAtEnd(); ++sit, ++fit, ++mit)
    {
    if (static_cast<ScalarValueType>(mit.Get()) == m_insideMaskValue)
      {
      sit.Set( static_cast<ScalarValueType>(m_multiplier));
      }
    else
      {
      if(fit.Get() < lower_threshold || fit.Get() > upper_threshold)
        {
        sit.Set(static_cast<ScalarValueType>(-1.0*m_multiplier));
        }
      else
        {
        //threshold ranges from -1.0 to 1.0
        threshold = (erfc(((double)fit.Get() - (m_mean - m_constant*m_variance) ) / (m_variance*sqrt2)) ) - 1.0;
        //sit.Set(m_multiplier*pow(threshold, m_alpha));
        sit.Set(m_multiplier*threshold);
        }
      }
/*    if ( m_EdgeWeight != 0.0)
      {
      sit.Set( static_cast<ScalarValueType>(threshold + m_EdgeWeight * lit.Get()) );
      ++lit;
      }
    else
      {
      sit.Set( static_cast<ScalarValueType>(threshold) );
      }*/
    }
}

template <class TImageType, class TFeatureImageType, class TMaskImageType>
double ErfcLevelSetFunction<TImageType, TFeatureImageType, TMaskImageType>
::erfc(double x)
{
    // Compute the complementary error function erfc(x).
    // Erfc(x) = (2/sqrt(pi)) Integral(exp(-t^2))dt between x and infinity
    //
    //--- Nve 14-nov-1998 UU-SAP Utrecht
    // The parameters of the Chebyshev fit
    const double a1 = -1.26551223,   a2 = 1.00002368,
    a3 =  0.37409196,   a4 = 0.09678418,
    a5 = -0.18628806,   a6 = 0.27886807,
    a7 = -1.13520398,   a8 = 1.48851587,
    a9 = -0.82215223,  a10 = 0.17087277;

    double v = 1.0; // The return value
    double z = std::fabs(x);

    if (z <= 0) return v; // erfc(0)=1

    double t = 1.0/(1.0+0.5*z);

    v = t*std::exp((-z*z) +a1+t*(a2+t*(a3+t*(a4+t*(a5+t*(a6+t*(a7+t*(a8+t*(a9+t*a10)))))))));

    if (x < 0) v = 2.0-v; // erfc(-x)=2-erfc(x)

    return v;
 }



} // end namespace itk


#endif
