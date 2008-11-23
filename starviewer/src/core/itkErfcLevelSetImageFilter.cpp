/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkErfcLevelSetImageFilter.txx,v $
  Language:  C++
  Date:      $Date: 2003/09/10 14:28:39 $
  Version:   $Revision: 1.11 $

  Copyright (c) Insight Software Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkErfcLevelSetImageFilter_txx_
#define __itkErfcLevelSetImageFilter_txx_

#include "itkErfcLevelSetImageFilter.h"

namespace itk {


template <class TInputImage, class TFeatureImage, class TMaskImage, class TOutputType>
ErfcLevelSetImageFilter<TInputImage, TFeatureImage, TMaskImage, TOutputType>
::ErfcLevelSetImageFilter()
{
  m_ErfcFunction = ErfcFunctionType::New();
  m_ErfcFunction->SetUpperThreshold(0);
  m_ErfcFunction->SetLowerThreshold(0);

  this->SetSegmentationFunction(m_ErfcFunction);
}

template <class TInputImage, class TFeatureImage, class TMaskImage, class TOutputType>
void
ErfcLevelSetImageFilter<TInputImage, TFeatureImage, TMaskImage, TOutputType>
::PrintSelf(std::ostream &os, Indent itkNotUsed(indent)) const
{
  //Superclass::PrintSelf(os, indent);
  os << "ErfcFunction: " << m_ErfcFunction;
}


}// end namespace itk




#endif
