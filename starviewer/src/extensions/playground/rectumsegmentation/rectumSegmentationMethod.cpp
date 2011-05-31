/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rectumSegmentationMethod.h"

#include <itkImage.h>
#include <itkImageRegionIterator.h>
#include <itkImageRegionIteratorWithIndex.h>
#include <itkBinaryThresholdImageFilter.h>

#include <itkConfidenceConnectedImageFilter.h>
#include <itkCurvatureFlowImageFilter.h>
#include <itkCastImageFilter.h>
#include <itkConnectedThresholdImageFilter.h>
#include <itkMedianImageFilter.h>
#include <itkCurvatureAnisotropicDiffusionImageFilter.h>
#include <itkGradientMagnitudeRecursiveGaussianImageFilter.h>
#include <itkSigmoidImageFilter.h>
#include <itkFastMarchingImageFilter.h>
#include <itkRescaleIntensityImageFilter.h>
#include <itkThresholdSegmentationLevelSetImageFilter.h>
#include <itkZeroCrossingImageFilter.h>
#include <itkGeodesicActiveContourLevelSetImageFilter.h>
#include <itkVolumeCalculatorImageFilter.h>
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>
#include <itkExtractImageFilter.h>
#include <itkDanielssonDistanceMapImageFilter.h>

#include <itkVector.h>
#include <itkListSample.h>
#include <itkMeanCalculator.h>
#include <itkCovarianceCalculator.h>

#include <itkErfcLevelSetImageFilter.h>
#include <itkImageFileWriter.h>

#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>

#include <vtkImageData.h>

#include <QMessageBox>

#include "logging.h"

namespace udg {

rectumSegmentationMethod::rectumSegmentationMethod()
{
    m_Volume = 0;
    m_Mask = 0;
    m_filteredInputImage = 0;

    m_px = 0.0;
    m_py = 0.0;
    m_pz = 0.0;

    m_lowerThreshold = 0;
    m_upperThreshold = 0;

    m_insideMaskValue  = 255;
    m_outsideMaskValue = 0;

    m_minROI[0]=-1;
    m_minROI[1]=-1;
    m_maxROI[0]=-1;
    m_maxROI[1]=-1;

}

rectumSegmentationMethod::~rectumSegmentationMethod()
{
}

void rectumSegmentationMethod::setSeedPosition (double x, double y, double z)
{
    m_px=x;
    m_py=y;
    m_pz=z;
}

void rectumSegmentationMethod::setHistogramLowerLevel (int x)
{
    m_lowerThreshold=x;
}

void rectumSegmentationMethod::setHistogramUpperLevel (int x)
{
    m_upperThreshold=x;
}

double rectumSegmentationMethod::applyMethod()
{
    typedef itk::CastImageFilter< Volume::ItkImageType, IntermediateImageType > InputCastingFilterType;
    typedef itk::CastImageFilter< InternalImageType,  ExternalImageType>    OutputCastingFilterType;
    typedef itk::ExtractImageFilter< IntermediateImageType, InternalImageType > ExtractFilterType;

    typedef itk::ConnectedThresholdImageFilter< InternalImageType, InternalImageType > ConnectedFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    ExtractFilterType::Pointer extracter = ExtractFilterType::New();
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();

    if(m_minROI[0]==-1)
    {
        QMessageBox::warning( 0 , QObject::tr( "Starviewer" ) , QObject::tr( "No definite ROI" ) );
        return -1.0;
    }

    Volume::ItkImageType::IndexType seedIndex;
    Volume::ItkImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);

    //std::cout<<"Seed Index: "<<seedIndex<<endl;
    incaster->SetInput( m_Volume->getItkData() );
    extracter->SetInput( incaster->GetOutput() );
    connectedThreshold->SetInput( extracter->GetOutput() );

