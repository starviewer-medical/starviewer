/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkErfcLevelSetFunction.h,v $
  Language:  C++
  Date:      $Date: 2006/04/18 19:04:23 $
  Version:   $Revision: 1.13 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkErfcLevelSetFunction_h_
#define __itkErfcLevelSetFunction_h_

#include "itkSegmentationLevelSetFunction.h"
#include "itkNumericTraits.h"
namespace itk {

/** \class ErfcLevelSetFunction
 *
 * \brief This function is used in ErfcLevelSetImageFilter to
 * segment structures in images based on intensity values.
 *
 * \par  SegmentationLevelSetFunction is a subclass of the generic LevelSetFunction.
 * It is useful for segmentations based on intensity values in an image.  It works
 * by constructing a speed term (feature image) with positive values inside an
 * intensity window (between a low and high threshold) and negative values
 * outside that intensity window.  The evolving level set front will lock onto
 * regions that are at the edges of the intensity window.
 *
 *  You may optionally add a Laplacian calculation on the image to the
 *  threshold-based speed term by setting the EdgeWeight parameter to a
 *  non-zero value.  The Laplacian term will cause the evolving surface to
 *  be more strongly attracted to image edges.   Several parameters control a
 *  preprocessing FeatureImage smoothing stage applied only to the Laplacian
 *  calculation.
 *
 *  \par
 *  Image \f$ f \f$ is thresholded pixel by pixel using upper threshold
 *  \f$ U \f$ and lower threshold \f$ L \f$ according to the following formula.
 *
 * \par
 *  \f$  f(x) = \left\{ \begin{array}{ll} g(x) - L & \mbox{if $(g)x < (U-L)/2 + L$} \\ U - g(x) & \mbox{otherwise} \end{array} \right. \f$
 *
 * \sa SegmentationLevelSetImageFunction
 *  \sa ErfcLevelSetImageFilter */
template <class TImageType, class TFeatureImageType = TImageType, class TMaskImageType = TImageType>
class ITK_EXPORT ErfcLevelSetFunction
  : public SegmentationLevelSetFunction<TImageType, TFeatureImageType>
{
public:
  /** Standard class typedefs. */
  typedef ErfcLevelSetFunction Self;
  typedef SegmentationLevelSetFunction<TImageType, TFeatureImageType> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self> ConstPointer;
  typedef TFeatureImageType FeatureImageType;
  typedef TMaskImageType MaskImageType;
  typedef typename MaskImageType::PixelType MaskPixelType;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods) */
  itkTypeMacro( ErfcLevelSetFunction, SegmentationLevelSetFunction );

  /** Extract some parameters from the superclass. */
  typedef typename Superclass::ImageType ImageType;
  typedef typename Superclass::ScalarValueType ScalarValueType;
  typedef typename Superclass::FeatureScalarType FeatureScalarType;
  typedef typename Superclass::RadiusType RadiusType;

  /** Extract some parameters from the superclass. */
  itkStaticConstMacro(ImageDimension, unsigned int,
                      Superclass::ImageDimension);

  /** Set/Get threshold values */
  void SetUpperThreshold(FeatureScalarType f)
  { m_UpperThreshold = f; }
  FeatureScalarType GetUpperThreshold() const
  { return m_UpperThreshold; }
  void SetLowerThreshold(FeatureScalarType f)
  { m_LowerThreshold = f; }
  FeatureScalarType GetLowerThreshold() const
  { return m_LowerThreshold; }

  virtual void CalculateSpeedImage();

  virtual void Initialize(const RadiusType &r)
  {
    Superclass::Initialize(r);

    this->SetAdvectionWeight( NumericTraits<ScalarValueType>::Zero);
    this->SetPropagationWeight(-1.0 * NumericTraits<ScalarValueType>::One);
    this->SetCurvatureWeight(NumericTraits<ScalarValueType>::One);
  }

  /** Set/Get the weight applied to the edge (Laplacian) attractor in the speed
   *  term function. Zero will turn this term off. */
  void SetEdgeWeight(const ScalarValueType p)
  {
    m_EdgeWeight = p;
  }
  ScalarValueType GetEdgeWeight() const
  {
    return m_EdgeWeight;
  }

  /** Anisotropic diffusion is applied to the FeatureImage before calculatign
   * the Laplacian (edge) term. This method sets/gets the smoothing
   * conductance. */
  void SetSmoothingConductance(const ScalarValueType p)
  {
    m_SmoothingConductance = p;
  }
  ScalarValueType GetSmoothingConductance() const
  {
    return m_SmoothingConductance;
  }

