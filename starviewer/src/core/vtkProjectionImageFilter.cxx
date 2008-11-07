/*=========================================================================

  Program:   Visualization Toolkit
  Module:    $RCSfile: vtkProjectionImageFilter.cxx,v $

  Copyright (c) ...
  All rights reserved.
  See Copyright.txt or http://www.kitware.com/Copyright.htm for details.

     This software is distributed WITHOUT ANY WARRANTY; without even
     the implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR
     PURPOSE.  See the above copyright notice for more information.

=========================================================================*/

#include "vtkProjectionImageFilter.h"

#include <vtkImageData.h>
#include <vtkInformation.h>
#include <vtkInformationVector.h>
#include <vtkObjectFactory.h>
#include <vtkStreamingDemandDrivenPipeline.h>

#include "logging.h"


vtkCxxRevisionMacro(vtkProjectionImageFilter, "$Revision: 1.0 $");
vtkStandardNewMacro(vtkProjectionImageFilter);


/**
 * Constructor
 */
// template <class TAccumulator>
vtkProjectionImageFilter/*<TAccumulator>*/::vtkProjectionImageFilter()
{
    ProjectionDimension = 2;
    AccumulatorType = udg::AccumulatorFactory::Maximum;
    FirstSlice = 0;
    NumberOfSlicesToProject = 1;
    Step = 1;
}


/**
 * Destructor
 */
// template <class TAccumulator>
vtkProjectionImageFilter/*<TAccumulator>*/::~vtkProjectionImageFilter()
{
}


// template <class TInputImage, class TOutputImage, class TAccumulator>
void vtkProjectionImageFilter::PrintSelf(ostream& os, vtkIndent indent)
{
    this->Superclass::PrintSelf(os,indent);

    os << indent << "ProjectionDimension: " << ProjectionDimension << "\n";
    os << indent << "AccumulatorType: " << AccumulatorType << "\n";
    os << indent << "FirstSlice: " << FirstSlice << "\n";
    os << indent << "NumberOfSlicesToProject: " << NumberOfSlicesToProject << "\n";
    os << indent << "Step: " << Step << "\n";

    os << std::flush;
}


// Change the WholeExtent
int vtkProjectionImageFilter::RequestInformation (
                                       vtkInformation * vtkNotUsed(request),
                                       vtkInformationVector **inputVector,
                                       vtkInformationVector *outputVector)
{
    // get the info objects
    vtkInformation* outInfo = outputVector->GetInformationObject(0);
    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);

    int extent[6];

    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent);

    DEBUG_LOG( QString( "inWholeExtent: %1 %2 %3 %4 %5 %6" ).arg(extent[0]).arg(extent[1]).arg(extent[2]).arg(extent[3]).arg(extent[4]).arg(extent[5]) );

    // \TODO aquí hi hauria d'anar un 0 per la versió final
//     extent[2*ProjectionDimension] = extent[2*ProjectionDimension+1] = FirstSlice;
    extent[2*ProjectionDimension] = extent[2*ProjectionDimension+1] = FirstSlice / Step;    // First = llesca * nFases + fase
                                                                                            // nFases = Step
                                                                                            // First / Step = llesca :)
//     extent[2*ProjectionDimension] = extent[2*ProjectionDimension+1] = 0;

    outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),extent,6);

    DEBUG_LOG( QString( "outWholeExtent: %1 %2 %3 %4 %5 %6" ).arg(extent[0]).arg(extent[1]).arg(extent[2]).arg(extent[3]).arg(extent[4]).arg(extent[5]) );

    return 1;
}

int vtkProjectionImageFilter::RequestUpdateExtent(
        vtkInformation *vtkNotUsed(request),
        vtkInformationVector **inputVector,
        vtkInformationVector *outputVector)
{
    int wholeExtent[6], updateExtent[6];

    vtkInformation *inInfo = inputVector[0]->GetInformationObject(0);
    vtkInformation *outInfo = outputVector->GetInformationObject(0);

    inInfo->Get(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(), wholeExtent);

    DEBUG_LOG( QString( "inWholeExtent: %1 %2 %3 %4 %5 %6" ).arg(wholeExtent[0]).arg(wholeExtent[1]).arg(wholeExtent[2]).arg(wholeExtent[3]).arg(wholeExtent[4]).arg(wholeExtent[5]) );

    outInfo->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), updateExtent);

    DEBUG_LOG( QString( "outUpdateExtent: %1 %2 %3 %4 %5 %6" ).arg(updateExtent[0]).arg(updateExtent[1]).arg(updateExtent[2]).arg(updateExtent[3]).arg(updateExtent[4]).arg(updateExtent[5]) );

    // TODO veure això de l'extent
    updateExtent[2*ProjectionDimension] = FirstSlice;
    updateExtent[2*ProjectionDimension+1] = FirstSlice + Step * ( NumberOfSlicesToProject - 1 );
