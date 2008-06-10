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
#include "vtkAtamaiPolyDataToImageStencil2.h"
#include "vtkImageStencilData.h"
#include "vtkObjectFactory.h"

// don't need all of these
#include "vtkLine.h"
#include "vtkCellArray.h"
#include "vtkDataArray.h"
#include "vtkDoubleArray.h"
#include "vtkMergePoints.h"
#include "vtkPoints.h"
#include "vtkPointData.h"
#include "vtkCellData.h"
#include "vtkGenericCell.h"
#include "vtkMath.h"
#include "vtkLine.h"
#if (VTK_MAJOR_VERSION > 4) || (VTK_MINOR_VERSION > 4)
#include "vtkInformation.h"
#include "vtkInformationVector.h"
#include "vtkStreamingDemandDrivenPipeline.h"
#endif

#include <iostream>

#if ((VTK_MAJOR_VERSION == 4)&&(VTK_MINOR_VERSION <= 2))
#include <vector>
#include <algorithm>
#else
#include <vtkstd/vector>
#include <vtkstd/algorithm>
#endif


vtkStandardNewMacro(vtkAtamaiPolyDataToImageStencil2);

//----------------------------------------------------------------------------
vtkAtamaiPolyDataToImageStencil2::vtkAtamaiPolyDataToImageStencil2()
{
}

//----------------------------------------------------------------------------
vtkAtamaiPolyDataToImageStencil2::~vtkAtamaiPolyDataToImageStencil2()
{
}

void vtkAtamaiPolyDataToImageStencil2::SetInput(vtkPolyData *input)
{
#if (VTK_MAJOR_VERSION == 4) && (VTK_MINOR_VERSION <= 4)
 this->vtkProcessObject::SetNthInput(0, input);
#else
 if (input)
   {
   this->SetInputConnection(0, input->GetProducerPort());
   }
 else
   {
   this->SetInputConnection(0, 0);
   }
#endif
}

//----------------------------------------------------------------------------
vtkPolyData *vtkAtamaiPolyDataToImageStencil2::GetInput()
{
#if (VTK_MAJOR_VERSION == 4) && (VTK_MINOR_VERSION <= 4)
 if (this->NumberOfInputs < 1)
   {
   return NULL;
   }

 return vtkPolyData::SafeDownCast(this->Inputs[0]);
#else
 if (this->GetNumberOfInputConnections(0) < 1)
   {
   return NULL;
   }

 return vtkPolyData::SafeDownCast(
   this->GetExecutive()->GetInputData(0, 0));
#endif
}

//----------------------------------------------------------------------------
void vtkAtamaiPolyDataToImageStencil2::PrintSelf(ostream& os,
                                                vtkIndent indent)
{
 this->Superclass::PrintSelf(os,indent);

 os << indent << "Input: " << this->GetInput() << "\n";
}

//----------------------------------------------------------------------------
// This method was taken from vtkCutter and slightly modified
void vtkAtamaiPolyDataToImageStencil2::DataSetCutter(
 vtkDataSet *input, vtkPolyData *output, double z, vtkMergePoints *locator)
{
 vtkIdType cellId, i;
 vtkPoints *cellPts;
 vtkDoubleArray *cellScalars;
 vtkGenericCell *cell;
 vtkCellArray *newVerts, *newLines, *newPolys;
 vtkPoints *newPoints;
 double s;
 vtkIdType estimatedSize, numCells=input->GetNumberOfCells();
 int numCellPts;
 vtkPointData *inPD, *outPD;
 vtkCellData *inCD=input->GetCellData(), *outCD=output->GetCellData();
 vtkIdList *cellIds;

 cellScalars=vtkDoubleArray::New();

 // Create objects to hold output of contour operation
 estimatedSize = (vtkIdType) pow ((double) numCells, .75);
 estimatedSize = estimatedSize / 1024 * 1024; //multiple of 1024
 if (estimatedSize < 1024)
   {
   estimatedSize = 1024;
   }

 newPoints = vtkPoints::New();
 newPoints->Allocate(estimatedSize,estimatedSize/2);
 newVerts = vtkCellArray::New();
 newVerts->Allocate(estimatedSize,estimatedSize/2);
 newLines = vtkCellArray::New();
 newLines->Allocate(estimatedSize,estimatedSize/2);
 newPolys = vtkCellArray::New();
 newPolys->Allocate(estimatedSize,estimatedSize/2);

 // Interpolate data along edge.
 inPD = input->GetPointData();
 outPD = output->GetPointData();
 outPD->InterpolateAllocate(inPD,estimatedSize,estimatedSize/2);
 outCD->CopyAllocate(inCD,estimatedSize,estimatedSize/2);

 // locator used to merge potentially duplicate points
 //
 locator->InitPointInsertion (newPoints, input->GetBounds());

 // Compute some information for progress methods
 //
 cell = vtkGenericCell::New();

 // Loop over all cells; get scalar values for all cell points
 // and process each cell.
 //
 for (cellId=0; cellId < numCells; cellId++)
   {
   input->GetCell(cellId,cell);
   cellPts = cell->GetPoints();
   cellIds = cell->GetPointIds();

   numCellPts = cellPts->GetNumberOfPoints();
   cellScalars->SetNumberOfTuples(numCellPts);
   for (i=0; i < numCellPts; i++)
     {
     // scalar value is distance from the specified z plane
     s = input->GetPoint(cellIds->GetId(i))[2] - z;
     cellScalars->SetTuple(i,&s);
     }

   cell->Contour(0.0, cellScalars, locator,
                 newVerts, newLines, newPolys, inPD, outPD,
                 inCD, cellId, outCD);
   }

 // Update ourselves.  Because we don't know upfront how many verts, lines,
 // polys we've created, take care to reclaim memory.
 //
 cell->Delete();
 cellScalars->Delete();

 output->SetPoints(newPoints);
 newPoints->Delete();

 if (newVerts->GetNumberOfCells())
   {
   output->SetVerts(newVerts);
   }
 newVerts->Delete();

 if (newLines->GetNumberOfCells())
   {
   output->SetLines(newLines);
   }
 newLines->Delete();

 if (newPolys->GetNumberOfCells())
   {
   output->SetPolys(newPolys);
   }
 newPolys->Delete();

 locator->Initialize();//release any extra memory
 output->Squeeze();
}


