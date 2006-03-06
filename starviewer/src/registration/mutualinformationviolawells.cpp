/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef __MUTUALINFORMATIONVIOLAWELLS_cpp
#define __MUTUALINFORMATIONVIOLAWELLS_cpp

#include "mutualinformationviolawells.h"

namespace udg
{
// Constructor
template <typename TFixedImage, typename TMovingImage>
MutualInformationViolaWells<TFixedImage,TMovingImage>
::MutualInformationViolaWells()
{
    m_transform     = TransformType::New();
    m_optimizer     = OptimizerType::New();
    m_interpolator  = InterpolatorType::New();
    m_registration  = RegistrationType::New();
    m_metric        = MetricType::New();
    
    m_registration->SetOptimizer(     m_optimizer     );
    m_registration->SetTransform(     m_transform     );
    m_registration->SetInterpolator(  m_interpolator  );
    m_registration->SetMetric( m_metric  );
    
    
    m_fixedNormalizer = FixedNormalizeFilterType::New();
    m_movingNormalizer = MovingNormalizeFilterType::New();
    
    // per inicialitzar la tranformació
    m_initializer = TransformInitializerType::New(); 
    
    m_fixedGaussianFilter  = GaussianFilterType::New();
    m_movingGaussianFilter = GaussianFilterType::New();
    
    m_finalTransform= TransformType::New();
    
    m_resample = ResampleFilterType::New();
}

// Metode el qual l'hi passem la imatge fixe i la mobil
template <typename TFixedImage, typename TMovingImage>
void MutualInformationViolaWells<TFixedImage,TMovingImage>
::SetInputImages(TFixedImage* fixedImage, TMovingImage* movingImage )
{
    m_fixedImage = fixedImage;
    m_movingImage = movingImage; 

 }

// Metode per configurar els paramatres necesaris per la metrica

template <typename TFixedImage, typename TMovingImage>
void MutualInformationViolaWells<TFixedImage,TMovingImage>::SetMetricParameters( double fixedStdDev, double movingStdDev, int numSamples )
{
    
    m_metric->SetFixedImageStandardDeviation(  fixedStdDev );
    m_metric->SetMovingImageStandardDeviation( movingStdDev );
    m_metric->SetNumberOfSpatialSamples( numSamples ); 
    
}


// Metode per configurar els paramatres necesaris del Gaussian
template <typename TFixedImage, typename TMovingImage>
void MutualInformationViolaWells<TFixedImage,TMovingImage>::SetGaussianFilterParameters(double fixedVariance, double movingVariance )
{
    
    m_fixedGaussianFilter->SetVariance( fixedVariance );
    m_movingGaussianFilter->SetVariance( movingVariance );
}

// Metode per configurar els paramatres de l'Optimitzador

template <typename TFixedImage, typename TMovingImage>
void MutualInformationViolaWells<TFixedImage,TMovingImage>::SetOptimizerParameters( double learningRate, int iterations )
{
    // Donem el nombre d'iteracions màximes
    m_optimizer->SetNumberOfIterations( iterations );
    
    m_optimizer->SetLearningRate( learningRate );
     
    // maximitzar el valor a optimitzar        
    m_optimizer->MaximizeOn();
    
    
}


// Metode que aplica el metode de registre i fa configuracions diverses.
template <typename TFixedImage, typename TMovingImage>
bool MutualInformationViolaWells<TFixedImage,TMovingImage>::applyMethod()
{

//-------- Normalització Imatges --------------------------------
  
 m_fixedNormalizer->SetInput( m_fixedImage );
 m_movingNormalizer->SetInput( m_movingImage );

 m_fixedGaussianFilter->SetInput( m_fixedNormalizer->GetOutput() );
 m_movingGaussianFilter->SetInput( m_movingNormalizer->GetOutput() );

 m_registration->SetFixedImage(    m_fixedGaussianFilter->GetOutput()    );
 m_registration->SetMovingImage(   m_movingGaussianFilter->GetOutput()   );
  
 m_fixedNormalizer->Update();
 m_registration->SetFixedImageRegion( m_fixedNormalizer->GetOutput()->GetBufferedRegion() );                                                                                               
 
 //---- Configuracio Transform -------------------------------
 
 // li apliquem l'inicialtzador al transfomador perquè trobi el centre de rotació adequat
 m_initializer->SetTransform(   m_transform );
 
 m_initializer->SetFixedImage(  m_fixedImage );
 m_initializer->SetMovingImage( m_movingImage );
 
 // fer servir el centre de masses ( no el geomètric )
 m_initializer->MomentsOn(); 
 m_initializer->InitializeTransform();

  
 m_registration->SetInitialTransformParameters( m_transform->GetParameters());
 
 std::cout << "Parameters Inicials de Transformació = " << std::endl;
 std::cout << m_transform->GetParameters() << std::endl;
 std::cout << m_transform->GetNumberOfParameters() << std::endl;
 
 
 
  // Monitorejem el procés de registre
  //
  CommandIterationUpdateMI::Pointer observer = CommandIterationUpdateMI::New();
  m_transform->AddObserver( itk::IterationEvent(), observer );

  // --------------- Execucio -----------------
  std::cout<<"Abans de registrar "<<std::endl;
  try
  {
      m_registration->StartRegistration();
  }
  catch (itk::ExceptionObject & e)
  {
      std::cout << e << std::endl;
      std::cout<<"Error, no ha anat bé el registre!!!"<<std::endl;
      return false;
  }
  std::cout<<"Després de registrar"<<std::endl;
 
 //-------------------- Resultats ------------------ 
 
    m_finalParameters = m_registration->GetLastTransformParameters();
 
 
    m_finalTransform->SetParameters( m_finalParameters );
    m_resample->SetTransform( m_finalTransform );
    m_resample->SetInput( m_movingImage );
    m_resample->SetSize( m_fixedImage->GetLargestPossibleRegion().GetSize() );
    m_resample->SetOutputOrigin(  m_fixedImage->GetOrigin() );
    m_resample->SetOutputSpacing( m_fixedImage->GetSpacing() );        
    
    m_resample->SetDefaultPixelValue( 400 );
    
    
    
  
 m_registeredImage = m_resample->GetOutput();
                  
    m_resample->Update();
  
 return true; 
}


template <typename TFixedImage, typename TMovingImage> 
TFixedImage*
MutualInformationViolaWells<TFixedImage,TMovingImage>::getRegisteredImage()
{
  return m_registeredImage;
}

/*
template <typename TFixedImage, typename TMovingImage> 
typename MutualInformationViolaWells<TFixedImage,TMovingImage>::OptimizerParametersType
MutualInformationViolaWells<TFixedImage,TMovingImage>::getParametresFinals()
{
  return m_finalParameters;
}
*/


}; // fi namespace 

#endif

