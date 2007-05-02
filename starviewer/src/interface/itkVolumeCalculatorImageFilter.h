/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkVolumeCalculatorImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2006/03/29 14:53:40 $
  Version:   $Revision: 1.12 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even 
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR 
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkVolumeCalculatorImageFilter_h
#define __itkVolumeCalculatorImageFilter_h

#include <itkImageToImageFilter.h>
#include <itkSimpleDataObjectDecorator.h>

#include <vector>

#include <itkNumericTraits.h>

namespace itk
{
/** \class VolumeCalculatorImageFilter
 * \brief Compute the number of pixels equals than a certain value.
 *
 * VolumeCalculatorImageFilter computes the number of pixels equals 
 * than a certain value. It is usefull for compute the volume of a mask.
 * 
 * \ingroup Operators
 * \sa StatisticsImageFilter
 */
template <class TInputImage>
class ITK_EXPORT VolumeCalculatorImageFilter :
    public ImageToImageFilter< TInputImage, TInputImage>
{
public:
  /** Extract dimension from input image. */
  itkStaticConstMacro(InputImageDimension, unsigned int,
                      TInputImage::ImageDimension);
  itkStaticConstMacro(OutputImageDimension, unsigned int,
                      TInputImage::ImageDimension);

  /** Standard class typedefs. */
  typedef VolumeCalculatorImageFilter Self;
  typedef ImageToImageFilter< TInputImage, TInputImage> Superclass;
  typedef SmartPointer<Self> Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Image related typedefs. */
  typedef typename TInputImage::Pointer InputImagePointer;

  typedef typename TInputImage::RegionType RegionType ;
  typedef typename TInputImage::SizeType SizeType ;
  typedef typename TInputImage::SpacingType SpacingType ;
  typedef typename TInputImage::IndexType IndexType ;
  typedef typename TInputImage::PixelType PixelType ;

  /** Smart Pointer type to a DataObject. */
  typedef typename DataObject::Pointer DataObjectPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(VolumeCalculatorImageFilter, ImageToImageFilter);
  
  /** Image typedef support. */
  typedef TInputImage InputImageType;

  /** Type of DataObjects used for scalar outputs */
  typedef SimpleDataObjectDecorator<PixelType> PixelObjectType;

  /** Get the real world volume value. */
  itkGetConstMacro( VolumeReal, double );

  /** Get the real world volume value. */
  itkGetConstMacro( VolumeCount, unsigned int );

  /** Return the computed Volume. */
  double GetVolume() const
    { return this->GetVolumeReal(); }
  
  /** Set the inside mask value. */
  itkSetMacro( InsideValue, PixelType );

  /** Make a DataObject of the correct type to be used as the specified
   * output. */
  virtual DataObjectPointer MakeOutput(unsigned int idx);

protected:
  VolumeCalculatorImageFilter();
  virtual ~VolumeCalculatorImageFilter() {}
  void PrintSelf(std::ostream& os, Indent indent) const;

  /** Pass the input through unmodified. Do this by Grafting in the AllocateOutputs method. */
  void AllocateOutputs();      

  /** Initialize some accumulators before the threads run. */
  void BeforeThreadedGenerateData ();
  
  /** Do final mean and variance computation from data accumulated in threads. */
  void AfterThreadedGenerateData ();
  
  /** Multi-thread version GenerateData. */
  void  ThreadedGenerateData (const RegionType& 
                              outputRegionForThread,
                              int threadId) ;

  // Override since the filter needs all the data for the algorithm
  void GenerateInputRequestedRegion();

  // Override since the filter produces all of its output
  void EnlargeOutputRequestedRegion(DataObject *data);

private:
  VolumeCalculatorImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented
  
  std::vector<unsigned int> m_ThreadVol;
  unsigned int m_VolumeCount;
  double m_VolumeReal;
  PixelType m_InsideValue;
};
  
} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkVolumeCalculatorImageFilter.cpp"
#endif

#endif