    IntermediateImageType::RegionType inputRegion = m_Volume->getItkData()->GetLargestPossibleRegion();
    IntermediateImageType::SizeType size = inputRegion.GetSize();
    size[2] = 0;
    IntermediateImageType::IndexType start = inputRegion.GetIndex();
    unsigned int sliceNumber = seedIndex[2];
    start[2] = sliceNumber;
    IntermediateImageType::RegionType desiredRegion;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    extracter->SetExtractionRegion( desiredRegion );

    connectedThreshold->SetLower(  m_lowerThreshold  );
    connectedThreshold->SetUpper(  m_upperThreshold  );
    connectedThreshold->SetReplaceValue( m_insideMaskValue );

    InternalImageType::IndexType internalSeedIndex;
    internalSeedIndex[0] = seedIndex[0];
    internalSeedIndex[1] = seedIndex[1];
    connectedThreshold->SetSeed( internalSeedIndex );

    /*std::cout<<"Init filter"<<std::endl;
    std::cout<<"Parameters: "<<internalSeedIndex<<std::endl;
    std::cout<<"Slice: "<<sliceNumber<<std::endl;
    std::cout<<"Histogram parameters: "<<m_lowerThreshold<<" "<<m_upperThreshold<<std::endl;
    std::cout<<"mask Values: "<<m_insideMaskValue<<" "<<m_outsideMaskValue<<std::endl;
    std::cout<<"Seed Indexs: "<<internalSeedIndex[0]<<" "<<internalSeedIndex[1]<<std::endl;
	*/

    typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType,2> StructuringElementType;
    typedef itk::BinaryErodeImageFilter<InternalImageType,InternalImageType,StructuringElementType > ErodeFilterType;
    typedef itk::BinaryDilateImageFilter<InternalImageType,InternalImageType,StructuringElementType > DilateFilterType;

    DilateFilterType::Pointer binaryDilatePre = DilateFilterType::New();
    ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();

    binaryDilatePre->SetDilateValue( m_insideMaskValue );
    binaryErode->SetErodeValue( m_insideMaskValue );
    binaryDilate->SetDilateValue( m_insideMaskValue );

    unsigned long radiusDilatePre[2];
    radiusDilatePre[0]=1;
    radiusDilatePre[1]=1;
    StructuringElementType structuringElementDilatePre;
    structuringElementDilatePre.SetRadius( radiusDilatePre );
    structuringElementDilatePre.CreateStructuringElement();

    binaryDilatePre->SetKernel( structuringElementDilatePre );
    binaryDilatePre->SetInput( connectedThreshold->GetOutput() );

