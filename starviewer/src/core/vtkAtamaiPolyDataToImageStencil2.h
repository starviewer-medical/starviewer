/*=========================================================================

Copyright (c) 2004 Atamai, Inc.

Use, modification and redistribution of the software, in source or
binary forms, are permitted provided that the following terms and
conditions are met:

1) Redistribution of the source code, in verbatim or modified
  form, must retain the above copyright notice, this license,
  the following disclaimer, and any notices that refer to this
  license and/or the following disclaimer.

2) Redistribution in binary form must include the above copyright
  notice, a copy of this license and the following disclaimer
  in the documentation or with other materials provided with the
  distribution.

3) Modified copies of the source code must be clearly marked as such,
  and must not be misrepresented as verbatim copies of the source code.

THE COPYRIGHT HOLDERS AND/OR OTHER PARTIES PROVIDE THE SOFTWARE "AS IS"
WITHOUT EXPRESSED OR IMPLIED WARRANTY INCLUDING, BUT NOT LIMITED TO,
THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
PURPOSE.  IN NO EVENT SHALL ANY COPYRIGHT HOLDER OR OTHER PARTY WHO MAY
MODIFY AND/OR REDISTRIBUTE THE SOFTWARE UNDER THE TERMS OF THIS LICENSE
BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL OR CONSEQUENTIAL DAMAGES
(INCLUDING, BUT NOT LIMITED TO, LOSS OF DATA OR DATA BECOMING INACCURATE
OR LOSS OF PROFIT OR BUSINESS INTERRUPTION) ARISING IN ANY WAY OUT OF
THE USE OR INABILITY TO USE THE SOFTWARE, EVEN IF ADVISED OF THE
POSSIBILITY OF SUCH DAMAGES.

=========================================================================*/
// .NAME vtkAtamaiPolyDataToImageStencil2

#ifndef __vtkAtamaiPolyDataToImageStencil2_h
#define __vtkAtamaiPolyDataToImageStencil2_h

#include "vtkImageStencilSource.h"
#include "vtkPolyData.h"

class vtkMergePoints;

class VTK_EXPORT vtkAtamaiPolyDataToImageStencil2 : public vtkImageStencilSource
{
public:
 static vtkAtamaiPolyDataToImageStencil2* New();
 vtkTypeMacro(vtkAtamaiPolyDataToImageStencil2, vtkImageStencilSource);
 void PrintSelf(ostream& os, vtkIndent indent);

 // Description:
 // Specify the implicit function to convert into a stencil.
 virtual void SetInput(vtkPolyData*);
 vtkPolyData *GetInput();

protected:
 vtkAtamaiPolyDataToImageStencil2();
 ~vtkAtamaiPolyDataToImageStencil2();

 void ThreadedExecute(vtkImageStencilData *output,
                      int extent[6], int threadId);

 static void DataSetCutter(vtkDataSet *input, vtkPolyData *output,
                           double z, vtkMergePoints *locator);

#if (VTK_MAJOR_VERSION > 4) || (VTK_MINOR_VERSION > 4)
 virtual int RequestData(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
 virtual int RequestInformation(vtkInformation *, vtkInformationVector **, vtkInformationVector *);
 virtual int FillInputPortInformation(int, vtkInformation*);
#endif

private:
 vtkAtamaiPolyDataToImageStencil2(const vtkAtamaiPolyDataToImageStencil2&);  // Not implemented.
 void operator=(const vtkAtamaiPolyDataToImageStencil2&);  // Not implemented.
};

#endif