  /** Anisotropic diffusion is applied to the FeatureImage before calculating
   * the Laplacian (edge) term. This method sets/gets the number of diffusion
   * iterations. */
  void SetSmoothingIterations(const int p)
  {
    m_SmoothingIterations = p;
  }
  int GetSmoothingIterations() const
  {
    return m_SmoothingIterations;
  }

  /** Anisotropic diffusion is applied to the FeatureImage before calculating
   * the Laplacian (edge) term. This method sets/gets the diffusion time
   * step. */
  void SetSmoothingTimeStep(const ScalarValueType i)
  {
    m_SmoothingTimeStep = i;
  }
  ScalarValueType GetSmoothingTimeStep() const
  {
    return m_SmoothingTimeStep;
  }

  void SetMaskImage(const typename MaskImageType::Pointer im)
  {
    m_maskImage = im;
  }
  typename MaskImageType::Pointer GetMaskImage()
  {
    return m_maskImage;
  }

  void SetMean(const float i)  {    m_mean = i;  }
  float GetMean() const  {    return m_mean;  }

  void SetVariance(const float i)  {    m_variance = i;  }
  float GetVariance() const  {    return m_variance;  }

  void SetConstant(const float i)  {    m_constant = i;  }
  float GetConstant() const  {    return m_constant;  }

  void SetMultiplier(const float i)  {    m_multiplier = i;  }
  float GetMultiplier() const  {    return m_multiplier;  }

  void SetAlpha(const float i)  {    m_alpha = i;  }
  float GetAlpha() const  {    return m_alpha;  }

  void SetMaskInsideValue(const MaskPixelType i)  {  m_insideMaskValue = i;  }
  MaskPixelType GetMaskInsideValue() const  {    return m_insideMaskValue;  }

protected:
  ErfcLevelSetFunction()
  {
    m_UpperThreshold = NumericTraits<FeatureScalarType>::max();
    m_LowerThreshold = NumericTraits<FeatureScalarType>::NonpositiveMin();
    this->SetAdvectionWeight(0.0);
    this->SetPropagationWeight(1.0);
    this->SetCurvatureWeight(1.0);
    this->SetSmoothingIterations(5);
    this->SetSmoothingConductance(0.8);
    this->SetSmoothingTimeStep(0.1);
    this->SetEdgeWeight(0.0);
    this->SetMean(30.0);
    this->SetVariance(20.0);
    this->SetConstant(1.0);
    this->SetMultiplier(1.0);
    this->SetAlpha(1.0);
    this->SetMaskInsideValue(255);
  }
  virtual ~ErfcLevelSetFunction(){}

  ErfcLevelSetFunction(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  void PrintSelf(std::ostream& os, Indent indent) const
  {
    //Superclass::PrintSelf(os, indent );
    os << indent << "UpperThreshold: " << m_UpperThreshold << std::endl;
    os << indent << "LowerThreshold: " << m_LowerThreshold << std::endl;
    os << indent << "EdgeWeight: " << m_EdgeWeight << std::endl;
    os << indent << "SmoothingTimeStep: " << m_SmoothingTimeStep << std::endl;
    os << indent << "SmoothingIterations: " << m_SmoothingIterations << std::endl;
    os << indent << "SmoothingConductance: " << m_SmoothingConductance << std::endl;
    os << indent << "Mean: " << m_mean << std::endl;
    os << indent << "Variance: " << m_variance << std::endl;
    os << indent << "Constant: " << m_constant << std::endl;
    os << indent << "Multiplier: " << m_multiplier << std::endl;
    os << indent << "Alpha: " << m_alpha << std::endl;
    os << indent << "InsideMaskValue: " << m_insideMaskValue << std::endl;
  }

  double erfc(double x);

  FeatureScalarType m_UpperThreshold;
  FeatureScalarType m_LowerThreshold;
  ScalarValueType   m_EdgeWeight;
  ScalarValueType   m_SmoothingConductance;
  int               m_SmoothingIterations;
  ScalarValueType   m_SmoothingTimeStep;
  typename MaskImageType::Pointer m_maskImage;
  float m_mean;
  float m_constant;
  float m_variance;
  float m_multiplier;
  float m_alpha;
  MaskPixelType m_insideMaskValue;
};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkErfcLevelSetFunction.cpp"
#endif

#endif
