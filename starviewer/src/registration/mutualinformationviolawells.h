/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef __MUTUALINFORMATIONVIOLAWELLS_h
#define __MUTUALINFORMATIONVIOLAWELLS_h

// Encapsula el mètode de registre
#include <itkImageRegistrationMethod.h>

// Transformador
#include <itkCenteredTransformInitializer.h>
#include <itkCenteredAffineTransform.h>

// Mètrica
#include <itkMutualInformationImageToImageMetric.h>

// Interpolador
#include <itkLinearInterpolateImageFunction.h>

// Optimitzador
#include <itkGradientDescentOptimizer.h>


#include <itkImage.h>
#include <itkNormalizeImageFilter.h>
#include <itkDiscreteGaussianImageFilter.h>
#include <itkResampleImageFilter.h>
#include <itkCommand.h>


namespace udg
{

class CommandIterationUpdateMI : public itk::Command 
{
public:
  typedef  CommandIterationUpdateMI   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdateMI() {};
public:
  
  typedef itk::GradientDescentOptimizer  OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer m_optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( typeid( event ) != typeid( itk::IterationEvent ) )
        {
        return;
        }
      std::cout << " " <<m_optimizer->GetCurrentIteration() << " ";
      std::cout << m_optimizer->GetValue() << "  ";
      std::cout << m_optimizer->GetCurrentPosition() << std::endl;
    }
};


template <typename TFixedImage, typename TMovingImage>
class MutualInformationViolaWells
{
 
 public:

  // ------------- ATRIBUTS ----------

  // Tipus de les imatges

    
  typedef TFixedImage  FixedImageType;
  typedef TMovingImage MovingImageType;

  static const unsigned int Dimension = FixedImageType::ImageDimension;
  typedef typename FixedImageType::PixelType  PixelType;

  typedef   float     InternalPixelType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

  
  // Filtres per normalitzar les imatges fixa i mòbil
  typedef itk::NormalizeImageFilter<FixedImageType, InternalImageType>     FixedNormalizeFilterType;
  typedef itk::NormalizeImageFilter<MovingImageType, InternalImageType >   MovingNormalizeFilterType;

  // Objectes del mètode de registre

  // Transformador
  typedef itk::CenteredAffineTransform < double >      TransformType;
  
  typedef itk::CenteredTransformInitializer< TransformType,FixedImageType,MovingImageType>
   TransformInitializerType;
  
  typedef typename TransformType::InputPointType        TransformPointType;

  // Optimitzador
  typedef itk::GradientDescentOptimizer                 OptimizerType;
    
  // Interpolador
  typedef itk::LinearInterpolateImageFunction<InternalImageType, double>       InterpolatorType;
  
  // Mètrica
  typedef itk::MutualInformationImageToImageMetric<InternalImageType, InternalImageType >  MetricType;

  // Classe que engloba el mètode de registre
  typedef itk::ImageRegistrationMethod<InternalImageType, InternalImageType >  RegistrationType;
  

  // filtre gaussia per a suavitzar les imatges
  typedef itk::DiscreteGaussianImageFilter<InternalImageType, InternalImageType>  GaussianFilterType;
  
  //---------------
  
  typedef typename RegistrationType::ParametersType                        ParametersType;
  typedef itk::ResampleImageFilter<MovingImageType, FixedImageType >       ResampleFilterType;

  typedef  unsigned short  OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;


// tipus dels parametres ( pel parametres finals )
  typedef OptimizerType::ParametersType             OptimizerParametersType;

  typedef itk::ImageRegionIteratorWithIndex<FixedImageType>  FixedIteratorType;

 
 // Definim els "objectes" que farem servir

 typename TransformType::Pointer             m_transform;
              OptimizerType::Pointer          m_optimizer;
 typename InterpolatorType::Pointer          m_interpolator;
 typename RegistrationType::Pointer          m_registration;
 typename MetricType::Pointer                m_metric;
 
 typename TransformInitializerType::Pointer    m_initializer;
 
 
 
 typename FixedNormalizeFilterType::Pointer  m_fixedNormalizer; 
 typename MovingNormalizeFilterType::Pointer m_movingNormalizer;
 
 typename GaussianFilterType::Pointer        m_fixedGaussianFilter;
 typename GaussianFilterType::Pointer        m_movingGaussianFilter; 

 
 typename TransformType::Pointer             m_finalTransform; 
 typename ResampleFilterType::Pointer        m_resample; 
 
//  typedef typename FixedImageType::Pointer OutputImageType;
 typename FixedImageType::Pointer            m_registeredImage;
 

 typename FixedImageType::Pointer            m_fixedImage;
 typename MovingImageType::Pointer           m_movingImage;
 
 
 typedef typename FixedImageType::SizeType  regionSizeType;
 
 
 // ---------- METODES --------------

 MutualInformationViolaWells();
 void SetInputImages(TFixedImage* fixedImage, TMovingImage* movingImage );
 void SetMetricParameters(double fixedStdDev, double movingStdDev, int numSamples );
 void SetGaussianFilterParameters(double fixedVariance, double movingVariance );
 void SetOptimizerParameters( double learningRate, int iterations ); 
 bool applyMethod();
 

 TFixedImage* getRegisteredImage();
 
private:
  OptimizerParametersType m_finalParameters;
 };
 
 }; //end namespace 
 
 #include "mutualinformationviolawells.cpp"

 
 
#endif

