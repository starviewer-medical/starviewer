#ifndef __UDGBINARYMAKER_CPP_
#define __UDGBINARYMAKER_CPP_


#include "udgBinaryMaker.h"


using namespace udg;
//namespace udg {

template <typename TInputImage, typename TMaskImage>
udgBinaryMaker<TInputImage,TMaskImage>::udgBinaryMaker()
{
  m_outsideValue = 0;
  m_insideValue = 255;

  m_minImageValue = 0;
  m_maxImageValue = 255;
}

template <typename TInputImage, typename TMaskImage>
udgBinaryMaker<TInputImage,TMaskImage>::~udgBinaryMaker()
{
}

template <typename TInputImage, typename TMaskImage>
void udgBinaryMaker<TInputImage,TMaskImage>::MascaraDifusio(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage)
{

  typename ThresholdFilterType::Pointer thresholdfilter = ThresholdFilterType::New();
  typename ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
  typename DilateFilterType::Pointer binaryDilate = DilateFilterType::New();


  thresholdfilter->SetInput( inImage );

  (this)->GetMaxImage(inImage, &m_maxImageValue);


  thresholdfilter->SetOutsideValue( m_outsideValue );
  thresholdfilter->SetInsideValue( m_insideValue  );

  const InputPixelType lowerThreshold = 20;
  const InputPixelType upperThreshold = m_maxImageValue;

  thresholdfilter->SetLowerThreshold( lowerThreshold );
  thresholdfilter->SetUpperThreshold( upperThreshold );
  thresholdfilter->Update();


  StructuringElementType structuringElement;
  structuringElement.SetRadius( 7 ); // 3x3 structuring element
  structuringElement.CreateStructuringElement();

  binaryErode->SetKernel( structuringElement );
  binaryDilate->SetKernel( structuringElement );

  binaryErode->SetErodeValue( m_insideValue );
  binaryDilate->SetDilateValue( m_insideValue );

  binaryDilate->SetInput( thresholdfilter->GetOutput() );
  binaryErode->SetInput( binaryDilate->GetOutput() );
  binaryErode->Update();

  maskImage = binaryErode->GetOutput();

}


template <typename TInputImage, typename TMaskImage>
void udgBinaryMaker<TInputImage,TMaskImage>::MascaraPerfusio(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage)
{

  typename ThresholdFilterType::Pointer thresholdfilter = ThresholdFilterType::New();
  typename ErodeFilterType::Pointer binaryErode1 = ErodeFilterType::New();
  typename ErodeFilterType::Pointer binaryErode2 = ErodeFilterType::New();
  typename DilateFilterType::Pointer binaryDilate = DilateFilterType::New();


  thresholdfilter->SetInput( inImage );

  thresholdfilter->SetOutsideValue( m_outsideValue );
  thresholdfilter->SetInsideValue( m_insideValue  );

  const InputPixelType lowerThreshold = 1;
  const InputPixelType upperThreshold = m_maxImageValue;

  thresholdfilter->SetLowerThreshold( lowerThreshold );
  thresholdfilter->SetUpperThreshold( upperThreshold );
  //thresholdfilter->Update();

  binaryErode1->SetErodeValue( m_insideValue );
  binaryDilate->SetDilateValue( m_insideValue );
  binaryErode2->SetErodeValue( m_insideValue );

  StructuringElementType structuringElementErode1;
  structuringElementErode1.SetRadius( 2 ); // 3x3 structuring element
  structuringElementErode1.CreateStructuringElement();

  binaryErode1->SetKernel( structuringElementErode1 );

  binaryErode1->SetInput( thresholdfilter->GetOutput() );

  StructuringElementType structuringElementDilate;
  structuringElementDilate.SetRadius( 5 ); // 3x3 structuring element
  structuringElementDilate.CreateStructuringElement();

  binaryDilate->SetKernel( structuringElementDilate );

  binaryDilate->SetInput( binaryErode1->GetOutput() );


  StructuringElementType structuringElementErode2;
  structuringElementErode2.SetRadius( 3 ); // 3x3 structuring element
  structuringElementErode2.CreateStructuringElement();

  binaryErode2->SetKernel( structuringElementErode2 );

  binaryErode2->SetInput( binaryDilate->GetOutput() );
  binaryErode2->Update();

  maskImage = binaryErode2->GetOutput();


}



