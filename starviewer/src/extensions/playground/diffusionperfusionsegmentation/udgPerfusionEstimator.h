#ifndef __UDGPERFUSIONESTIMATOR_H_
#define __UDGPERFUSIONESTIMATOR_H_


#include "itkImage.h"
#include "itkTransform.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkNearestNeighborInterpolateImageFunction.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkArray.h"
#include "itkNumericTraits.h"
#include "itkImageRegionConstIteratorWithIndex.h"
#include "itkImageRegionConstIterator.h"
#include "itkImageRegionIterator.h"
#include "itkConstNeighborhoodIterator.h"
#include "itkNeighborhoodIterator.h"
#include "itkTranslationTransform.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkGradientAnisotropicDiffusionImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkResampleImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"



namespace udg {

template <typename TPerfuImage, typename TMaskImage, typename TTransform>
class udgPerfusionEstimator{
public:
     typedef TPerfuImage    PerfuImageType;
     typedef TMaskImage     MaskImageType;

    static const unsigned int ImageDimension = TPerfuImage::ImageDimension;

     typedef  typename PerfuImageType::PixelType   PerfuPixelType;
     typedef  typename MaskImageType::PixelType  MaskPixelType;
     typedef  typename itk::NumericTraits< PerfuPixelType>::RealType RealType;

     typedef  typename PerfuImageType::PointType  PerfuPointType;
     typedef  typename MaskImageType::PointType   MaskPointType;

     typedef typename itk::ImageRegionIteratorWithIndex<PerfuImageType> PerfuIteratorType;
     typedef typename itk::ImageRegionIteratorWithIndex<MaskImageType> MaskIteratorType;

     typedef typename itk::NeighborhoodIterator<PerfuImageType> PerfuNeighborIteratorType;
     typedef typename itk::ConstNeighborhoodIterator<MaskImageType> MaskNeighborIteratorType;
     typedef typename PerfuNeighborIteratorType::RadiusType RadiusNeighborType;

     typedef TTransform TransformType;
//     typedef TransformType::ParametersType 	TransformParametersType;

     typedef itk::NearestNeighborInterpolateImageFunction<MaskImageType, double>	InterpolatorType;
//     typedef itk::LinearInterpolateImageFunction<MaskImageType, double>	InterpolatorType;
     typedef typename InterpolatorType::RealType InterpolatorRealType;

     typedef itk::BinaryBallStructuringElement<MaskPixelType,ImageDimension >			StructuringElementType;
     typedef itk::BinaryDilateImageFilter<MaskImageType,MaskImageType,StructuringElementType >	DilateFilterType;
//     typedef itk::GradientAnisotropicDiffusionImageFilter<PerfuImageType, PerfuImageType>	SmoothingFilterType;
//     typedef itk::CurvatureFlowImageFilter<PerfuImageType, PerfuImageType>	SmoothingFilterType;
     typedef itk::DiscreteGaussianImageFilter<PerfuImageType, PerfuImageType>			SmoothingFilterType;

     typedef itk::ResampleImageFilter<MaskImageType, PerfuImageType > ResampleFilterType;
     typedef itk::RescaleIntensityImageFilter<PerfuImageType, PerfuImageType > RescaleFilterType;


/*     typedef itk::BinaryThresholdImageFilter<InputImageType, OutputImageType >				ThresholdFilterType;
     typedef itk::BinaryBallStructuringElement<InputPixelType,Dimension >				StructuringElementType;
     typedef itk::BinaryErodeImageFilter< InputImageType, OutputImageType,StructuringElementType >	ErodeFilterType;
     typedef itk::BinaryDilateImageFilter<InputImageType,OutputImageType,StructuringElementType >	DilateFilterType;
  */

    udgPerfusionEstimator();

    ~udgPerfusionEstimator();

    void SetInsideValue(MaskPixelType in){m_insideValue = in;}
    MaskPixelType GetInsideValue () {return m_insideValue;}

    void SetPerfuImage(PerfuImageType *imPerfu) {m_perfuImage = imPerfu;}

    void SetPerfuMask(MaskImageType *maskPerfu) {m_perfuMask = maskPerfu;}
    void SetVentricleMask(MaskImageType *maskVentricle) {m_ventricleMask = maskVentricle;}
    void SetStrokeMask(MaskImageType *maskStroke) {m_strokeMask = maskStroke;}

    void SetTransform(TransformType *trans) {m_Transform = trans;}
    typename PerfuImageType::Pointer GetEstimatedImage()  {  return m_estimatedImage;  }

//    void SetInterpolator(InterpolatorType::Pointer interp) {m_Interpolator = interp;};

    void ComputeEstimation();

    typename PerfuImageType::Pointer getVentricles() { return Ventricles; }
    typename PerfuImageType::Pointer getStroke() { return Stroke; }

private:
  typename PerfuImageType::Pointer m_perfuImage;
  typename MaskImageType::Pointer m_perfuMask;
  typename MaskImageType::Pointer  m_ventricleMask;
  typename MaskImageType::Pointer  m_strokeMask;
  typename TransformType::Pointer m_Transform;
  typename InterpolatorType::Pointer m_InterpolatorVentricle;
  typename InterpolatorType::Pointer m_InterpolatorStroke;
  typename PerfuImageType::Pointer m_estimatedImage;
  MaskPixelType m_insideValue;

  typename PerfuImageType::Pointer Ventricles;
  typename PerfuImageType::Pointer Stroke;


};

//#ifndef ITK_MANUAL_INSTANTIATION
#include "udgPerfusionEstimator.cpp"
//#endif

//template class udgPerfusionEstimator<itk::Image<unsigned char,2>,itk::Image<unsigned char,2>,itk::TranslationTransform<double,2>>;

};


#endif