//--------------------------------------------------------------------------------

static void vtkFloatingEndPointLine2D(double pt1[2], double pt2[2],
                              int z, int zInc, int ymin, int ymax,
                              vtkstd::vector< vtkstd::vector<int> >& zyBucket)
{
 if (pt1[1] > pt2[1])
   {
   vtkstd::swap(pt1,pt2);
   }
 double x0, xN, y0, yN ;
 double dx, dy;
 int X0, XN, Y0, YN, Xi, X, Y;
 float invGrad;
 float C;

 x0 = pt1[0];
 xN = pt2[0];
 y0 = pt1[1];
 yN = pt2[1];

 if (x0 == xN && y0 == yN)
  {
  return;
  }

 // Calculate and draw the two end points
 Y0 =  vtkMath::Round( y0 );
 YN =  vtkMath::Round( yN );

 X0 =  vtkMath::Round( x0 );
 XN =  vtkMath::Round( xN );

 if (Y0 == YN)
  {
  return;
  }

 zyBucket[z*zInc+Y0].push_back( X0 );

 dx = xN - x0;
 dy = yN - y0;

 if (X0 == XN) // parallel to x-axis
  {
  invGrad = 0;
  C = X0;
  }
 else // parallel to y-axis
  {
  invGrad = dx/dy;
  C = -( xN*y0 - x0*yN )/dy;
  }

 int fixInvGrad = int(invGrad*65536);
 int fixC = int(C*65536);

 int Ymid = Y0 + (YN - Y0)/2;

 //segment 1
 Y = Y0 + 1;
 X = Y*fixInvGrad + fixC ;
 while ( Y <= Ymid )
  {
  Xi = ((X >> 15) + 1) >> 1;
  zyBucket[z*zInc+Y].push_back( Xi );
  X = X + fixInvGrad;
  Y++;
  }

 // segment 2
 Y = YN - 1;
 X = Y*fixInvGrad + fixC ;
 while ( Y > Ymid )
  {
  Xi = ((X >> 15) + 1) >> 1;
  zyBucket[z*zInc+Y].push_back( Xi );
  X = X - fixInvGrad;
  Y--;
  }

}

//-----------------------------------------------------------------------------
template<class T>
static double DistanceMeasure( T *point1, T *point2 )
{
 double x = point1[0] - point2[0];
 double y = point1[1] - point2[1];
 double z = point1[2] - point2[2];

 double distance = sqrt( x*x + y*y + z*z );

 return distance;
}

static double DistanceMeasure( float *point1, float *point2 )
{
 double x = point1[0] - point2[0];
 double y = point1[1] - point2[1];
 double z = point1[2] - point2[2];

 double distance = sqrt( x*x + y*y + z*z );

 return distance;

}