    unsigned long radiusErode[2];
    //Màxim budell sa 8mm--> Per confirmar!!!!
    //double rectumwith = 8.0;
    double rectumwidth = m_multiplier;
    //std::cout<<"Rectumwidth (Multiplier): "<<rectumwidth<<std::endl;
    radiusErode[0]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[0]))+radiusDilatePre[0];
    radiusErode[1]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[1]))+radiusDilatePre[1];

    StructuringElementType structuringElementErode;
    structuringElementErode.SetRadius( radiusErode );
    structuringElementErode.CreateStructuringElement();

    binaryErode->SetKernel( structuringElementErode );
    binaryErode->SetInput( binaryDilatePre->GetOutput() );

    unsigned long radiusDilate[2];
    radiusDilate[0]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[0]));
    radiusDilate[1]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[1]));
    StructuringElementType structuringElementDilate;
    structuringElementDilate.SetRadius( radiusDilate );
    structuringElementDilate.CreateStructuringElement();

    binaryDilate->SetKernel( structuringElementDilate );
    binaryDilate->SetInput( binaryErode->GetOutput() );

    //std::cout<<"Morfo Operators: "<<radiusDilatePre[0]<<" "<<radiusDilatePre[1]<<" // "<<radiusErode[0]<<" "<<radiusErode[1]<<" // "<<radiusDilate[0]<<" "<<radiusDilate[1]<<std::endl;

    ConnectedFilterType::Pointer connectedThreshold2 = ConnectedFilterType::New();
    connectedThreshold2->SetLower( m_insideMaskValue - 2 );
    connectedThreshold2->SetUpper( m_insideMaskValue + 2 );
    connectedThreshold2->SetReplaceValue( m_insideMaskValue );
    connectedThreshold2->SetSeed( internalSeedIndex );

   try
    {
        binaryDilate->Update();
        connectedThreshold2->SetInput( binaryDilate->GetOutput() );
        connectedThreshold2->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

//         OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
//         typedef itk::ImageFileWriter< ExternalImageType > ExternalWriterType;
//
//         ExternalWriterType::Pointer mapWriter = ExternalWriterType::New();
//         outcaster->SetInput( connectedThreshold2->GetOutput() );
//         mapWriter->SetInput( outcaster->GetOutput() );
//         mapWriter->SetFileName("segmented.jpg");
//         mapWriter->Update();

    typedef itk::BinaryThresholdImageFilter< InternalImageType, InternalImageType > BinThresholdFilterType;
    typedef itk::DanielssonDistanceMapImageFilter<InternalImageType, InternalImageType >  DistanceFilterType;
    BinThresholdFilterType::Pointer binthresholdFilter = BinThresholdFilterType::New();
    DistanceFilterType::Pointer distanceFilter = DistanceFilterType::New();

    //binthresholdFilter->SetInput( binaryDilate->GetOutput() );
    binthresholdFilter->SetInput( connectedThreshold2->GetOutput() );
    binthresholdFilter->SetOutsideValue( m_insideMaskValue );
    binthresholdFilter->SetInsideValue(  m_outsideMaskValue  );
    binthresholdFilter->SetLowerThreshold( m_insideMaskValue - 2 );
    binthresholdFilter->SetUpperThreshold( m_insideMaskValue + 2 );
    distanceFilter->SetInput( binthresholdFilter->GetOutput() );
    //std::cout<<"Inside: "<<m_insideMaskValue<<", outside: "<<m_outsideMaskValue<<std::endl;
    distanceFilter->Update();

        /*OutputCastingFilterType::Pointer outcaster2 = OutputCastingFilterType::New();
        ExternalWriterType::Pointer mapWriter2 = ExternalWriterType::New();
        outcaster2->SetInput( distanceFilter->GetOutput() );
        //outcaster2->SetInput( binthresholdFilter->GetOutput() );
        mapWriter2->SetInput( outcaster2->GetOutput() );
        mapWriter2->SetFileName("distanceMap.jpg");
        mapWriter2->Update();

        OutputCastingFilterType::Pointer outcaster3 = OutputCastingFilterType::New();
        ExternalWriterType::Pointer mapWriter3 = ExternalWriterType::New();
        //outcaster2->SetInput( distanceFilter->GetOutput() );
        outcaster3->SetInput( binaryDilate->GetOutput() );
        mapWriter3->SetInput( outcaster2->GetOutput() );
        mapWriter3->SetFileName("distanceMap2.jpg");
        mapWriter3->Update();*/

    itk::ImageRegionIteratorWithIndex< InternalImageType > itDist( distanceFilter->GetOutput(), distanceFilter->GetOutput()->GetLargestPossibleRegion() );
    itDist.GoToBegin();
    int value,max,min, xmax, ymax;
    max=itDist.Get();
    min=max;
    while(!itDist.IsAtEnd())
    {
        value=itDist.Get();
        if(value<min)
        {
            min=value;
        }
        if(value>max){
            max=value;
            xmax=itDist.GetIndex()[0];
            ymax=itDist.GetIndex()[1];
        }
        ++itDist;
    }
    //std::cout<<"Max="<<max<<", min="<<min<<std::endl;
    //std::cout<<"PosMax= ["<<xmax<<", "<<ymax<<"]"<<std::endl;


    Volume::ItkImageType::Pointer maskAux = Volume::ItkImageType::New();
    maskAux->SetRegions( m_Volume->getItkData()->GetBufferedRegion() );
    maskAux->SetSpacing( m_Volume->getItkData()->GetSpacing() );
    maskAux->SetOrigin( m_Volume->getItkData()->GetOrigin() );
    maskAux->Allocate();

    itk::ImageRegionIteratorWithIndex< Volume::ItkImageType > itMask( maskAux, maskAux->GetLargestPossibleRegion() );
    //Inicialment posem tota la mascara com outside
    itMask.GoToBegin();
    int cont3=0;
    while(!itMask.IsAtEnd())
    {
        itMask.Set(m_outsideMaskValue);
        ++itMask;
        cont3++;
    }
    //Ara copiem a la llesca que toca els valors de la mascara
    start[0]=0;
    start[1]=0;
    start[2]=sliceNumber;
    itMask.SetIndex(start);
    //itk::ImageRegionIterator< InternalImageType > itSeg( connectedThreshold->GetOutput(), connectedThreshold->GetOutput()->GetLargestPossibleRegion() );
    //itk::ImageRegionIterator< InternalImageType > itSeg( binaryDilate->GetOutput(), binaryDilate->GetOutput()->GetLargestPossibleRegion() );
    itk::ImageRegionIterator< InternalImageType > itSeg( connectedThreshold2->GetOutput(), connectedThreshold2->GetOutput()->GetLargestPossibleRegion() );
    itSeg.GoToBegin();
    m_cont=0;
    int cont2=0;
    IntermediateImageType::SizeType sizeOut = inputRegion.GetSize();
    for (int j = 0; j < static_cast<int>(sizeOut[1]); ++j)
    {
        for (int i = 0; i < static_cast<int>(sizeOut[0]); ++i)
        {
            if (itSeg.Get() == m_insideMaskValue && i >= m_minROI[0] && i <= m_maxROI[0] && j >= m_minROI[1] && j <= m_maxROI[1])
			{
                itMask.Set(m_insideMaskValue);
                ++m_cont;
            }
			else
			{
                itMask.Set(m_outsideMaskValue);
                ++cont2;
            }
            ++itSeg;
            ++itMask;
        }
    }
    Volume::ItkImageType::SpacingType sp = m_Volume->getItkData()->GetSpacing();
    m_volume = m_cont*sp[0]*sp[1]*sp[2];

    //std::cout<<"Volume: "<<m_volume<<" ("<<m_cont<<", "<<cont2<<", "<<cont3<<" voxels)"<<std::endl;
    //std::cout<<"Size: "<<sizeOut<<std::endl;

    m_Mask->setData( maskAux );
    m_Mask->getVtkData()->Update();

    if(m_cont!=0 && (sliceNumber>0)){
        this->applyMethodNextSlice(sliceNumber-1, -1);
    }
    if(m_cont!=0 && (sliceNumber < sizeOut[2]-1)){
        this->applyMethodNextSlice(sliceNumber+1, 1);
    }
    return m_volume;
}