template <typename TInputImage, typename TMaskImage>
void udgBinaryMaker<TInputImage,TMaskImage>::VentricleSegmentation(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage, unsigned int level)
{

  typename ThresholdFilterType::Pointer thresholdfilter = ThresholdFilterType::New();
  typename ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
  typename DilateFilterType::Pointer binaryDilate = DilateFilterType::New();

/*  //Allocating memory for the output image
  typename TInputImage::RegionType region;
  region=inImage->GetLargestPossibleRegion();
  maskImage->SetRegions( region );
  maskImage->Allocate();
*/

  thresholdfilter->SetInput( inImage );

  thresholdfilter->SetOutsideValue( m_outsideValue );
  thresholdfilter->SetInsideValue( m_insideValue  );

  const InputPixelType lowerThreshold = m_minImageValue;
  const InputPixelType upperThreshold = level;

  thresholdfilter->SetLowerThreshold( lowerThreshold );
  thresholdfilter->SetUpperThreshold( upperThreshold );

/*  binaryErode->SetErodeValue( m_insideValue );
  binaryDilate->SetDilateValue( m_insideValue );

  StructuringElementType structuringElement;
  structuringElement.SetRadius( 2 ); // 3x3 structuring element
  structuringElement.CreateStructuringElement();

  binaryErode->SetKernel( structuringElement );
  binaryErode->SetInput( thresholdfilter->GetOutput() );

  binaryDilate->SetKernel( structuringElement );
  binaryDilate->SetInput( binaryErode->GetOutput() );
  binaryDilate->Update();

  maskImage = binaryDilate->GetOutput();
*/
  thresholdfilter->Update();
  maskImage = thresholdfilter->GetOutput();

}


template <typename TInputImage, typename TMaskImage>
void udgBinaryMaker<TInputImage,TMaskImage>::StrokeSegmentation(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage, unsigned int level, unsigned int max)
{

  typename ThresholdFilterType::Pointer thresholdfilter = ThresholdFilterType::New();
  typename ErodeFilterType::Pointer binaryErode1 = ErodeFilterType::New();
  typename ErodeFilterType::Pointer binaryErode2 = ErodeFilterType::New();
  typename DilateFilterType::Pointer binaryDilate = DilateFilterType::New();


  thresholdfilter->SetInput( inImage );


  thresholdfilter->SetOutsideValue( m_outsideValue );
  thresholdfilter->SetInsideValue( m_insideValue  );

  const InputPixelType lowerThreshold = level;
  //const InputPixelType upperThreshold = m_maxImageValue;
  const InputPixelType upperThreshold = max;

  thresholdfilter->SetLowerThreshold( lowerThreshold );
  thresholdfilter->SetUpperThreshold( upperThreshold );
//  thresholdfilter->Update();

/*  binaryErode1->SetErodeValue( m_insideValue );
  binaryDilate->SetDilateValue( m_insideValue );
  binaryErode2->SetErodeValue( m_insideValue );

  StructuringElementType structuringElementErode1;
  structuringElementErode1.SetRadius( 1 ); // 3x3 structuring element
  structuringElementErode1.CreateStructuringElement();

  binaryErode1->SetKernel( structuringElementErode1 );

  binaryErode1->SetInput( thresholdfilter->GetOutput() );

  StructuringElementType structuringElementDilate;
  structuringElementDilate.SetRadius( 3 ); // 3x3 structuring element
  structuringElementDilate.CreateStructuringElement();

  binaryDilate->SetKernel( structuringElementDilate );

  binaryDilate->SetInput( binaryErode1->GetOutput() );


  StructuringElementType structuringElementErode2;
  structuringElementErode2.SetRadius( 2 ); // 3x3 structuring element
  structuringElementErode2.CreateStructuringElement();

  binaryErode2->SetKernel( structuringElementErode2 );

  binaryErode2->SetInput( binaryDilate->GetOutput() );
  binaryErode2->Update();

  maskImage = binaryErode2->GetOutput();
 */
   //Afegit
  thresholdfilter->Update();
  maskImage = thresholdfilter->GetOutput();

  //Fi Afegit
}