//----------------------------------------------------------------------------
void vtkAtamaiPolyDataToImageStencil2::ThreadedExecute(
 vtkImageStencilData *data,
 int extent[6],
 int id)
{
#if (VTK_MAJOR_VERSION == 4) && (VTK_MINOR_VERSION <= 3)
 float *spacing = data->GetSpacing();
 float *origin = data->GetOrigin();
#else
 double *spacing = data->GetSpacing();
 double *origin = data->GetOrigin();
#endif

 // if we have no data then return
 if (!this->GetInput()->GetNumberOfPoints())
   {
   return;
   }

 // Only divide once
 double invspacing[3];
 invspacing[0] = 1.0/spacing[0];
 invspacing[1] = 1.0/spacing[1];
 invspacing[2] = 1.0/spacing[2];

 // get the input data
 vtkPolyData *input = this->GetInput();

 // the locator to use with the data
 vtkMergePoints *locator = vtkMergePoints::New();

 // the output produced by cutting the polydata with the Z plane
 vtkPolyData *slice = vtkPolyData::New();

 // the polylines from the cut
 vtkCellArray *lines;
 int ptId1, ptId2;
 vtkIdType *pts, npts;
 pts = NULL;

#if (VTK_MAJOR_VERSION == 4) && (VTK_MINOR_VERSION <= 3)
 float point1[3], point2[3];
#else
 double point1[3], point2[3];
#endif
 int end1[2], end2[2];

 int result;

 // Determine data dimensions
 int dims[3];
 dims[0] = (extent[1]-extent[0])+1;
 dims[1] = (extent[3]-extent[2])+1;
 dims[2] = (extent[5]-extent[4])+1;

 // STL vectors
 vtkstd::vector<int> xList;
 vtkstd::vector<int>::iterator xIter;
 vtkstd::vector< vtkstd::vector<int> > zyBucket(dims[1]*dims[2]);

 for (int idxZ = extent[4]; idxZ <= extent[5]; idxZ++)
   {
   double z = (idxZ*spacing[2])+origin[2];

   slice->PrepareForNewData();

   this->DataSetCutter(input, slice, z, locator);

   if (!slice->GetNumberOfLines())
     {
     continue;
     }

   slice->BuildLinks( 0 );
   lines = slice->GetLines();
   lines->InitTraversal();

   // "lines" is a vtkCellArray, we need to find any lines that have
   // loose ends (i.e. that don't connect to other lines) and create
   // new line segments that tie these loose ends together.

   // Description of algorith:
   // 1) find all "loose ends" with slice->GetPointCells(),
   //    store them in a vtkIdList
   // 2) go through the loose ends, and for each one, find which
   //    other loose end is closest, then remove them both from the
   //    vtkIdList (use brute-force method, search all other points)
   // 3) add a new line segment that connects these ends:
   //    lines->InsertNextCell(2);
   //    lines->InsertCellPoint(point1Id);
   //    lines->InsertCellPoint(point2Id);
   /////////////////////////////////////////////////////////////////////////
   vtkIdType currentPointId, firstLooseEndId, secondLooseEndId, currentLooseEndId ;
   vtkIdList *looseEndIdList, *pointIdList, *cellIdList;
   vtkPoints * points;
   int numberOfPoints;
#if (VTK_MAJOR_VERSION == 4) && (VTK_MINOR_VERSION <= 3)
   float firstLooseEnd[3], secondLooseEnd[3], currentLooseEnd[3];
#else
   double firstLooseEnd[3], secondLooseEnd[3], currentLooseEnd[3];
#endif
   double minimumDistance = 0.0 ;
   double fend1[2], fend2[2];

   points = slice->GetPoints();
   numberOfPoints = points->GetNumberOfPoints();

   cellIdList = vtkIdList::New();
   looseEndIdList =  vtkIdList::New();
   pointIdList    =  vtkIdList::New();

   for ( int i = 0; i < numberOfPoints ; i++ )
     {
     currentPointId = i;
     slice->GetPointCells( currentPointId , cellIdList );
     if( cellIdList->GetNumberOfIds()<2 )
       {
       looseEndIdList->InsertNextId( currentPointId ); // stored the
                                                       // loose end
       }
     }
   while( looseEndIdList->GetNumberOfIds() >= 2 )
     {
     firstLooseEndId = looseEndIdList->GetId(0);
     secondLooseEndId = looseEndIdList->GetId(1);

     slice->GetPoint( firstLooseEndId, firstLooseEnd ); // first one
                                                        // in the list
     slice->GetPoint( secondLooseEndId, secondLooseEnd );//second one
                                                         //in the list
     looseEndIdList->DeleteId( firstLooseEndId );
     minimumDistance = DistanceMeasure(firstLooseEnd, secondLooseEnd);

     for( int j = 2 ; j < looseEndIdList->GetNumberOfIds() ; j++ )
       {
       currentLooseEndId = looseEndIdList->GetId( j );
       slice->GetPoint( looseEndIdList->GetId( j ), currentLooseEnd);
       if ( DistanceMeasure( firstLooseEnd, currentLooseEnd )< minimumDistance )
         {
         minimumDistance = DistanceMeasure( firstLooseEnd, currentLooseEnd );
         secondLooseEndId = currentLooseEndId;
         }
       }
      looseEndIdList->DeleteId( secondLooseEndId );
      lines->InsertNextCell( 2 );
      lines->InsertCellPoint( firstLooseEndId );
      lines->InsertCellPoint( secondLooseEndId );
     }

   /////////////////////////////////////////////////////////////////////////////
   lines->InitTraversal();

   result = lines->GetNextCell(npts, pts);

   while (result)
     {
     ptId1 = pts[0]; ptId2 = pts[1];

     slice->GetPoint(ptId1, point1);

     end1[0] = vtkMath::Round( (point1[0]-origin[0])*invspacing[0] );
     end1[1] = vtkMath::Round( (point1[1]-origin[1])*invspacing[1] );

     fend1[0] =  (point1[0]-origin[0])*invspacing[0] ;
     fend1[1] =  (point1[1]-origin[1])*invspacing[1] ;

     slice->GetPoint(ptId2, point2);

     end2[0] = vtkMath::Round( (point2[0]-origin[0])*invspacing[0] );
     end2[1] = vtkMath::Round( (point2[1]-origin[1])*invspacing[1] );

     fend2[0] =  (point2[0]-origin[0])*invspacing[0] ;
     fend2[1] =  (point2[1]-origin[1])*invspacing[1] ;

     vtkFloatingEndPointLine2D(fend1, fend2, idxZ, dims[1],
                        extent[2], extent[3], zyBucket);

     result = lines->GetNextCell(npts, pts);
     }

   cellIdList->Delete();
   looseEndIdList->Delete();
   pointIdList->Delete();
   }

 int r1, r2, lastr2;
 vtkstd::vector<int> xList2;
 for (int idxZ = extent[4]; idxZ <= extent[5]; idxZ++)
   {
   for (int idxY = extent[2]; idxY <= extent[3]; idxY++)
     {
     xList = zyBucket[idxZ*dims[1]+idxY];

     if (xList.empty())
       {
       continue;
       }

     // handle pairs
     lastr2 = extent[0]-1; // smarter
     if (xList.size() % 2 == 0)
       {
       vtkstd::sort(xList.begin(), xList.end());

       for (xIter = xList.begin(); xIter != xList.end(); xIter++)
         {
         r1 = *xIter;
         xIter++;
         r2 = *xIter;

         // extents are not allowed to overlap
         if (r1 == lastr2)
           {
           r1++;
           // eliminate empty extents
           if (r1 > r2)
             {
             continue;
             }
           }

         data->InsertNextExtent(r1, r2, idxY, idxZ);

         lastr2 = r2;
         }
       }
     }
   }

 slice->Delete();
 locator->Delete();
}