//     updateExtent[2*ProjectionDimension] = 0;
//     updateExtent[2*ProjectionDimension+1] = 0;

    inInfo->Set(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), updateExtent, 6);

    DEBUG_LOG( QString( "inUpdateExtent: %1 %2 %3 %4 %5 %6" ).arg(updateExtent[0]).arg(updateExtent[1]).arg(updateExtent[2]).arg(updateExtent[3]).arg(updateExtent[4]).arg(updateExtent[5]) );

    return 1;
}


template <class T>
void vtkProjectionImageFilterExecute(vtkProjectionImageFilter *self,
                                     vtkImageData *inData, T *inPtr,
                                     vtkImageData *outData, T *outPtr,
                                     int inExt[6], int outExt[6],
                                     int vtkNotUsed(id) )
{
    unsigned int projectionDimension = self->GetProjectionDimension();
    unsigned int iA = projectionDimension, i0 = (iA + 1) % 3, i1 = (iA + 2) % 3;

    // per 1 thread això no cal, però per 2 o més potser sí
    // compute the input region for this thread
    for ( unsigned int i = 0; i < 6; i++ )
    {
//         DEBUG_LOG( QString( "inExt[%1] = %2 | outExt[%1] = %3" ).arg( i ).arg( inExt[i] ).arg( outExt[i] ) );
        if( i / 2 != projectionDimension )
        {
            inExt[i] = outExt[i];
        }
    }

    int inMin0, inMax0, inMin1, inMax1, inMinA, inMaxA;
    inMin0 = inExt[2*i0]; inMax0 = inExt[2*i0+1];
    inMin1 = inExt[2*i1]; inMax1 = inExt[2*i1+1];
    inMinA = inExt[2*iA]; inMaxA = inExt[2*iA+1];

    DEBUG_LOG( QString( "inMin0 = %1, inMax0 = %2" ).arg( inMin0 ).arg( inMax0 ) );
    DEBUG_LOG( QString( "inMin1 = %1, inMax1 = %2" ).arg( inMin1 ).arg( inMax1 ) );
    DEBUG_LOG( QString( "inMinA = %1, inMaxA = %2" ).arg( inMinA ).arg( inMaxA ) );
    /*
    switch (projectionDimension)
    {
        case 0:
            inMin0 = inExt[2]; inMax0 = inExt[3];
            inMin1 = inExt[4]; inMax1 = inExt[5];
            inMinA = inExt[0]; inMaxA = inExt[1];
            break;
        case 1:
            inMin0 = inExt[0]; inMax0 = inExt[1];
            inMin1 = inExt[4]; inMax1 = inExt[5];
            inMinA = inExt[2]; inMaxA = inExt[3];
            break;
        case 2:
            inMin0 = inExt[0]; inMax0 = inExt[1];
            inMin1 = inExt[2]; inMax1 = inExt[3];
            inMinA = inExt[4]; inMaxA = inExt[5];
            break;
    }
    */


    vtkIdType inIncs[3], outIncs[3];
    inData->GetIncrements(inIncs);
    outData->GetIncrements(outIncs);



    int outDims[3];
    outData->GetDimensions( outDims );
//     DEBUG_LOG(QString("out dims: (%1, %2, %3)").arg(outDims[0]).arg(outDims[1]).arg(outDims[2]));
//     DEBUG_LOG(QString("size = %1").arg(outData->GetPointData()->GetScalars()->GetSize()));
//     DEBUG_LOG(QString("out incs: (%1, %2, %3)").arg(outIncs[0]).arg(outIncs[1]).arg(outIncs[2]));



    vtkIdType inInc0, inInc1, inIncA, outInc0, outInc1;
    inInc0 = inIncs[i0]; inInc1 = inIncs[i1]; inIncA = inIncs[iA];
    DEBUG_LOG( QString( "inInc0 = %1" ).arg( inInc0 ) );
    DEBUG_LOG( QString( "inInc1 = %1" ).arg( inInc1 ) );
    DEBUG_LOG( QString( "inIncA = %1" ).arg( inIncA ) );
    outInc0 = outIncs[i0]; outInc1 = outIncs[i1];

    int step = self->GetStep();
    DEBUG_LOG( QString( "step = %1" ).arg( step ) );
    unsigned long projectionSize = self->GetNumberOfSlicesToProject();

    // instantiate the accumulator
    udg::Accumulator<T> * accumulator = udg::AccumulatorFactory::getAccumulator<T>(self->GetAccumulatorType(), projectionSize);

    // loop through pixels of input
    T * inPtr0 = inPtr;
    T * outPtr0 = outPtr;

    for ( int inIdx0 = inMin0; inIdx0 <= inMax0 && !self->AbortExecute; ++inIdx0 )
    {
        T * inPtr1 = inPtr0;
        T * outPtr1 = outPtr0;

        for ( int inIdx1 = inMin1; inIdx1 <= inMax1 && !self->AbortExecute; ++inIdx1 )
        {
            // init the accumulator before a new set of pixels
            accumulator->initialize();

            T * inPtrA = inPtr1;

            for ( int inIdxA = inMinA; inIdxA <= inMaxA && !self->AbortExecute; inIdxA += step )
            {
                accumulator->accumulate( *inPtrA );
                inPtrA += (step * inIncA);
            }

            *outPtr1 = accumulator->getValue();

            inPtr1 += inInc1;
            outPtr1 += outInc1;
        }

        inPtr0 += inInc0;
        outPtr0 += outInc0;
    }
    delete accumulator;
}


