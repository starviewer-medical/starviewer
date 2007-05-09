#ifndef __UDGBINARYMAKER_H_
#define __UDGBINARYMAKER_H_


#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkBinaryThresholdImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"
#include "itkConnectedThresholdImageFilter.h"


namespace udg {

template <typename TInputImage, typename TMaskImage>
class udgBinaryMaker{
public:
     typedef TInputImage  InputImageType;  
     typedef TMaskImage   MaskImageType;

     typedef  typename InputImageType::PixelType   InputPixelType;
     typedef  typename MaskImageType::PixelType  MaskPixelType;
     
     static const unsigned int ImageDimension = TInputImage::ImageDimension;
       
     typedef typename itk::ImageRegionIterator<InputImageType> InputIteratorType;
     typedef typename itk::ImageRegionIterator<MaskImageType>  MaskIteratorType;
     
     typedef itk::BinaryThresholdImageFilter<InputImageType, MaskImageType >				ThresholdFilterType;
     typedef itk::ConnectedThresholdImageFilter<InputImageType, MaskImageType >				ConnectedThresholdFilterType;
     typedef itk::BinaryBallStructuringElement<InputPixelType,ImageDimension >				StructuringElementType;
     typedef itk::BinaryErodeImageFilter< InputImageType,MaskImageType,StructuringElementType >	ErodeFilterType;
     typedef itk::BinaryDilateImageFilter<InputImageType,MaskImageType,StructuringElementType >	DilateFilterType;
      
    typedef itk::ImageFileReader< InputImageType >  ReaderType;
    typedef itk::ImageFileWriter< InputImageType >  WriterType;

    
    udgBinaryMaker();

    ~udgBinaryMaker();
    
    void SetImage(InputImageType *image) {m_Image = image;}
    InputImageType *GetImage() {return m_Image;}
    void SetMask(MaskImageType *mask) {m_Mask = mask;}
    MaskImageType *GetMask() {return m_Mask;}
    void SetInsideValue(MaskPixelType in)  { m_insideValue = in;  }
    void SetOutsideValue(MaskPixelType out)  { m_outsideValue = out;  }
    
    void MascaraDifusio(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage);
    void MascaraPerfusio(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage);
    void VentricleSegmentation(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage, unsigned int level = 20);
    void StrokeSegmentation(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &mask, unsigned int level = 150, unsigned int max = 256);
    void PenombraSegmentation(typename TInputImage::Pointer inImage, typename TMaskImage::Pointer &maskImage, unsigned int level, unsigned int max, const typename ConnectedThresholdFilterType::IndexType & seed );
    double VolumeCalculation(typename TMaskImage::Pointer maskImage, bool x = false);
    void JoinMask(typename TMaskImage::Pointer maskImage1,typename TMaskImage::Pointer maskImage2, typename TMaskImage::Pointer maskImageOut);
   
private:
  void GetMaxImage(typename TInputImage::Pointer inImage, InputPixelType *maxImage);

  typename InputImageType::Pointer m_Image;
  typename MaskImageType::Pointer m_Mask;
  
  MaskPixelType m_outsideValue;
  MaskPixelType m_insideValue;
  
  InputPixelType m_minImageValue;
  InputPixelType m_maxImageValue;

};

//#ifndef ITK_MANUAL_INSTANTIATION
#include "udgBinaryMaker.cpp"
//#endif

//template class udgBinaryMaker<itk::Image<unsigned char,2>,itk::Image<unsigned char,2>>;

};


#endif
