/***************************************************************************
 *   Copyright (C) 2004 by Joaquim Rodr�uez i Guerrero                    *
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
#ifndef _ITKREGISTRE3DAFFINE_CPP_
#define _ITKREGISTRE3DAFFINE_CPP_

#include "itkRegistre3DAffine.h"

using namespace udg;
//namespace udg{


// Constructor
template <typename TFixedImage, typename TMovingImage>
itkRegistre3DAffine<TFixedImage,TMovingImage>::itkRegistre3DAffine()
{
  transform     = TransformType::New();
  optimizer     = OptimizerType::New();
  interpolator  = InterpolatorType::New();
  registration  = RegistrationType::New();
  metric        = MetricType::New();

  registration->SetOptimizer(     optimizer     );
  registration->SetTransform(     transform     );
  registration->SetInterpolator(  interpolator  );
  registration->SetMetric( metric  );

 // fixedImageReader  = FixedImageReaderType::New();
 // movingImageReader = MovingImageReaderType::New();

  fixedNormalizer = FixedNormalizeFilterType::New();
  movingNormalizer = MovingNormalizeFilterType::New();

 // initializer = TransformInitializerType::New();

  fixedSmoother  = GaussianFilterType::New();
  movingSmoother = GaussianFilterType::New();

  finalTransform= TransformType::New();
  resample=ResampleFilterType::New();
}

// Metode el qual l'hi passem la imatge fixe i la mobil
template <typename TFixedImage, typename TMovingImage>
void itkRegistre3DAffine<TFixedImage,TMovingImage>::SetInputImages(TFixedImage* FixedImage, TMovingImage* MovingImage)
{
 fixedInputImage=FixedImage;
 movingInputImage=MovingImage;


 //const TMovingImage::SpacingType& sp = MovingImage->GetSpacing();



}



// Metode per configurar els paramatres necesaris pel metirc

template <typename TFixedImage, typename TMovingImage>
void itkRegistre3DAffine<TFixedImage,TMovingImage>::SetParamatersMetric(double FStantardDevitation, double MStantardDevitation, int NumberOfSpatialSample)
{
 metric->SetFixedImageStandardDeviation(  FStantardDevitation );
 metric->SetMovingImageStandardDeviation( MStantardDevitation );
 metric->SetNumberOfSpatialSamples( NumberOfSpatialSample );


}


// Metode per configurar els paramatres necesaris del Gaussian
template <typename TFixedImage, typename TMovingImage>
void itkRegistre3DAffine<TFixedImage,TMovingImage>::SetParamatersGaussian(int FVariance, int MVariance)
{
 fixedSmoother->SetVariance( FVariance );
 movingSmoother->SetVariance( MVariance );
}

// Metode per configurar els paramatres del Optimizer
template <typename TFixedImage, typename TMovingImage>
void itkRegistre3DAffine<TFixedImage,TMovingImage>::SetParamatresOptimizer(int Maximun, double Minium, int Iterations)
{

MaximumStepLength=Maximun;
MiniumStepLength=Minium;
nIterations=Iterations;
/*//RegularStep
optimizer->SetNumberOfIterations(Iterations);
//optimizer->SetLearningRate( 0.1 );
optimizer->SetLearningRate( 1e-4 );

//optimizer->SetMaximumStepLength(Maximun);
//optimizer->SetMinimumStepLength(Minium);
*/

//One plus one
optimizer->SetMaximumIteration(Iterations);
typedef itk::Statistics::NormalVariateGenerator GeneratorType;

GeneratorType::Pointer generator = GeneratorType::New();
generator->Initialize(1111);
optimizer->SetNormalVariateGenerator( generator );
optimizer->Initialize(10, 1.0, -1);
optimizer->SetEpsilon(0.01);

optimizer->MaximizeOn();

}


