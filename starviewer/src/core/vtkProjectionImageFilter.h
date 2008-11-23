/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkProjectionImageFilter.h,v $

  Copyright (c) ...
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#ifndef __vtkProjectionImageFilter_h
#define __vtkProjectionImageFilter_h

#include <vtkThreadedImageAlgorithm.h>
#include "accumulator.h"


/** \class vtkProjectionImageFilter
 * \brief Implements an accumulation of an image along a selected direction.
 *
 *    This class accumulates an image along a dimension and reduce the size 
 * of this dimension to 1. The dimension being accumulated is set by 
 * ProjectionDimension. 
 *
 *   Each pixel is the cumulative sum of the pixels along the collapsed
 * dimension and reduce the size of the accumulated dimension to 1 (only 
 * on the accumulated). 
 *
 *   The dimensions of the InputImage and the OutputImage must be the same.
 *
 * This class is parameterized over the type of the input image and
 * the type of the output image.
 *
 * This class was contributed to the Insight Journal by Emilian Beronich and
 * Gaetan Lehmann. The original paper can be found at 
 *      http://hdl.handle.net/1926/164
 *
 * \author Emiliano Beronich
 * \author Ga�tan Lehmann. Biologie du D�veloppement et de la Reproduction,
 * INRA de Jouy-en-Josas, France.
 *
 *
 * \sa AccumulateImageFilter
 * \ingroup   IntensityImageFilters     Singlethreaded
 */


// template <template <class T> class TAccumulator >
class vtkProjectionImageFilter : public vtkThreadedImageAlgorithm
{
public:
    static vtkProjectionImageFilter *New();
    vtkTypeRevisionMacro(vtkProjectionImageFilter,vtkThreadedImageAlgorithm);
    void PrintSelf( ostream& os, vtkIndent indent );

//     typedef TAccumulator<class T> AccumulatorType;


    /** Set/Get the direction in which to accumulate the data.  It must be set
    * before the update of the filter. Defaults to the last dimension. */
    vtkSetMacro( ProjectionDimension, unsigned int );
    vtkGetMacro( ProjectionDimension, unsigned int );

    vtkSetMacro( AccumulatorType, udg::AccumulatorFactory::AccumulatorType );
    vtkGetMacro( AccumulatorType, udg::AccumulatorFactory::AccumulatorType );

    vtkSetMacro( FirstSlice, int );
    vtkGetMacro( FirstSlice, int );

    vtkSetMacro( NumberOfSlicesToProject, int );
    vtkGetMacro( NumberOfSlicesToProject, int );

    vtkSetMacro( Step, int );
    vtkGetMacro( Step, int );


protected:
    vtkProjectionImageFilter();
    virtual ~vtkProjectionImageFilter();

    virtual int RequestInformation (vtkInformation *,
                                    vtkInformationVector **,
                                    vtkInformationVector *);
    virtual int RequestUpdateExtent (vtkInformation *, vtkInformationVector **, vtkInformationVector *);

    void ThreadedRequestData(vtkInformation *request,
                             vtkInformationVector **inputVector,
                             vtkInformationVector *outputVector,
                             vtkImageData ***inData, vtkImageData **outData,
                             int outExt[6], int id);

  /** Apply changes to the output image information. */
//   virtual void GenerateOutputInformation();

  /** Apply changes to the input image requested region. */
//   virtual void GenerateInputRequestedRegion();

//   virtual void ThreadedGenerateData( 
//    const OutputImageRegionType& outputRegionForThread, int threadId );
//     template <class T> Accumulator<T> NewAccumulator();
    


private:
    vtkProjectionImageFilter(const vtkProjectionImageFilter&);  // Not implemented.
    void operator=(const vtkProjectionImageFilter&);  // Not implemented.

    unsigned int ProjectionDimension;
    udg::AccumulatorFactory::AccumulatorType AccumulatorType;
    int FirstSlice;
    int NumberOfSlicesToProject;
    int Step;

};


#endif
