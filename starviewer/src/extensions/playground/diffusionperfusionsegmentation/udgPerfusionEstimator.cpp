#ifndef __UDGPERFUSIONESTIMATOR_CPP_
#define __UDGPERFUSIONESTIMATOR_CPP_

#include "udgPerfusionEstimator.h"


using namespace udg;
//namespace udg {

template <typename TPerfuImage, typename TMaskImage, typename TTransform>
udgPerfusionEstimator<TPerfuImage,TMaskImage, TTransform>::udgPerfusionEstimator()
{
  m_perfuImage = 0;
  m_perfuMask = 0;
  m_ventricleMask = 0;
  m_strokeMask = 0;
  m_Transform = 0;
  m_estimatedImage = 0;
  m_InterpolatorVentricle = InterpolatorType::New();
  m_InterpolatorStroke = InterpolatorType::New();
  m_insideValue = static_cast<MaskPixelType> (255);
}

template <typename TPerfuImage, typename TMaskImage, typename TTransform>
udgPerfusionEstimator<TPerfuImage,TMaskImage, TTransform>::~udgPerfusionEstimator()
{
}

template <typename TPerfuImage, typename TMaskImage, typename TTransform>
void udgPerfusionEstimator<TPerfuImage,TMaskImage, TTransform>::ComputeEstimation()
{

  if(m_perfuImage.IsNull()){
    std::cout<<"Not Perfusion Image defined"<<std::endl;
    return;
    }


  if(m_ventricleMask.IsNull()){
    std::cout<<"Not Ventricle Mask Image defined"<<std::endl;
    return;
    }

  if(m_strokeMask.IsNull()){
    std::cout<<"Not Stroke Mask Image defined"<<std::endl;
    return;
    }

  if(m_Transform.IsNull()){
    std::cout<<"Not Transform defined"<<std::endl;
    return;
    }

  m_estimatedImage = PerfuImageType::New();
  m_estimatedImage->SetRegions( m_perfuImage->GetLargestPossibleRegion() );
  m_estimatedImage->SetSpacing( m_perfuImage->GetSpacing() );
  m_estimatedImage->SetOrigin( m_perfuImage->GetOrigin() );
  m_estimatedImage->Allocate();

  //Definim la regi�molt probablement infartada
  typename MaskImageType::Pointer m_strokeInfluence = MaskImageType::New();
  m_strokeInfluence->SetRegions( m_strokeMask->GetLargestPossibleRegion() );
  m_strokeInfluence->SetSpacing( m_strokeMask->GetSpacing() );
  m_strokeInfluence->SetOrigin( m_strokeMask->GetOrigin() );
  m_strokeInfluence->Allocate();

  typename DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
  binaryDilate->SetDilateValue( 255 );	//suposem que el valor alt ser�255
  StructuringElementType structuringElementDilate;
  structuringElementDilate.SetRadius( 2 ); // 3x3 structuring element
  structuringElementDilate.CreateStructuringElement();

  binaryDilate->SetKernel( structuringElementDilate );

  binaryDilate->SetInput( m_strokeMask );
  binaryDilate->Update();
  m_strokeInfluence = binaryDilate->GetOutput();

  //Fi regi�molt probablement infartada




      typename ResampleFilterType::Pointer resample = ResampleFilterType::New();

      typename TransformType::Pointer inverse = TransformType::New();
      if (!m_Transform->GetInverse( inverse ))
      {
          std::cout<<"ERROR! udgPerfusionEstimator<TPerfuImage,TMaskImage, TTransform>::ComputeEstimation() No hi ha inversa!"<<std::endl;
      }

      resample->SetTransform( inverse );
      resample->SetInput( m_ventricleMask);
      resample->SetSize( m_perfuImage->GetLargestPossibleRegion().GetSize() );
      resample->SetOutputOrigin(  m_perfuImage->GetOrigin() );
      resample->SetOutputSpacing( m_perfuImage->GetSpacing() );
      resample->SetDefaultPixelValue( 0 );
      resample->SetInterpolator( m_InterpolatorVentricle );

      resample->Update();

      Ventricles = resample->GetOutput();


      typename ResampleFilterType::Pointer resample2 = ResampleFilterType::New();

      resample2->SetTransform( inverse );
      resample2->SetInput( m_strokeInfluence);
      resample2->SetSize( m_perfuImage->GetLargestPossibleRegion().GetSize() );
      resample2->SetOutputOrigin(  m_perfuImage->GetOrigin() );
      resample2->SetOutputSpacing( m_perfuImage->GetSpacing() );
      resample2->SetDefaultPixelValue( 0 );
      resample2->SetInterpolator( m_InterpolatorStroke );

      resample2->Update();

      //Creem les finestres mostrant les imatges registrades

      Stroke = resample2->GetOutput();



  PerfuIteratorType estimatedIt(m_estimatedImage, m_estimatedImage->GetBufferedRegion());

  PerfuIteratorType perfuIt(m_perfuImage, m_perfuImage->GetBufferedRegion());
  PerfuIteratorType VentIt(Ventricles, Ventricles->GetBufferedRegion());
  PerfuIteratorType StkIt(Stroke, Stroke->GetBufferedRegion());

  RadiusNeighborType radius;
  radius.Fill(2);

  m_InterpolatorVentricle->SetInputImage( m_ventricleMask );
  m_InterpolatorStroke->SetInputImage( m_strokeInfluence );

  PerfuPointType inputPoint;
  PerfuPixelType perfuValue;
  PerfuPixelType VentValue;
  PerfuPixelType StkValue;
  typename PerfuImageType::IndexType index;

  MaskPointType transformedPoint;

  perfuIt.GoToBegin();
  VentIt.GoToBegin();
  StkIt.GoToBegin();
  estimatedIt.GoToBegin();
  ++perfuIt;
  ++VentIt;
  ++StkIt;
  ++estimatedIt;
    while (!perfuIt.IsAtEnd())
    {
        perfuValue = perfuIt.Value();

//         if(perfuValue == 0) // ((maskValue == m_insideValue)&&(perfuValue == 0))
        if ( perfuValue < 32 ) // ((maskValue == m_insideValue)&&(perfuValue == 0))
        {   //és a dir, és un punt negre
            index = perfuIt.GetIndex();
            m_perfuImage->TransformIndexToPhysicalPoint(index, inputPoint);

            transformedPoint = m_Transform->TransformPoint(inputPoint); //transformed point és el punt corresponent en difusió
            if (m_InterpolatorVentricle->IsInsideBuffer(transformedPoint))
            {
//                m_Interpolator->SetInputImage( m_ventricleMask );
                //const RealType VentriclemaskValue = m_InterpolatorVentricle->Evaluate(transformedPoint);
                VentValue = VentIt.Value();

//                if(VentriclemaskValue!=0)
                if(VentValue!=0)
                {
                    estimatedIt.Set(0);//estimatedIt.Set(1);
                }
                else
                {
                    /*
                    const RealType strokemaskValue = m_InterpolatorStroke->Evaluate(transformedPoint);
                    if(strokemaskValue!=0){
                        estimatedIt.Set(1);//estimatedIt.Set(255);
                    }
                    */
                    StkValue = StkIt.Value();
                    if(StkValue != 0)
                    {
                        estimatedIt.Set(255);
                    }
                    else
                    {
                        estimatedIt.Set(0);
                    }
                }
            }
        }
        else
        {
            estimatedIt.Set( perfuValue );  //estimatedIt.Set( perfuValue );
        }
        ++perfuIt;
        ++VentIt;
        ++StkIt;
        ++estimatedIt;
    }

    //Aqu�determinem els punts que no s� ni infart ni ventricle, fent una mitjana dels valors veins
    //La imatge s'actualitza sobre ella mateixa, per�no sembla que aix�hagi de portar problemes
 /*
  double med, cont;
  PerfuNeighborIteratorType perfuNeighborIt(radius, m_estimatedImage,  m_estimatedImage->GetBufferedRegion());

  perfuNeighborIt.GoToBegin();
  perfuIt.GoToBegin();
  estimatedIt.GoToBegin();
  ++perfuNeighborIt;
  ++estimatedIt;
  while (!estimatedIt.IsAtEnd())
  {
	  perfuValue = estimatedIt.Get();

    if (perfuValue == 0)    //� a dir, � un punt negre
	  {
      med=0;
		  cont=0;

		  for (unsigned int i = 0; i < perfuNeighborIt.Size(); i++)
		  {
		    if ( perfuNeighborIt.GetPixel(i) != 0 && perfuNeighborIt.GetPixel(i)<1000 )
		    {
		      med += perfuNeighborIt.GetPixel(i);
		      cont ++;
			  }
		  }
		  estimatedIt.Set(static_cast<PerfuPixelType> (med/cont));
	  }
	  ++perfuNeighborIt;
    ++estimatedIt;
  }
  */
  //Suavitzem la sortida --> S'hauria de fer per�d�a errors
/*
  typename SmoothingFilterType::Pointer smoothFilter = SmoothingFilterType::New();
//  smoothFilter->SetInput(m_estimatedImage);
  smoothFilter->SetInput(m_perfuImage);


//  smoothFilter->SetNumberOfIterations(5);
//  smoothFilter->SetTimeStep(0.0625);
//  smoothFilter->SetConductanceParameter(1);

//std::cout<<"hola 1"<<std::endl;

  smoothFilter->SetVariance(1);
  smoothFilter->SetMaximumKernelWidth(6);


  //smoothFilter->Update();

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
//std::cout<<"hola 2"<<std::endl;

  rescaler->SetInput(smoothFilter->GetOutput());
  rescaler->Update();
//std::cout<<"hola 3"<<std::endl;

  m_estimatedImage = rescaler->GetOutput();
  */
}

//};

#endif