template <typename TInputImage, typename TMaskImage>
        void udgBinaryMaker<TInputImage,TMaskImage>::PenombraSegmentation(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage, unsigned int level, unsigned int max, const typename ConnectedThresholdFilterType::IndexType & seed )
{

    // farem primer un threshold normal i al final de tot un connected
    // per evitar els problemes amb la llavor

    typename ConnectedThresholdFilterType::Pointer connectedthresholdfilter = ConnectedThresholdFilterType::New();
    typename ThresholdFilterType::Pointer thresholdfilter = ThresholdFilterType::New();
    typename DilateFilterType::Pointer binaryErode1 = DilateFilterType::New();
  typename DilateFilterType::Pointer binaryErode2 = DilateFilterType::New();
  typename ErodeFilterType::Pointer binaryDilate = ErodeFilterType::New();


  thresholdfilter->SetInput( inImage );


   thresholdfilter->SetOutsideValue( m_outsideValue );
   thresholdfilter->SetInsideValue( m_insideValue  );
//  thresholdfilter->SetReplaceValue( m_insideValue );

  const InputPixelType lowerThreshold = level;
  //const InputPixelType upperThreshold = m_maxImageValue;
  const InputPixelType upperThreshold = max;

   thresholdfilter->SetLowerThreshold( lowerThreshold );
   thresholdfilter->SetUpperThreshold( upperThreshold );
//     thresholdfilter->SetLower( lowerThreshold );
//     thresholdfilter->SetUpper( upperThreshold );
//     thresholdfilter->SetSeed( seed );
    thresholdfilter->Update();

  binaryErode1->SetDilateValue( m_insideValue );
  binaryDilate->SetErodeValue( m_insideValue );
  binaryErode2->SetDilateValue( m_insideValue );

  StructuringElementType structuringElementErode1;
  typename StructuringElementType::SizeType radiusErode1 = { { 3, 3, 1 } };
  structuringElementErode1.SetRadius( radiusErode1 ); // 3x3 structuring element
  structuringElementErode1.CreateStructuringElement();

  binaryErode1->SetKernel( structuringElementErode1 );

  binaryErode1->SetInput( thresholdfilter->GetOutput() );

  StructuringElementType structuringElementDilate;
  typename StructuringElementType::SizeType radiusDilate = { { 7, 7, 2 } };
//   structuringElementDilate.SetRadius( 3 ); // 3x3 structuring element
  structuringElementDilate.SetRadius( radiusDilate ); // 3x3 structuring element
  structuringElementDilate.CreateStructuringElement();

  binaryDilate->SetKernel( structuringElementDilate );
  binaryDilate->SetBackgroundValue( m_outsideValue );

  binaryErode1->Update();
//   maskImage = binaryErode1->GetOutput();

  binaryDilate->SetInput( binaryErode1->GetOutput() );


  StructuringElementType structuringElementErode2;
  typename StructuringElementType::SizeType radiusErode2 = { { 4, 4, 1 } };
//   structuringElementErode2.SetRadius( 2 ); // 3x3 structuring element
  structuringElementErode2.SetRadius( radiusErode2 ); // 3x3 structuring element
  structuringElementErode2.CreateStructuringElement();

  binaryErode2->SetKernel( structuringElementErode2 );

  binaryDilate->Update();
//   maskImage = binaryDilate->GetOutput();

  binaryErode2->SetInput( binaryDilate->GetOutput() );
  binaryErode2->Update();

//   maskImage = binaryErode2->GetOutput();



    connectedthresholdfilter->SetInput( binaryErode2->GetOutput() );
    connectedthresholdfilter->SetReplaceValue( m_insideValue );
    connectedthresholdfilter->SetLower( 128 );
    connectedthresholdfilter->SetUpper( 255 );
    connectedthresholdfilter->SetSeed( seed );
    connectedthresholdfilter->Update();

    maskImage = connectedthresholdfilter->GetOutput();

   //Afegit
//   thresholdfilter->Update();
//   maskImage = thresholdfilter->GetOutput();

  //Fi Afegit
}

