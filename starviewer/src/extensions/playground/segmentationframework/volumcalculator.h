/**Recorrerà tot el model de voxels i calcularà el volum*/
#ifndef VOLUMCALCULATOR_H
#define VOLUMCALCULATOR_H

#include "itkImage.h"
#include <iostream>
#include "itkImageSliceConstIteratorWithIndex.h"
#include "itkImageLinearIteratorWithIndex.h"
#include "itkImageFileReader.h"
#include "itkImageFileWriter.h"

using namespace itk;

namespace udg {

class VolumCalculator{

    private: 
    float           pTotals;
    float           pSegmen;
    float           pSegSli[200];
    int             maxSlices;
    
    
    public:
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
    
    ImageType2D::RegionType region;
    ImageType2D::RegionType::SizeType size;
    ImageType2D::RegionType::IndexType index;
    
    ImageType3D::RegionType requestedRegion;
    
    SliceIteratorType  inputIt;//nose si anirà
    
    typedef itk::Image<signed int,3>     ImageType;
    typedef ImageType::Pointer      ImagePointer;
    
    ///Constructor
    VolumCalculator(ImageType* rea, int proj);
    ///Destructor
    ~VolumCalculator();
    
    ///Retorna el volum total del model
    float GetPixelT();
    ///Retorna el volum segmentat del model
    float GetPixelS();
    ///Retorna l'àrea de la llesca (ind).
    float GetPixelSSli(int ind);

 


  };
};
#endif
