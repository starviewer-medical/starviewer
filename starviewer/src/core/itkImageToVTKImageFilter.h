/*=========================================================================

  Program:   Insight Segmentation & Registration Toolkit
  Module:    $RCSfile: itkImageToVTKImageFilter.h,v $
  Language:  C++
  Date:      $Date: 2004/02/19 03:47:59 $
  Version:   $Revision: 1.6 $

  Copyright (c) 2002 Insight Consortium. All rights reserved.
  See ITKCopyright.txt or http://www.itk.org/HTML/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notices for more information.

=========================================================================*/
#ifndef __itkImageToVTKImageFilter_h
#define __itkImageToVTKImageFilter_h

#include "itkVTKImageExport.h"
#include <vtkImageImport.h>
#include "vtkImageData.h"

namespace itk
{

/** \class ImageToVTKImageFilter
 * \brief Converts an ITK image into a VTK image and plugs a
 *  itk data pipeline to a VTK datapipeline.
 *
 *  This class puts together an itkVTKImageExporter and a vtkImageImporter.
 *  It takes care of the details related to the connection of ITK and VTK
 *  pipelines. The User will perceive this filter as an adaptor to which
 *  an itk::Image can be plugged as input and a vtkImage is produced as
 *  output.
 *
 * \ingroup   ImageFilters
 */
template <class TInputImage >
class ITK_EXPORT ImageToVTKImageFilter : public ProcessObject
{
public:
  /** Standard class typedefs. */
  typedef ImageToVTKImageFilter       Self;
  typedef ProcessObject             Superclass;
  typedef SmartPointer<Self>        Pointer;
  typedef SmartPointer<const Self>  ConstPointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(ImageToVTKImageFilter, ProcessObject);

  /** Some typedefs. */
  typedef TInputImage InputImageType;
  typedef typename    InputImageType::ConstPointer    InputImagePointer;
  typedef VTKImageExport< InputImageType>            ExporterFilterType;
  typedef typename ExporterFilterType::Pointer        ExporterFilterPointer;

  /** Get the output in the form of a vtkImage.
      This call is delegated to the internal vtkImageImporter filter  */
  vtkImageData *  GetOutput() const;

  /** Set the input in the form of an itk::Image */
  void SetInput( const InputImageType * );

  /** Return the internal VTK image importer filter.
      This is intended to facilitate users the access
      to methods in the importer */
  vtkImageImport * GetImporter() const;

  /** Return the internal ITK image exporter filter.
      This is intended to facilitate users the access
      to methods in the exporter */
  ExporterFilterType * GetExporter() const;

  /** This call delegate the update to the importer */
  void Update() override;

protected:
  ImageToVTKImageFilter();
  ~ImageToVTKImageFilter() override;

private:
  ImageToVTKImageFilter(const Self&); //purposely not implemented
  void operator=(const Self&); //purposely not implemented

  ExporterFilterPointer       m_Exporter;
  vtkImageImport            * m_Importer;

};

} // end namespace itk

#ifndef ITK_MANUAL_INSTANTIATION
#include "itkImageToVTKImageFilter.txx"
#endif

#endif