// Metode que aplica el metode de registre i fa configuracions diverses.
template <typename TFixedImage, typename TMovingImage>
bool itkRegistre3DAffine<TFixedImage,TMovingImage>::applyMethod()
{

//-------- Normalitzaci? Imatges --------------------------------

 fixedNormalizer->SetInput( fixedInputImage);
 movingNormalizer->SetInput( movingInputImage);

 fixedSmoother->SetInput( fixedNormalizer->GetOutput() );
 movingSmoother->SetInput( movingNormalizer->GetOutput() );

 registration->SetFixedImage(    fixedSmoother->GetOutput()    );
 registration->SetMovingImage(   movingSmoother->GetOutput()   );

 fixedNormalizer->Update();
 registration->SetFixedImageRegion(fixedNormalizer->GetOutput()->GetBufferedRegion() );

 //---- Configuracio Transform -------------------------------

 // Per tal de poder calcular el centre de la massa i els vector.

 // Aqui tinc un diubte, ja que no ser si hem d'utilitzar la normalitzada o la normal. No m'accepta la normalitzada.

 // Aqui calculem el centre amb l'ajuda de la classe, ja que estem utiltizant el centeredAFfineTransform.

 //initializer->SetTransform(   transform );

 //initializer->SetFixedImage(  fixedInputImage );
 //initializer->SetMovingImage( movingInputImage );

 //initializer->MomentsOn();
 //initializer->InitializeTransform();


  const typename FixedImageType::SpacingType& fixedSpacing = fixedInputImage->GetSpacing();
  const typename FixedImageType::PointType& fixedOrigin = fixedInputImage->GetOrigin();
  //const typename FixedImageType::SpacingType& fixedspacing = fixedInputImage->GetSpacing();

  typename FixedImageType::SizeType fixedSize = fixedInputImage->GetLargestPossibleRegion().GetSize();

  TransformPointType centerFixed;

  centerFixed[0]= fixedOrigin[0] + fixedSpacing[0]*fixedSize[0]/2.0;
  centerFixed[1]= fixedOrigin[1] + fixedSpacing[1]*fixedSize[1]/2.0;
  centerFixed[2]= fixedOrigin[2] + fixedSpacing[2]*fixedSize[2]/2.0;

  const typename MovingImageType::SpacingType& movingSpacing = movingInputImage->GetSpacing();
  const typename MovingImageType::PointType& movingOrigin = movingInputImage->GetOrigin();
  //const typename FixedImageType::SpacingType& fixedspacing = fixedInputImage->GetSpacing();

  typename MovingImageType::SizeType movingSize = movingInputImage->GetLargestPossibleRegion().GetSize();

  TransformPointType centerMoving;

  centerMoving[0]= movingOrigin[0] + movingSpacing[0]*movingSize[0]/2.0;
  centerMoving[1]= movingOrigin[1] + movingSpacing[1]*movingSize[1]/2.0;
  centerMoving[2]= movingOrigin[2] + movingSpacing[2]*movingSize[2]/2.0;

 // transform->SetCenterOfRotationComponent(centerFixed);
 // transform->SetOffset(centerMoving - centerFixed);
  transform->SetIdentity();
  transform->SetCenter(centerFixed);
  transform->SetTranslation(centerMoving - centerFixed);

 // transform->SetAngle(0.0);


/* TransformPointType point;
 point.Fill(3);
 point[0]=64;
 point[1]=64;
 point[2]=10;
  transform->SetCenter(point);
 */
//  transform->SetCenterOfRotationComponent(point);
 //transform->SetIdentity();


 //------------------------------------------
  ParametersType InitialParameters( transform->GetNumberOfParameters()) ;
  InitialParameters.Fill(0.0001);
  //InitialParameters[0] = 0.0001 ;
  //InitialParameters[1] = 0.0001 ;
  //InitialParameters[2] = 0.0001 ;
  //InitialParameters[3] = 0.0001 ;
  //InitialParameters[4] = 0.0001 ;
  //InitialParameters[5] = 0.0001 ;

 //registration->SetInitialTransformParameters( InitialParameters );
 registration->SetInitialTransformParameters( transform->GetParameters());

 std::cout << "Intial Parameters = " << std::endl;
 std::cout << transform->GetParameters() << std::endl;
 std::cout << InitialParameters << std::endl;
 std::cout << transform->GetNumberOfParameters() << std::endl;

 // -------------- Configuraci? Optimizer ---------------

 OptimizerScalesType optimizerScales( transform->GetNumberOfParameters() );


//  double translationScale = 1.0 / 1000.0;

 // (N + 1) X N = 12 + N PQ CenteredAffineTransform

 // Aqui falten els altres

 /*
  //Affine Transformation
 optimizerScales[0] = 1.0;
 optimizerScales[1] = 0.1;
 optimizerScales[2] = 0.1;
 optimizerScales[3] = 0.1;
 optimizerScales[4] = 1.0;
 optimizerScales[5] = 0.1;
 optimizerScales[6] = 0.1;
 optimizerScales[7] = 0.1;
 optimizerScales[8] = 1.0;



 optimizerScales[9] = translationScale;
 optimizerScales[10] = translationScale;
 optimizerScales[11] = translationScale;

 optimizerScales[12] = translationScale;
 optimizerScales[13] = translationScale;
 optimizerScales[14] = translationScale;
*/

/*
 //RegularStep
 //EulerTransform
 optimizerScales[0] = 1;
 optimizerScales[1] = 1;
 optimizerScales[2] = 1;
 optimizerScales[3] = 0.001;
 optimizerScales[4] = 0.001;
 optimizerScales[5] = 0.001;
  */
 //OnePlusOne
 optimizerScales[0] = 10000;
 optimizerScales[1] = 10000;
 optimizerScales[2] = 10000;
 optimizerScales[3] = 10;
 optimizerScales[4] = 10;
 optimizerScales[5] = 10;
 optimizerScales[6] = 10;
 optimizerScales[7] = 10;
 optimizerScales[8] = 10;

 std::cout<<" optimizerScales ="<< optimizerScales<<std::endl;
 optimizer->SetScales( optimizerScales );


// MetricScalesType derivativeScales( transform->GetNumberOfParameters() );

  /*
 // (N + 1) X N = 12 + N PQ CenteredAffineTransform

 // Aqui falten els altres

 derivativeScales[0] = 1.0;
 derivativeScales[1] = 0.1;
 derivativeScales[2] = 0.1;
 derivativeScales[3] = 0.1;
 derivativeScales[4] = 1.0;
 derivativeScales[5] = 0.1;
 derivativeScales[6] = 0.1;
 derivativeScales[7] = 0.1;
 derivativeScales[8] = 1.0;

 derivativeScales[9] = 1;
 derivativeScales[10] = 1;
 derivativeScales[11] = 1;
// derivativeScales[9] = translationScale;
// derivativeScales[10] = translationScale;
// derivativeScales[11] = translationScale;
 //derivativeScales[12] = 0.1;
 //derivativeScales[13] = 0.1;
 //derivativeScales[14] = 0.1;
*/
/*
 derivativeScales.Fill(1);
 derivativeScales[0] = 1;
 derivativeScales[1] = 1;
 derivativeScales[2] = 1;
 derivativeScales[3] = 0.05;
 derivativeScales[4] = 0.05;
 derivativeScales[5] = 0.05;


 std::cout<<" derivativeScales ="<< derivativeScales<<std::endl;
 std::cout<<" derivativeStep ="<< metric->GetDerivativeStepLength()<<std::endl;
 metric->SetDerivativeStepLengthScales( derivativeScales );
*/

  // Create the Command observer and register it with the optimizer.
  //
  CommandIterationUpdate::Pointer observer = CommandIterationUpdate::New();
  optimizer->AddObserver( itk::IterationEvent(), observer );

  // --------------- Execuci? -----------------
  std::cout<<"Abans registre"<<std::endl;
  try
  {
      registration->StartRegistration();
  }
  catch (itk::ExceptionObject & e)
  {
      std::cout << e << std::endl;
      std::cout<<"Error, no ha anat b�el registre!!!"<<std::endl;
      return false;
  }
  std::cout<<"Despres registre"<<std::endl;

 //-------------------- Resultats ------------------

 OptimizerParametersType finalParameters = registration->GetLastTransformParameters();

/*  const double param1               = finalParameters[0];
  const double param2              = finalParameters[1];
  const double param3              = finalParameters[2];
  const double param4              = finalParameters[3];
  const double param5    = finalParameters[4];
  const double param6    = finalParameters[5];
  const double param7              = finalParameters[6];
  const double param8             = finalParameters[7];
  const double param9     = finalParameters[8];
  const double param10    = finalParameters[9];
  const double param11    = finalParameters[10];
  const double param12    = finalParameters[11];
  const double param13    = finalParameters[12];
  const double param14     = finalParameters[13];
  const double param15     = finalParameters[14];
*/

  std::cout << " paramatres : " << finalParameters  << std::endl;

  std::cout << "-------- Registre Fet -77-------- " << std::endl;
  for(unsigned int i=0;i<transform->GetNumberOfParameters();i++)
  {
    std::cout << " paramatre "<<i<<": " << finalParameters[i]  << std::endl;
  }
  m_finalParameters = finalParameters;

  std::cout<<"TRANSFORMACI� "<<transform<<std::endl;

 return true;
}



template <typename TFixedImage, typename TMovingImage>
typename itkRegistre3DAffine<TFixedImage,TMovingImage>::OptimizerParametersType
itkRegistre3DAffine<TFixedImage,TMovingImage>::getFinalParameters()
{
  return m_finalParameters;
}

//};

#endif

