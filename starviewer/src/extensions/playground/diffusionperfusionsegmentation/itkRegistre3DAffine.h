/***************************************************************************
 *   Copyright (C) 2004 by Joaquim Rodríguez i Guerrero                    *
 *   u1033967@correu.udg.es                                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
#ifndef __ITKREGISTRE3DAFFINE_h
#define __ITKREGISTRE3DAFFINE_h


#include "itkImageRegistrationMethod.h"

#include "itkCenteredEuler3DTransform.h"
#include "itkCenteredAffineTransform.h"
#include "itkFixedCenterOfRotationAffineTransform.h"

#include "itkCenteredTransformInitializer.h"
#include "itkMutualInformationImageToImageMetric.h"
#include "itkMutualInformationHistogramImageToImageMetric.h"
#include "itkLinearInterpolateImageFunction.h"

#include "itkGradientDescentOptimizer.h"
#include "itkRegularStepGradientDescentOptimizer.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"

#include "itkImage.h"
#include "itkNormalizeImageFilter.h"
#include "itkDiscreteGaussianImageFilter.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCommand.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageBase.h"
#include <qstringlist.h>
#include "itkCommand.h"


class CommandIterationUpdate : public itk::Command 
{
public:
  typedef  CommandIterationUpdate   Self;
  typedef  itk::Command             Superclass;
  typedef itk::SmartPointer<Self>  Pointer;
  itkNewMacro( Self );
protected:
  CommandIterationUpdate() {};
public:
  //typedef itk::RegularStepGradientDescentOptimizer  OptimizerType;
  //typedef itk::GradientDescentOptimizer  OptimizerType;
  typedef itk::OnePlusOneEvolutionaryOptimizer   OptimizerType;
  typedef   const OptimizerType   *    OptimizerPointer;

  void Execute(itk::Object *caller, const itk::EventObject & event)
    {
      Execute( (const itk::Object *)caller, event);
    }

  void Execute(const itk::Object * object, const itk::EventObject & event)
    {
      OptimizerPointer optimizer = 
        dynamic_cast< OptimizerPointer >( object );
      if( typeid( event ) != typeid( itk::IterationEvent ) )
        {
        return;
        }
      std::cout << optimizer->GetCurrentIteration() << "   ";
      std::cout << optimizer->GetValue() << "   ";
      std::cout << optimizer->GetCurrentPosition() << std::endl;
    }
};


namespace udg{


template <typename TFixedImage, typename TMovingImage>
class itkRegistre3DAffine
{
 public:

  // ------------- ATRIBUTS ----------

  // Tipus Imatge

  
  // Es el mateix que hi ha ara hem l'ho de l'exemple.
  
  typedef TFixedImage  FixedImageType;
  typedef TMovingImage MovingImageType;

  static const   unsigned int Dimension = FixedImageType::ImageDimension;
  typedef typename FixedImageType::PixelType  PixelType;

  typedef   float     InternalPixelType;
  typedef itk::Image< InternalPixelType, Dimension > InternalImageType;

//  typedef itk::ImageFileReader< FixedImageType  >  FixedImageReaderType;
//  typedef itk::ImageFileReader< MovingImageType >  MovingImageReaderType;

  typedef itk::NormalizeImageFilter<FixedImageType, InternalImageType>     FixedNormalizeFilterType;
  typedef itk::NormalizeImageFilter<MovingImageType, InternalImageType >   MovingNormalizeFilterType;

 

  // Objectes del metode de registre

  typedef itk::CenteredEuler3DTransform < double >                                         TransformType;
  typedef typename TransformType::InputPointType                                       TransformPointType;

//  typedef itk::RegularStepGradientDescentOptimizer                                         OptimizerType;
//  typedef itk::GradientDescentOptimizer                                         OptimizerType;
  typedef itk::OnePlusOneEvolutionaryOptimizer                                             OptimizerType;
  
  typedef itk::LinearInterpolateImageFunction<InternalImageType, double>                   InterpolatorType;
  
  typedef itk::ImageRegistrationMethod<InternalImageType, InternalImageType >              RegistrationType;
  typedef itk::MutualInformationImageToImageMetric<InternalImageType, InternalImageType >  MetricType;
//  typedef itk::MutualInformationHistogramImageToImageMetric<InternalImageType, InternalImageType >  MetricType;
//  typedef typename MetricType::ScalesType             MetricScalesType;

  typedef itk::CenteredTransformInitializer< TransformType,FixedImageType,MovingImageType> TransformInitializerType;

  typedef itk::DiscreteGaussianImageFilter<InternalImageType, InternalImageType>  GaussianFilterType;
  
  //---------------
  
  typedef typename RegistrationType::ParametersType                              ParametersType;
  typedef itk::ResampleImageFilter<MovingImageType, FixedImageType >              ResampleFilterType;

  typedef  unsigned short  OutputPixelType;
  typedef itk::Image< OutputPixelType, Dimension > OutputImageType;
  typedef itk::CastImageFilter< FixedImageType, OutputImageType > CastFilterType;
  typedef itk::ImageFileWriter< OutputImageType >  WriterType;

  typedef OptimizerType::ScalesType                 OptimizerScalesType;
  typedef typename TMovingImage::SpacingType       spacing;
  
  typedef OptimizerType::ParametersType             OptimizerParametersType;
 
  
  typedef itk::ImageRegionIteratorWithIndex<FixedImageType>  FixedIteratorType;

 
 // Definim els "objectes" que farem servir

 typename TransformType::Pointer             transform;
              OptimizerType::Pointer          optimizer;
 typename InterpolatorType::Pointer          interpolator;
 typename RegistrationType::Pointer          registration;
 typename MetricType::Pointer                metric;
 
 //typename FixedImageReaderType::Pointer      fixedImageReader;
 //typename MovingImageReaderType::Pointer     movingImageReader;
 typename FixedNormalizeFilterType::Pointer  fixedNormalizer;
 typename MovingNormalizeFilterType::Pointer movingNormalizer;
 
 //typename TransformInitializerType::Pointer  initializer;

 typename GaussianFilterType::Pointer        fixedSmoother;
 typename GaussianFilterType::Pointer        movingSmoother; 

 
 
 
 typename TransformType::Pointer             finalTransform; 
 typename ResampleFilterType::Pointer        resample; //
 typename FixedImageType::Pointer            OutputImage;
 

 typename FixedImageType::Pointer            fixedInputImage;
 typename MovingImageType::Pointer           movingInputImage;
 
 typedef typename FixedImageType::SizeType  regionSizeType;
 
 
 
 

 double  TranslationAlongX;
 double  TranslationAlongY;
 double  TranslationAlongZ;
 int     numberOfIterations;
 double  bestValue;
 
 int nIterations;
 int MaximumStepLength;
 double MiniumStepLength;
 


 // ---------- METODES --------------

 itkRegistre3DAffine();
 void SetInputImages(TFixedImage* FixedImage, TMovingImage* MovingImage);
 void SetParamatersMetric(double FStantardDevitation, double MStantardDevitation, int NumberOfSpatialSample);
 void SetParamatersGaussian(int FVariance, int MVariance);
 void SetParamatresOptimizer(int Maximun, double Minium, int Iterations); 
 bool applyMethod();
 
 OptimizerParametersType getFinalParameters();
 
private:
  OptimizerParametersType m_finalParameters;
 };
 #include "itkRegistre3DAffine.cpp"

};
#endif
