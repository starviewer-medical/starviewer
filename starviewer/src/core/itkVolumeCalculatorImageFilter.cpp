/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkMinimumMaximumImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2004/12/22 01:52:26 $
  Version:   $Revision: 1.19 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.
eplace
     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef _itkVolumeCalculatorImageFilter_txx
#define _itkVolumeCalculatorImageFilter_txx
#include "itkVolumeCalculatorImageFilter.h"

#include "itkImageRegionIterator.h"
#include "itkImageRegionConstIterator.h"
#include "itkNumericTraits.h"
#include "itkProgressReporter.h"

#include <vector>

namespace itk {

template<class TInputImage>
VolumeCalculatorImageFilter<TInputImage>
::VolumeCalculatorImageFilter()
{
  this->SetNumberOfRequiredOutputs(1);
  // first output is a copy of the image, DataObject created by
  // superclass
  //
  // allocate the data objects for the remaining outputs which are
  // just decorators around floating point types
  typename PixelObjectType::Pointer output  = static_cast<PixelObjectType*>(this->MakeOutput(1).GetPointer());
  this->ProcessObject::SetNthOutput(1, output.GetPointer());

  m_VolumeCount = 0;
  m_VolumeReal = 0.0;
  m_InsideValue = static_cast<PixelType>(0);
}


template<class TInputImage>
DataObject::Pointer
VolumeCalculatorImageFilter<TInputImage>
::MakeOutput(unsigned int output)
{
  switch (output)
    {
   case 1:
      return static_cast<DataObject*>(TInputImage::New().GetPointer());
      break;
   default:
      // might as well make an image
      return static_cast<DataObject*>(TInputImage::New().GetPointer());
      break;
    }
}


template<class TInputImage>
void
VolumeCalculatorImageFilter<TInputImage>
::GenerateInputRequestedRegion()
{
  Superclass::GenerateInputRequestedRegion();
  if ( this->GetInput() )
    {
    InputImagePointer image =
      const_cast< typename Superclass::InputImageType * >( this->GetInput() );
    image->SetRequestedRegionToLargestPossibleRegion();
    }
}

template<class TInputImage>
void
VolumeCalculatorImageFilter<TInputImage>
::EnlargeOutputRequestedRegion(DataObject *data)
{
  Superclass::EnlargeOutputRequestedRegion(data);
  data->SetRequestedRegionToLargestPossibleRegion();
}


template<class TInputImage>
void
VolumeCalculatorImageFilter<TInputImage>
::AllocateOutputs()
{
  // Pass the input through as the output
  InputImagePointer image =
    const_cast< TInputImage * >( this->GetInput() );
  this->GraftOutput( image );

  // Nothing that needs to be allocated for the remaining outputs
}

template<class TInputImage>
void
VolumeCalculatorImageFilter<TInputImage>
::BeforeThreadedGenerateData()
{
  int numberOfThreads = this->GetNumberOfThreads();

  // Create the thread temporaries
  m_ThreadVol = std::vector<unsigned int>(numberOfThreads, 0);
}

template<class TInputImage>
void
VolumeCalculatorImageFilter<TInputImage>
::AfterThreadedGenerateData()
{
  m_VolumeCount = 0;

  std::vector<unsigned int>::iterator it;
  it= m_ThreadVol.begin();
  while(it != m_ThreadVol.end())
    {
    m_VolumeCount += (*it);
    it++;
    }

  const SpacingType& spacing = this->GetInput()->GetSpacing();
  double voxelVolume = 1;

  for(unsigned int i=0;i<InputImageDimension;i++)
  {
      voxelVolume *= spacing[i];
  }

  m_VolumeReal = static_cast<double>(m_VolumeCount)*voxelVolume;

}

template<class TInputImage>
void
VolumeCalculatorImageFilter<TInputImage>
::ThreadedGenerateData(const RegionType& outputRegionForThread,
                       int threadId)
{
  PixelType value;
  ImageRegionConstIterator<TInputImage> it (this->GetInput(), outputRegionForThread);
  it.GoToBegin();

  // support progress methods/callbacks
  ProgressReporter progress(this, threadId, outputRegionForThread.GetNumberOfPixels());

  // do the work
  while (!it.IsAtEnd())
    {
    value = static_cast<PixelType>(it.Get());
    if (value == m_InsideValue)
      {
      m_ThreadVol[threadId] ++;
      }
    ++it;
    progress.CompletedPixel();
    }
}

template <class TImage>
void
VolumeCalculatorImageFilter<TImage>
::PrintSelf(std::ostream& os, Indent indent) const
{
  Superclass::PrintSelf(os,indent);

  os << indent << "Volume: "
     << static_cast<typename NumericTraits<PixelType>::PrintType>(this->GetVolume())
     << std::endl;
}


}// end namespace itk
#endif