#if (VTK_MAJOR_VERSION > 4) || (VTK_MINOR_VERSION > 4)

//----------------------------------------------------------------------------
int vtkAtamaiPolyDataToImageStencil2::RequestData(
 vtkInformation *request,
 vtkInformationVector **inputVector,
 vtkInformationVector *outputVector)
{
 this->Superclass::RequestData(request, inputVector, outputVector);

 vtkInformation *outInfo = outputVector->GetInformationObject(0);

 vtkImageStencilData *data = vtkImageStencilData::SafeDownCast(
   outInfo->Get(vtkDataObject::DATA_OBJECT()));

 int extent[6];
 data->GetExtent(extent);
 this->ThreadedExecute(data, extent, 0);

 return 1;
}

//----------------------------------------------------------------------------
int vtkAtamaiPolyDataToImageStencil2::RequestInformation(
 vtkInformation *,
 vtkInformationVector **,
 vtkInformationVector *outputVector)
{
 vtkInformation *outInfo = outputVector->GetInformationObject(0);

 // this is an odd source that can produce any requested size.  so its whole
 // extent is essentially infinite. This would not be a great source to
 // connect to some sort of writer or viewer. For a sanity check we will
 // limit the size produced to something reasonable (depending on your
 // definition of reasonable)
 outInfo->Set(vtkStreamingDemandDrivenPipeline::WHOLE_EXTENT(),
              0, VTK_LARGE_INTEGER >> 2,
              0, VTK_LARGE_INTEGER >> 2,
              0, VTK_LARGE_INTEGER >> 2);
 return 1;
}

//----------------------------------------------------------------------------
int vtkAtamaiPolyDataToImageStencil2::FillInputPortInformation(
 int,
 vtkInformation* info)
{
 info->Set(vtkAlgorithm::INPUT_REQUIRED_DATA_TYPE(), "vtkPolyData");
 return 1;
}

#endif  /* (VTK_MAJOR_VERSION > 4) || (VTK_MINOR_VERSION > 4) */