void rectumSegmentationMethod::applyMethodNextSlice( unsigned int slice, int step )
{
    typedef itk::CastImageFilter< Volume::ItkImageType, IntermediateImageType > InputCastingFilterType;
    typedef itk::CastImageFilter< InternalImageType,  ExternalImageType>    OutputCastingFilterType;
    typedef itk::ExtractImageFilter< IntermediateImageType, InternalImageType > ExtractFilterType;

    typedef itk::ConnectedThresholdImageFilter< InternalImageType, InternalImageType > ConnectedFilterType;
    typedef itk::BinaryThresholdImageFilter< InternalImageType, InternalImageType > ThresholdFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    ExtractFilterType::Pointer extracter = ExtractFilterType::New();
    ThresholdFilterType::Pointer ThresholdFilter = ThresholdFilterType::New();

    incaster->SetInput( m_Volume->getItkData() );
    extracter->SetInput( incaster->GetOutput() );
    ThresholdFilter->SetInput( extracter->GetOutput() );

    IntermediateImageType::RegionType inputRegion = m_Volume->getItkData()->GetLargestPossibleRegion();
    IntermediateImageType::SizeType size = inputRegion.GetSize();
    size[2] = 0;
    IntermediateImageType::IndexType start = inputRegion.GetIndex();
    start[2] = slice;
    IntermediateImageType::RegionType desiredRegion;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    extracter->SetExtractionRegion( desiredRegion );

    ThresholdFilter->SetLowerThreshold (  m_lowerThreshold  );
    ThresholdFilter->SetUpperThreshold (  m_upperThreshold  );
    ThresholdFilter->SetOutsideValue( m_outsideMaskValue );
    ThresholdFilter->SetInsideValue(  m_insideMaskValue );

    /*std::cout<<"Init filter"<<std::endl;
    std::cout<<"Slice: "<<slice<<std::endl;
    std::cout<<"Histogram parameters: "<<m_lowerThreshold<<" "<<m_upperThreshold<<std::endl;
    std::cout<<"mask Values: "<<m_insideMaskValue<<" "<<m_outsideMaskValue<<std::endl;
	*/
	
    typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType,2> StructuringElementType;
    typedef itk::BinaryErodeImageFilter<InternalImageType,InternalImageType,StructuringElementType > ErodeFilterType;
    typedef itk::BinaryDilateImageFilter<InternalImageType,InternalImageType,StructuringElementType > DilateFilterType;

    DilateFilterType::Pointer binaryDilatePre = DilateFilterType::New();
    ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();

    binaryDilatePre->SetDilateValue( m_insideMaskValue );
    binaryErode->SetErodeValue( m_insideMaskValue );
    binaryDilate->SetDilateValue( m_insideMaskValue );

    unsigned long radiusDilatePre[2];
    radiusDilatePre[0]=1;
    radiusDilatePre[1]=1;
    StructuringElementType structuringElementDilatePre;
    structuringElementDilatePre.SetRadius( radiusDilatePre );
    structuringElementDilatePre.CreateStructuringElement();

    binaryDilatePre->SetKernel( structuringElementDilatePre );
    binaryDilatePre->SetInput( ThresholdFilter->GetOutput() );

    unsigned long radiusErode[2];
    //Màxim budell sa 8mm--> Per confirmar!!!!
    //double rectumwidth = 8.0;
    double rectumwidth = m_multiplier;
    //std::cout<<"Rectumwidth (Multiplier): "<<rectumwidth<<std::endl;
    radiusErode[0]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[0]))+radiusDilatePre[0];
    radiusErode[1]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[1]))+radiusDilatePre[1];

    StructuringElementType structuringElementErode;
    structuringElementErode.SetRadius( radiusErode );
    structuringElementErode.CreateStructuringElement();

    binaryErode->SetKernel( structuringElementErode );
    binaryErode->SetInput( binaryDilatePre->GetOutput() );

    unsigned long radiusDilate[2];
    radiusDilate[0]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[0]));
    radiusDilate[1]=(unsigned long) (rectumwidth/(2*m_Volume->getItkData()->GetSpacing()[1]));
    StructuringElementType structuringElementDilate;
    structuringElementDilate.SetRadius( radiusDilate );
    structuringElementDilate.CreateStructuringElement();

    binaryDilate->SetKernel( structuringElementDilate );
    binaryDilate->SetInput( binaryErode->GetOutput() );

    //std::cout<<"Morfo Operators: "<<radiusDilatePre[0]<<" "<<radiusDilatePre[1]<<" // "<<radiusErode[0]<<" "<<radiusErode[1]<<" // "<<radiusDilate[0]<<" "<<radiusDilate[1]<<std::endl;

   try
    {
        binaryDilate->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    //std::cout<<"Init Region Growing From Region..."<<std::endl;
    ExtractFilterType::Pointer extracterPrevious = ExtractFilterType::New();
    extracterPrevious->SetInput( m_Mask->getItkData() );
    start[2] = slice - step;
    desiredRegion.SetSize(  size  );
    desiredRegion.SetIndex( start );
    extracterPrevious->SetExtractionRegion( desiredRegion );
    extracterPrevious->Update();

    InternalImageType::Pointer regionThreshold = InternalImageType::New();
    regionThreshold->SetRegions( ThresholdFilter->GetOutput()->GetBufferedRegion() );
    regionThreshold->SetSpacing( ThresholdFilter->GetOutput()->GetSpacing() );
    regionThreshold->SetOrigin( ThresholdFilter->GetOutput()->GetOrigin() );
    regionThreshold->Allocate();

    itk::ImageRegionIteratorWithIndex< InternalImageType > itDilate( binaryDilate->GetOutput(), binaryDilate->GetOutput()->GetLargestPossibleRegion() );
    itk::ImageRegionIteratorWithIndex< InternalImageType > itPrevious( extracterPrevious->GetOutput(), extracterPrevious->GetOutput()->GetLargestPossibleRegion() );
    itk::ImageRegionIteratorWithIndex< InternalImageType > itRegion( regionThreshold, regionThreshold->GetLargestPossibleRegion() );

    itRegion.GoToBegin();
    //std::cout<<"Init Region..."<<std::endl;
    while(!itRegion.IsAtEnd())
    {
        itRegion.Set(m_outsideMaskValue);
        ++itRegion;
    }

    itDilate.GoToBegin();
    itPrevious.GoToBegin();
    itRegion.GoToBegin();
    //std::cout<<"Init Recorregut..."<<std::endl;
    while(!itDilate.IsAtEnd())
    {
        if((itDilate.Get()==m_insideMaskValue)&&(itPrevious.Get()==m_insideMaskValue)&&(itRegion.Get()!=m_insideMaskValue))
        {
            //std::cout<<"Crida a regionGrowingRecursive a "<<itDilate.GetIndex()[0]<<", "<<itDilate.GetIndex()[1]<<" [ "<<regionThreshold->GetLargestPossibleRegion().GetSize()<<std::endl;
			//Primer ho desem en atributs de la classe així ja no s'ha de passar per paràmetre
			m_maskrecursive = regionThreshold;
			m_imrecursive = binaryDilate->GetOutput();
            this->regionGrowingRecursive( itDilate.GetIndex()[0], itDilate.GetIndex()[1],0);
        }
        ++itDilate;
        ++itPrevious;
        ++itRegion;
    }


    itk::ImageRegionIterator< Volume::ItkImageType > itMask( m_Mask->getItkData(), m_Mask->getItkData()->GetLargestPossibleRegion() );
    itMask.GoToBegin();
    itk::ImageRegionIterator< InternalImageType > itSeg( regionThreshold, regionThreshold->GetLargestPossibleRegion() );
    itSeg.GoToBegin();
    unsigned int i,j;
    int contant=m_cont;
    IntermediateImageType::SizeType sizeOut = inputRegion.GetSize();
    //std::cout<<sizeOut<<std::endl;
    itk::ImageRegionIterator< Volume::ItkImageType >::IndexType indexMask;
    indexMask[0]=0;
    indexMask[1]=0;
    indexMask[2]=slice;
    itMask.SetIndex(indexMask);
    //std::cout<<"Init Recorregut2... i="<<sizeOut[0]<<",j="<<sizeOut[1]<<std::endl;
    for(j=0;j<sizeOut[1];j++){
        for(i=0;i<sizeOut[0];i++){
            if(itSeg.Get()==m_insideMaskValue)
			{
                itMask.Set(m_insideMaskValue);
                m_cont++;
            }
            ++itSeg;
            ++itMask;
        }
    }

    Volume::ItkImageType::SpacingType sp = m_Volume->getItkData()->GetSpacing();
    m_volume = m_cont*sp[0]*sp[1]*sp[2];

    //std::cout<<"Volume: "<<m_volume<<" ("<<m_cont<<" voxels), ( increment de "<<m_cont-contant<<" voxels)"<<std::endl;

    m_Mask->getVtkData()->Update();
    //std::cout<<"End for step "<<step<<" in slice "<<slice<<std::endl;
    unsigned int sliceWithStep = slice + step;
    if (sliceWithStep < m_Mask->getItkData()->GetLargestPossibleRegion().GetSize()[2] && contant != m_cont)
    {
        this->applyMethodNextSlice(sliceWithStep, step);
    }

    return;
}