template <typename TInputImage, typename TMaskImage>
double udgBinaryMaker<TInputImage,TMaskImage>::VolumeCalculation(typename TMaskImage::Pointer maskImage, bool x)
{

  if ( maskImage.IsNull() ) return 0.0;

  MaskIteratorType  maskIt(maskImage, maskImage->GetBufferedRegion());	//m�cara de la perfusi�
 const typename TMaskImage::SpacingType& spacing = maskImage->GetSpacing();
  //const double* spacing =  maskImage->GetSpacing();	//new double[TMaskImage::ImageDimension]
  double volume = 1;

  for(unsigned int i=0;i<TMaskImage::ImageDimension;i++)
  {
    volume *= spacing[i];
  }
  std::cout<<"spacing ="<<spacing<<std::endl;

  unsigned int cont=0;
  maskIt.GoToBegin();
  ++maskIt;

  MaskPixelType maskValue;

  if (x)
  {
      //Tenir en compte els valors incorrectes del volum amb erode i dilate
      while (!maskIt.IsAtEnd())
        {
            maskValue = maskIt.Value();

            if (maskValue != m_outsideValue && maskValue < 400)
            {
                cont++;
            }
            ++maskIt;
        }
  }
  else
  {
      while (!maskIt.IsAtEnd())
        {
            maskValue = maskIt.Value();

            if (maskValue != m_outsideValue)
            {
                cont++;
            }
            ++maskIt;
        }
  }

  volume = volume*cont;

  return volume;
}

template <typename TInputImage, typename TMaskImage>
void udgBinaryMaker<TInputImage,TMaskImage>::JoinMask(typename TMaskImage::Pointer maskImage1,typename TMaskImage::Pointer maskImage2, typename TMaskImage::Pointer maskImageOut)
{

  MaskIteratorType  maskIt1(maskImage1, maskImage1->GetBufferedRegion());
  MaskIteratorType  maskIt2(maskImage2, maskImage2->GetBufferedRegion());

  typename TMaskImage::RegionType region = maskImage1->GetLargestPossibleRegion();
  maskImageOut->SetRegions(region);
  maskImageOut->SetSpacing( maskImage1->GetSpacing() );
  maskImageOut->SetOrigin( maskImage1->GetOrigin() );
  maskImageOut->Allocate();

  MaskIteratorType  maskItOut(maskImageOut, maskImage2->GetBufferedRegion());	//m�cara de la perfusi�

  maskIt1.GoToBegin();
  maskIt2.GoToBegin();
  maskItOut.GoToBegin();
  ++maskIt1;
  ++maskIt2;
  ++maskItOut;
  MaskPixelType maskValue1;
  MaskPixelType maskValue2;

  while (!maskIt1.IsAtEnd())
  {
	  maskValue1 = maskIt1.Value();
	  maskValue2 = maskIt2.Value();

	  if (maskValue1 != m_outsideValue)
	  {
	     maskItOut.Set(1);
	  }
    else if (maskValue2 != m_outsideValue)
    {
       maskItOut.Set(2);
    }
    else
    {
      maskItOut.Set(0);
    }

	  ++maskIt1;
	  ++maskIt2;
	  ++maskItOut;
  }

}


template <typename TInputImage, typename TMaskImage>
void udgBinaryMaker<TInputImage,TMaskImage>::GetMaxImage(typename TInputImage::Pointer inImage, InputPixelType *maxImage)
{
  InputIteratorType imIt(inImage, inImage->GetBufferedRegion());

  InputPixelType max;
  imIt.GoToBegin();
  max=imIt.Value();
  ++imIt;
  while(!imIt.IsAtEnd())
  {
    if(max<imIt.Value())
    {
      max=imIt.Value();
    }
  ++imIt;
  }
   std::cout<<"Max Image: "<<(int)max<<std::endl;
  (*maxImage)=max;

}


//};

#endif
