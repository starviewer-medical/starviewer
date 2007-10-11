#ifndef DETERMINATECONTOUR_H
#define DETERMINATECONTOUR_H

#include "itkImage.h"
#include <iostream>
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

#include "itkGradientMagnitudeImageFilter.h"//contour

using namespace itk;

namespace udg {

typedef struct contorn{
	int x[200000];
	int y[200000];
	int z[200000];
	int npslice[20000][3];
	int np;
	float xt,yt;
};
class DeterminateContour{

private: //posarem la part privada avans de la publica
  float           pTotals;
  float           pSegmen;
  float           *pSegSli;
  int             maxSlices;
  int             maxX,maxY;
  
  int quanteslices;
  int npuntstot;
  //contorn         *c;
  //bool		  matriu[100][100][100];
 

  public:
  contorn         *c;
  typedef signed int PixelType; 
  typedef itk::Image< PixelType, 2 >  ImageType2D;
  typedef itk::Image< PixelType, 3 >  ImageType3D;
  
  typedef itk::ImageLinearIteratorWithIndex< ImageType2D > LinearIteratorType;
  typedef itk::ImageSliceConstIteratorWithIndex< ImageType3D >  SliceIteratorType;

  
  typedef itk::ImageFileReader< ImageType3D > ReaderType;
  typedef itk::ImageFileWriter< ImageType2D > WriterType;

  ImageType3D::Pointer inputImage;
  
  unsigned int projectionDirection;
  unsigned int i, j;
  unsigned int direction[2];
  //int ***matriu;
  int matriu[300][300][300];
  
  ImageType2D::RegionType region;
  ImageType2D::RegionType::SizeType size;
  ImageType2D::RegionType::IndexType index;
  
  ImageType3D::RegionType requestedRegion;
  
  SliceIteratorType  inputIt;//nose si anirà
  
  typedef itk::Image<signed int,3>     ImageType;
  typedef ImageType::Pointer      ImagePointer;
  
  typedef ImageType::RegionType   RegionType;
  typedef ImageType::SizeType     SizeType;
  
 // typedef itk::GradientMagnitudeImageFilter< ImageType, ImageType >  FilterType;
   
  //FilterType::Pointer filter;
  friend class QtVtkImage;
  friend class vtkIPWCallback;
  
  DeterminateContour(ImageType* rea, int proj);
  ~DeterminateContour();
  
 //void ApplySave( char* wri );  
  float GetPixelT();
  float GetPixelS();
  float GetPixelSSli(int ind);
  
  void ferreseguit(int x, int y, int slice);
  

 


  };

}

#endif
