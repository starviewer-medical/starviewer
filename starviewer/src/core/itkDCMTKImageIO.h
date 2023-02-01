/*************************************************************************************
  Copyright (C) 2014 Laboratori de Gràfics i Imatge, Universitat de Girona &
  Institut de Diagnòstic per la Imatge.
  Girona 2014. All rights reserved.
  http://starviewer.udg.edu

  This file is part of the Starviewer (Medical Imaging Software) open source project.
  It is subject to the license terms in the LICENSE file found in the top-level
  directory of this distribution and at http://starviewer.udg.edu/license. No part of
  the Starviewer (Medical Imaging Software) open source project, including this file,
  may be copied, modified, propagated, or distributed except according to the
  terms contained in the LICENSE file.

  This file incorporates work covered by the following copyright and
  permission notice:

    Copyright Insight Software Consortium

    Licensed under the Apache License, Version 2.0 (the "License");
    you may not use this file except in compliance with the License.
    You may obtain a copy of the License at

           http://www.apache.org/licenses/LICENSE-2.0.txt

    Unless required by applicable law or agreed to in writing, software
    distributed under the License is distributed on an "AS IS" BASIS,
    WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
    See the License for the specific language governing permissions and
    limitations under the License.
 *************************************************************************************/

#ifndef __itkDCMTKImageIO_h
#define __itkDCMTKImageIO_h


#include <fstream>
#include <stdio.h>
#include "itkImageIOBase.h"
#include "dcmtk/dcmimgle/dcmimage.h"

namespace itk
{
/** \class DCMTKImageIO
 *
 *  \brief Read DICOM image file format.
 *
 *  \ingroup IOFilters
 *
 * \ingroup ITKIODCMTK
 */
class ITK_EXPORT DCMTKImageIO:public ImageIOBase
{
public:
  /** Standard class typedefs. */
  typedef DCMTKImageIO              Self;
  typedef ImageIOBase               Superclass;
  typedef SmartPointer< Self >      Pointer;

  /** Method for creation through the object factory. */
  itkNewMacro(Self);

  /** Run-time type information (and related methods). */
  itkTypeMacro(DCMTKImageIO, Superclass);

  /** */
  void SetDicomImagePointer( DicomImage* UserProvided)
    {
    m_DImage = UserProvided;
    m_DicomImageSetByUser = true;
    };

  /*-------- This part of the interfaces deals with reading data. ----- */

  /** Determine the file type. Returns true if this ImageIO can read the
   * file specified. */
  bool CanReadFile(const char *) override;

  /** Set the spacing and dimension information for the set filename. */
  void ReadImageInformation() override;

  /** Reads the data from disk into the memory buffer provided. */
  void Read(void *buffer) override;

  /*-------- This part of the interfaces deals with writing data. ----- */

  /** Determine the file type. Returns true if this ImageIO can write the
   * file specified. */
  bool CanWriteFile(const char *) override;

  /** Set the spacing and dimension information for the set filename. */
  void WriteImageInformation() override;

  /** Writes the data to disk from the memory buffer provided. Make sure
   * that the IORegions has been set properly. */
  void Write(const void *buffer) override;

  /** If true, a multiframe image is read frame by frame, otherwise all frames are read at once. */
  bool GetReadFrameByFrame() const;
  void SetReadFrameByFrame(bool readFrameByFrame);

  DCMTKImageIO();
  ~DCMTKImageIO() override;
  void PrintSelf(std::ostream & os, Indent indent) const override;

private:
  DCMTKImageIO(const Self &);     //purposely not implemented
  void operator=(const Self &); //purposely not implemented

  void OpenDicomImage();

  /*----- internal helpers --------------------------------------------*/
  bool m_UseJPEGCodec;
  bool m_UseJPLSCodec;
  bool m_UseRLECodec;

  DicomImage* m_DImage;

  bool m_DicomImageSetByUser;

  double      m_RescaleSlope;
  double      m_RescaleIntercept;
  std::string m_LastFileName;

  bool m_ReadFrameByFrame;
};
} // end namespace itk

#endif // __itkDCMTKImageIO_h