void rectumSegmentationMethod::regionGrowingRecursive(int indexX, int indexY , int prof)
{
    itk::ImageRegionIteratorWithIndex< InternalImageType > itIm( m_imrecursive, m_imrecursive->GetLargestPossibleRegion());
    itk::ImageRegionIteratorWithIndex< InternalImageType > itMask( m_maskrecursive, m_maskrecursive->GetLargestPossibleRegion());
    InternalImageType::IndexType index;
    index[0]=indexX;
    index[1]=indexY;

	itIm.SetIndex(index);
    itMask.SetIndex(index);
    //if((itIm.Get()>=m_lowerThreshold)&&(itIm.Get()<=m_upperThreshold)&&(itMask.Get()!=m_insideMaskValue))
    //if((itIm.Get()==m_insideMaskValue)&&(itMask.Get()!=m_insideMaskValue))
    if((itIm.Get()==m_insideMaskValue)&&(itMask.Get()!=m_insideMaskValue)&& (indexX>=m_minROI[0])&&(indexX<=m_maxROI[0])&&(indexY>=m_minROI[1])&&(indexY<=m_maxROI[1])&&prof<=2000)
    {
        //std::cout<<"->Set "<<std::endl;
        itMask.Set(m_insideMaskValue);
        if(indexX+1 < (int)m_maskrecursive->GetLargestPossibleRegion().GetSize()[0])
            regionGrowingRecursive( indexX + 1, indexY, prof +1);
        if(indexX > 0)
            regionGrowingRecursive( indexX - 1, indexY, prof +1 );
        if(indexY+1 < (int)m_maskrecursive->GetLargestPossibleRegion().GetSize()[1])
            regionGrowingRecursive( indexX, indexY + 1, prof +1 );
        if(indexY > 0)
            regionGrowingRecursive( indexX, indexY - 1, prof +1 );
    }
    else
    {
      //  std::cout<<"*"<<std::endl;
    }
}

void rectumSegmentationMethod::applyFilter(Volume* output)
{
    typedef   float           InternalPixelType;
    typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

    typedef itk::CastImageFilter< Volume::ItkImageType, InternalImageType >                     InputCastingFilterType;
    typedef itk::CastImageFilter< InternalImageType, Volume::ItkImageType >                      OutputCastingFilterType;

    typedef itk::CurvatureFlowImageFilter< InternalImageType, InternalImageType >      CurvatureFlowImageFilterType;
    typedef itk::ConnectedThresholdImageFilter< InternalImageType, InternalImageType > ConnectedFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();


    incaster->SetInput( m_Volume->getItkData() );
    smoothing->SetInput( incaster->GetOutput() );
    outcaster->SetInput( smoothing->GetOutput() );

    smoothing->SetNumberOfIterations( 5 );
    smoothing->SetTimeStep( 0.0625 );
    //smoothing->SetNumberOfIterations( numiterations );
    //smoothing->SetTimeStep( timeStep );



    DEBUG_LOG("Init filter");

    try
    {
        outcaster->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    output->setImages(m_Volume->getImages());

    output->setData( outcaster->GetOutput());
    output->getVtkData()->Update();

    return;
}

}