void vtkProjectionImageFilter::ThreadedRequestData(vtkInformation *vtkNotUsed(request),
                                                   vtkInformationVector **inputVector,
                                                   vtkInformationVector *vtkNotUsed(outputVector),
                                                   vtkImageData ***inData, vtkImageData **outData,
                                                   int outExt[6], int id)
{
    DEBUG_LOG( QString( "outExt: %1 %2 %3 %4 %5 %6" ).arg(outExt[0]).arg(outExt[1]).arg(outExt[2]).arg(outExt[3]).arg(outExt[4]).arg(outExt[5]) );
    void *outPtr = outData[0]->GetScalarPointerForExtent(outExt);
    void *outPtr0 = outData[0]->GetScalarPointer();
    int out1_0 = (int)outPtr - (int)outPtr0;
    DEBUG_LOG( QString( "outPtr - outPtr0 = %1" ).arg( out1_0 ) );

    int inExt[6];
    inputVector[0]->GetInformationObject(0)->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inExt);
    DEBUG_LOG( QString( "inExt: %1 %2 %3 %4 %5 %6" ).arg(inExt[0]).arg(inExt[1]).arg(inExt[2]).arg(inExt[3]).arg(inExt[4]).arg(inExt[5]) );

    /// \warning Això funciona amb 2 nuclis, però amb més no ho sé!!!!!!!!!!!!!!!!
    /// \warning Copiem la X i la Y de l'outExt
    inExt[0] = outExt[0]; inExt[1] = outExt[1];
    inExt[2] = outExt[2]; inExt[3] = outExt[3];

    void *inPtr = inData[0][0]->GetScalarPointerForExtent(inExt);
    void *inPtr0 = inData[0][0]->GetScalarPointer();
    int in1_0 = (int)inPtr - (int)inPtr0;
    DEBUG_LOG( QString( "inPtr - inPtr0 = %1" ).arg( in1_0 ) );

    /// \warning Restaurem el valor original
    inputVector[0]->GetInformationObject(0)->Get(vtkStreamingDemandDrivenPipeline::UPDATE_EXTENT(), inExt);

    // this filter expects the output type to be same as input
    if (outData[0]->GetScalarType() != inData[0][0]->GetScalarType())
    {
        vtkErrorMacro(<< "Execute: output ScalarType, "
                      << vtkImageScalarTypeNameMacro(outData[0]->GetScalarType())
                      << " must match input scalar type");
        return;
    }

    switch (inData[0][0]->GetScalarType())
    {
        vtkTemplateMacro(
                        vtkProjectionImageFilterExecute(this,
                            inData[0][0], reinterpret_cast<VTK_TT *>(inPtr),
                            outData[0], reinterpret_cast<VTK_TT *>(outPtr),
                            inExt, outExt,
                            id));

    default:
        vtkErrorMacro(<< "Execute: Unknown ScalarType");
        return;
    }
}
