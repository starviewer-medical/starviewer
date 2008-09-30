/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "rectumSegmentationMethod.h"

#include "itkImage.h"
#include "itkImageRegionIterator.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkBinaryThresholdImageFilter.h"

#include "itkConfidenceConnectedImageFilter.h"
#include "itkCurvatureFlowImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkConnectedThresholdImageFilter.h"
#include "itkMedianImageFilter.h"
#include "itkCurvatureAnisotropicDiffusionImageFilter.h"
#include "itkGradientMagnitudeRecursiveGaussianImageFilter.h"
#include "itkSigmoidImageFilter.h"
#include "itkFastMarchingImageFilter.h"
#include "itkRescaleIntensityImageFilter.h"
#include "itkThresholdSegmentationLevelSetImageFilter.h"
#include "itkZeroCrossingImageFilter.h"
#include "itkGeodesicActiveContourLevelSetImageFilter.h"
#include "itkVolumeCalculatorImageFilter.h"
#include "itkBinaryErodeImageFilter.h"
#include "itkBinaryDilateImageFilter.h"
#include "itkBinaryBallStructuringElement.h"
#include "itkExtractImageFilter.h"
#include "itkDanielssonDistanceMapImageFilter.h"

#include "itkVector.h"
#include "itkListSample.h"
#include "itkMeanCalculator.h"
#include "itkCovarianceCalculator.h"

#include "itkErfcLevelSetImageFilter.h"
#include "itkImageFileWriter.h"

#include "itkResampleImageFilter.h"
#include "itkAffineTransform.h"
#include "itkNearestNeighborInterpolateImageFunction.h"

#include <QMessageBox>


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

    std::cout<<"Seed Index: "<<seedIndex<<endl;
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

    std::cout<<"Init filter"<<std::endl;
    std::cout<<"Parameters: "<<internalSeedIndex<<std::endl;
    std::cout<<"Slice: "<<sliceNumber<<std::endl;
    std::cout<<"Histogram parameters: "<<m_lowerThreshold<<" "<<m_upperThreshold<<std::endl;
    std::cout<<"mask Values: "<<m_insideMaskValue<<" "<<m_outsideMaskValue<<std::endl;
    std::cout<<"Seed Indexs: "<<internalSeedIndex[0]<<" "<<internalSeedIndex[1]<<std::endl;

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
    std::cout<<"Rectumwidth (Multiplier): "<<rectumwidth<<std::endl;
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

    std::cout<<"Morfo Operators: "<<radiusDilatePre[0]<<" "<<radiusDilatePre[1]<<" // "<<radiusErode[0]<<" "<<radiusErode[1]<<" // "<<radiusDilate[0]<<" "<<radiusDilate[1]<<std::endl;

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
    std::cout<<"Inside: "<<m_insideMaskValue<<", outside: "<<m_outsideMaskValue<<std::endl;
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
    std::cout<<"Max="<<max<<", min="<<min<<std::endl;
    std::cout<<"PosMax= ["<<xmax<<", "<<ymax<<"]"<<std::endl;


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
    unsigned int i,j;
    m_cont=0;
    int cont2=0;
    IntermediateImageType::SizeType sizeOut = inputRegion.GetSize();
    for(j=0;j<sizeOut[1];j++){
        for(i=0;i<sizeOut[0];i++){
            if(itSeg.Get()==m_insideMaskValue){
                itMask.Set(m_insideMaskValue);
                m_cont++;
            }else{
                itMask.Set(m_outsideMaskValue);
                cont2++;
            }
            ++itSeg;
            ++itMask;
        }
    }
    Volume::ItkImageType::SpacingType sp = m_Volume->getItkData()->GetSpacing();
    m_volume = m_cont*sp[0]*sp[1]*sp[2];

    std::cout<<"Volume: "<<m_volume<<" ("<<m_cont<<", "<<cont2<<", "<<cont3<<" voxels)"<<std::endl;
    std::cout<<"Size: "<<sizeOut<<std::endl;

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

    std::cout<<"Init filter"<<std::endl;
    std::cout<<"Slice: "<<slice<<std::endl;
    std::cout<<"Histogram parameters: "<<m_lowerThreshold<<" "<<m_upperThreshold<<std::endl;
    std::cout<<"mask Values: "<<m_insideMaskValue<<" "<<m_outsideMaskValue<<std::endl;

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
    std::cout<<"Rectumwidth (Multiplier): "<<rectumwidth<<std::endl;
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

    std::cout<<"Morfo Operators: "<<radiusDilatePre[0]<<" "<<radiusDilatePre[1]<<" // "<<radiusErode[0]<<" "<<radiusErode[1]<<" // "<<radiusDilate[0]<<" "<<radiusDilate[1]<<std::endl;

   try
    {
        binaryDilate->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    std::cout<<"Init Region Growing From Region..."<<std::endl;
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
    std::cout<<"Init Region..."<<std::endl;
    while(!itRegion.IsAtEnd())
    {
        itRegion.Set(m_outsideMaskValue);
        ++itRegion;
    }

    itDilate.GoToBegin();
    itPrevious.GoToBegin();
    itRegion.GoToBegin();
    std::cout<<"Init Recorregut..."<<std::endl;
    while(!itDilate.IsAtEnd())
    {
        if((itDilate.Get()==m_insideMaskValue)&&(itPrevious.Get()==m_insideMaskValue)&&(itRegion.Get()!=m_insideMaskValue))
        {
            //std::cout<<"Crida a regionGrowingRecursive a "<<itDilate.GetIndex()[0]<<", "<<itDilate.GetIndex()[1]<<" [ "<<regionThreshold->GetLargestPossibleRegion().GetSize()<<std::endl;
            this->regionGrowingRecursive(regionThreshold, binaryDilate->GetOutput(), itDilate.GetIndex()[0], itDilate.GetIndex()[1]);
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
    std::cout<<sizeOut<<std::endl;
    itk::ImageRegionIterator< Volume::ItkImageType >::IndexType indexMask;
    indexMask[0]=0;
    indexMask[1]=0;
    indexMask[2]=slice;
    itMask.SetIndex(indexMask);
    for(j=0;j<sizeOut[1];j++){
        for(i=0;i<sizeOut[0];i++){
            if(itSeg.Get()==m_insideMaskValue){
                itMask.Set(m_insideMaskValue);
                m_cont++;
            }
            ++itSeg;
            ++itMask;
        }
    }

    Volume::ItkImageType::SpacingType sp = m_Volume->getItkData()->GetSpacing();
    m_volume = m_cont*sp[0]*sp[1]*sp[2];

    std::cout<<"Volume: "<<m_volume<<" ("<<m_cont<<" voxels), ( increment de "<<m_cont-contant<<" voxels)"<<std::endl;

    m_Mask->getVtkData()->Update();
    std::cout<<"End for step "<<step<<" in slice "<<slice<<std::endl;
    if((slice + step)>0 && (slice + step) < m_Mask->getItkData()->GetLargestPossibleRegion().GetSize()[2] && contant != m_cont)
    {
        this->applyMethodNextSlice(slice + step, step);
    }

    return;
}

void rectumSegmentationMethod::regionGrowingRecursive( InternalImageType::Pointer mask, InternalImageType::Pointer im, int indexX, int indexY )
{
    itk::ImageRegionIteratorWithIndex< InternalImageType > itIm( im, im->GetLargestPossibleRegion());
    itk::ImageRegionIteratorWithIndex< InternalImageType > itMask( mask, mask->GetLargestPossibleRegion());
    InternalImageType::IndexType index;
    index[0]=indexX;
    index[1]=indexY;
    //std::cout<<index[0]<<" "<<index[1]<<" ["<<mask->GetLargestPossibleRegion().GetSize()[0]<<" "<<mask->GetLargestPossibleRegion().GetSize()[1];
    itIm.SetIndex(index);
    itMask.SetIndex(index);
    //if((itIm.Get()>=m_lowerThreshold)&&(itIm.Get()<=m_upperThreshold)&&(itMask.Get()!=m_insideMaskValue))
    //if((itIm.Get()==m_insideMaskValue)&&(itMask.Get()!=m_insideMaskValue))
    if((itIm.Get()==m_insideMaskValue)&&(itMask.Get()!=m_insideMaskValue)&& (indexX>=m_minROI[0])&&(indexX<=m_maxROI[0])&&(indexY>=m_minROI[1])&&(indexY<=m_maxROI[1]))
    {
        //std::cout<<"->Set "<<std::endl;
        itMask.Set(m_insideMaskValue);
        if(indexX+1 < (int)mask->GetLargestPossibleRegion().GetSize()[0])
            regionGrowingRecursive( mask, im, indexX + 1, indexY );
        if(indexX > 0)
            regionGrowingRecursive( mask, im, indexX - 1, indexY );
        if(indexY+1 < (int)mask->GetLargestPossibleRegion().GetSize()[1])
            regionGrowingRecursive( mask, im, indexX, indexY + 1 );
        if(indexY > 0)
            regionGrowingRecursive( mask, im, indexX, indexY - 1 );
    }
    else
    {
      //  std::cout<<"*"<<std::endl;
    }
}


double rectumSegmentationMethod::applyCleanSkullMethod()
{
    std::cout<<"Clean Skull!!"<<std::endl;
  typedef itk::BinaryThresholdImageFilter<Volume::ItkImageType, Volume::ItkImageType >  ThresholdFilterType;
  typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType,Volume::VDimension> StructuringElementType;
  typedef itk::BinaryErodeImageFilter<Volume::ItkImageType,Volume::ItkImageType,StructuringElementType > ErodeFilterType;
  typedef itk::BinaryDilateImageFilter<Volume::ItkImageType,Volume::ItkImageType,StructuringElementType > DilateFilterType;

  ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
  DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
  DilateFilterType::Pointer binaryDilatePre = DilateFilterType::New();

  binaryErode->SetErodeValue( m_insideMaskValue );
  binaryDilate->SetDilateValue( m_insideMaskValue );
  binaryDilatePre->SetDilateValue( m_insideMaskValue );

  unsigned long radiusDilatePre[3];
  radiusDilatePre[0]=2;
  radiusDilatePre[1]=2;
  radiusDilatePre[2]=1;
  StructuringElementType structuringElementDilatePre;
  structuringElementDilatePre.SetRadius( radiusDilatePre ); // 3x3 structuring element
  structuringElementDilatePre.CreateStructuringElement();

  binaryDilatePre->SetKernel( structuringElementDilatePre );

  binaryDilatePre->SetInput( m_Mask->getItkData() );

  unsigned long radiusErode[3];
  radiusErode[0]=5;
  radiusErode[1]=5;
  radiusErode[2]=2;

  StructuringElementType structuringElementErode;
  structuringElementErode.SetRadius( radiusErode ); // 3x3 structuring element
  structuringElementErode.CreateStructuringElement();

  binaryErode->SetKernel( structuringElementErode );

  //binaryErode->SetInput( m_Mask->getItkData() );
  binaryErode->SetInput( binaryDilatePre->GetOutput() );

  unsigned long radiusDilate[3];
  radiusDilate[0]=9;
  radiusDilate[1]=9;
  radiusDilate[2]=2;
  StructuringElementType structuringElementDilate;
  structuringElementDilate.SetRadius( radiusDilate ); // 3x3 structuring element
  structuringElementDilate.CreateStructuringElement();

  binaryDilate->SetKernel( structuringElementDilate );

  binaryDilate->SetInput( binaryErode->GetOutput() );

  binaryDilate->Update();

    Volume::ItkImageType::Pointer maskAux = Volume::ItkImageType::New();
    maskAux->SetRegions( m_Mask->getItkData()->GetBufferedRegion() );
    maskAux->SetSpacing( m_Mask->getItkData()->GetSpacing() );
    maskAux->SetOrigin( m_Mask->getItkData()->GetOrigin() );
    maskAux->Allocate();

  maskAux = binaryDilate->GetOutput();

    //std::cout<<"End dilate erode!!"<<std::endl;

    itk::ImageRegionIterator<Volume::ItkImageType> maskIt( m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> auxIt( maskAux, maskAux->GetBufferedRegion());
    auxIt.GoToBegin();
    maskIt.GoToBegin();

    //Fem la intersecció de les dues màscares
    while(!auxIt.IsAtEnd())
    {
        if(maskIt.Value()!= m_insideMaskValue && auxIt.Value() == m_insideMaskValue )
        {
            auxIt.Set(m_outsideMaskValue); //l'únic cas que canvia
        }
        ++auxIt;
        ++maskIt;
    }

    //std::cout<<"End intersection!!"<<std::endl;

    typedef itk::ConnectedThresholdImageFilter< Volume::ItkImageType, Volume::ItkImageType > ConnectedFilterType;

    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();

    connectedThreshold->SetInput( maskAux );//Comentem aquesta l�ia per fer el filtratge

    connectedThreshold->SetLower(  m_insideMaskValue - 20  );
    connectedThreshold->SetUpper(  m_insideMaskValue + 20  );
    connectedThreshold->SetReplaceValue( m_insideMaskValue );

    Volume::ItkImageType::IndexType seedIndex;
    Volume::ItkImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
    connectedThreshold->SetSeed( seedIndex );

    //std::cout<<"Init filter"<<std::endl;
    //std::cout<<"Parameters: "<<seedIndex<<std::endl;
    //std::cout<<"Histogram parameters: "<<m_insideMaskValue - 2<<" "<<m_insideMaskValue + 2<<std::endl;

    typedef itk::VolumeCalculatorImageFilter< Volume::ItkImageType > VolumeCalcFilterType;
    VolumeCalcFilterType::Pointer volumeCalc= VolumeCalcFilterType::New();
    volumeCalc->SetInput(connectedThreshold->GetOutput());
    volumeCalc->SetInsideValue( m_insideMaskValue );

    try
    {
        //outcaster->Update();
        volumeCalc->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    m_volume = volumeCalc->GetVolume();
    m_cont = volumeCalc->GetVolumeCount();
    //std::cout<<"End volume calc!!"<<std::endl;

    //m_Mask->setData( outcaster->GetOutput());
    m_Mask->setData( volumeCalc->GetOutput() );
    //m_Mask->setData( binaryDilate->GetOutput());
    //m_Volume->setData( maskAux );
    //m_Mask->setData( maskAux );
    //std::cout<<"Abans update!!"<<std::endl;
    //m_Mask->getVtkData()->Update();
    // m_Mask  = outcaster->GetOutput();

    //std::cout<<"Abans delete!!"<<std::endl;

    //maskAux->Delete();
    //std::cout<<"End!!"<<std::endl;

    return m_volume;
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



    std::cout<<"Init filter"<<std::endl;

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

void rectumSegmentationMethod::computeSpeedMap( itk::Image< float, 3 >* speedMap)
{
    typedef   float           InternalPixelType;
    typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

    InternalImageType::Pointer auxVolume = InternalImageType::New();
    auxVolume->SetRegions( m_Volume->getItkData()->GetLargestPossibleRegion() );
    auxVolume->SetSpacing( m_Volume->getItkData()->GetSpacing() );
    auxVolume->SetOrigin ( m_Volume->getItkData()->GetOrigin() );
    auxVolume->Allocate();

    m_filteredInputImage = new Volume();

    this->applyFilter(m_filteredInputImage);
    itk::ImageRegionIterator<InternalImageType> auxIt( auxVolume, auxVolume->GetBufferedRegion());
    //itk::ImageRegionIterator<Volume::ItkImageType> mainIt( m_Volume->getItkData(), m_Volume->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> mainIt( m_filteredInputImage->getItkData(), m_filteredInputImage->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> maskIt( m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());
    auxIt.GoToBegin();
    mainIt.GoToBegin();
    maskIt.GoToBegin();

    while(!auxIt.IsAtEnd())
    {
        if(maskIt.Value() == m_insideMaskValue)
        {
            auxIt.Set(1.0);
        }
        else if(mainIt.Value() < m_lowerVentriclesThreshold ||mainIt.Value() > m_upperVentriclesThreshold)
        {
            auxIt.Set(0.0);
        }
        else
        {
            auxIt.Set(0.5);
        }
        ++auxIt;
        ++mainIt;
        ++maskIt;
    }

    /*typedef itk::RescaleIntensityImageFilter< InternalImageType, InternalImageType> RescaleFilterType;
    typedef itk::CastImageFilter< InternalImageType, Volume::ItkImageType > OutputCastingFilterType;


    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();

  RescaleFilterType::Pointer rescaler = RescaleFilterType::New();

  rescaler->SetInput(auxVolume);
  rescaler->SetOutputMinimum(   0 );
  rescaler->SetOutputMaximum( 255 );
  outcaster->SetInput(rescaler->GetOutput());

    std::cout<<"Init filter"<<std::endl;

    try
    {
        outcaster->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    output->setData( outcaster->GetOutput());
    output->getVtkData()->Update();*/
    speedMap = auxVolume;

    return;
}

double rectumSegmentationMethod::applyMethodRectum(Volume * lesionMask)
{
    typedef   float           InternalPixelType;
    typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

    typedef itk::CastImageFilter< InternalImageType, Volume::ItkImageType > OutputCastingFilterType;
    typedef  itk::BinaryThresholdImageFilter<InternalImageType, Volume::ItkImageType> ThresholdingFilterType;
    typedef  itk::FastMarchingImageFilter< InternalImageType, InternalImageType > FastMarchingFilterType;

    typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType,Volume::VDimension> StructuringElementType;
    typedef itk::BinaryDilateImageFilter<Volume::ItkImageType,Volume::ItkImageType,StructuringElementType > DilateFilterType;

    typedef itk::ResampleImageFilter<Volume::ItkImageType,Volume::ItkImageType> ResampleFilterType;
    typedef itk::AffineTransform< double, 3 >  TransformType;
    typedef itk::NearestNeighborInterpolateImageFunction<Volume::ItkImageType, double >  InterpolatorType;


    //Resamplagem la imatge per tal de que tingui un vòxel isomètric (el mètode ho requereix)
    ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    TransformType::Pointer transform = TransformType::New();
    resampleFilter->SetTransform( transform );
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resampleFilter->SetInterpolator( interpolator );
    resampleFilter->SetDefaultPixelValue( 0 );
    double spacing[ 3 ] , newspacing[ 3 ] ;
    //Fem el vòxel isomètric
    m_Volume->getSpacing(spacing);
    newspacing[0] = 2.0*spacing[0]; // pixel spacing in millimeters along X
    newspacing[1] = 2.0*spacing[0]; // pixel spacing in millimeters along Y
    newspacing[2] = 2.0*spacing[0]; // pixel spacing in millimeters along Z

    resampleFilter->SetOutputSpacing( newspacing );

    double origin[ 3 ], neworigin[ 3 ];
    m_Volume->getOrigin(origin);
    neworigin[0] = origin[0];  // X space coordinate of origin
    neworigin[1] = origin[1];  // Y space coordinate of origin
    neworigin[2] = origin[2];  // Z space coordinate of origin

    resampleFilter->SetOutputOrigin( neworigin );
    Volume::ItkImageType::SizeType   size, newsize;
    size = m_Volume->getItkData()->GetBufferedRegion().GetSize();

    newsize[0] = (unsigned long int)((size[0]*spacing[0])/newspacing[0]);  // number of pixels along X
    newsize[1] = (unsigned long int)((size[1]*spacing[1])/newspacing[1]);  // number of pixels along Y
    newsize[2] = (unsigned long int)((size[2]*spacing[2])/newspacing[2]);  // number of pixels along Y

    resampleFilter->SetSize( newsize );
    resampleFilter->SetInput( m_Volume->getItkData() );
    resampleFilter->Update();
    std::cout<<"resampleVolumeOriginal: ["<<origin[0]<<","<<origin[1]<<","<<origin[2]<<"] ,["<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<"] ,"<<size<<std::endl;
    std::cout<<"resampleVolumeNew: ["<<neworigin[0]<<","<<neworigin[1]<<","<<neworigin[2]<<"] ,["<<newspacing[0]<<","<<newspacing[1]<<","<<newspacing[2]<<"] ,"<<newsize<<std::endl;

    //Resamplagem la màscara per tal de que tingui un vòxel isomètric (el mètode ho requereix)
    ResampleFilterType::Pointer resampleMaskFilter = ResampleFilterType::New();
    TransformType::Pointer transformMask = TransformType::New();
    resampleMaskFilter->SetTransform( transformMask );
    InterpolatorType::Pointer interpolatorMask = InterpolatorType::New();
    resampleMaskFilter->SetInterpolator( interpolatorMask );
    resampleMaskFilter->SetDefaultPixelValue( m_outsideMaskValue );
    //Fem el vòxel isomètric
    m_Mask->getSpacing(spacing);
    newspacing[0] = 2.0*spacing[0]; // pixel spacing in millimeters along X
    newspacing[1] = 2.0*spacing[0]; // pixel spacing in millimeters along Y
    newspacing[2] = 2.0*spacing[0]; // pixel spacing in millimeters along Z

    resampleMaskFilter->SetOutputSpacing( newspacing );

    m_Mask->getOrigin(origin);
    neworigin[0] = origin[0];  // X space coordinate of origin
    neworigin[1] = origin[1];  // Y space coordinate of origin
    neworigin[2] = origin[2];  // Z space coordinate of origin

    resampleMaskFilter->SetOutputOrigin( neworigin );
    size = m_Mask->getItkData()->GetBufferedRegion().GetSize();

    newsize[0] = (unsigned long int)((size[0]*spacing[0])/newspacing[0]);  // number of pixels along X
    newsize[1] = (unsigned long int)((size[1]*spacing[1])/newspacing[1]);  // number of pixels along Y
    newsize[2] = (unsigned long int)((size[2]*spacing[2])/newspacing[2]);  // number of pixels along Y

    resampleMaskFilter->SetSize( newsize );
    resampleMaskFilter->SetInput( m_Mask->getItkData() );

    std::cout<<"resampleMaskOriginal: ["<<origin[0]<<","<<origin[1]<<","<<origin[2]<<"] ,["<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<"] ,"<<size<<std::endl;
    std::cout<<"resampleMaskNew: ["<<neworigin[0]<<","<<neworigin[1]<<","<<neworigin[2]<<"] ,["<<newspacing[0]<<","<<newspacing[1]<<","<<newspacing[2]<<"] ,"<<newsize<<std::endl;

    //Fi resample

    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();

    unsigned long radiusDilate[3];
    radiusDilate[0]=2;
    radiusDilate[1]=2;
    radiusDilate[2]=2;
    StructuringElementType structuringElementDilate;
    structuringElementDilate.SetRadius( radiusDilate ); // 3x3 structuring element
    structuringElementDilate.CreateStructuringElement();

    binaryDilate->SetDilateValue( m_insideMaskValue );
    binaryDilate->SetKernel( structuringElementDilate );
    //binaryDilate->SetInput( m_Mask->getItkData() );
    binaryDilate->SetInput( resampleMaskFilter->GetOutput() );

    binaryDilate->Update();


    //std::cout<<"FI Dilate!!"<<std::endl;


    //itk::ImageRegionIterator<Volume::ItkImageType> mainIt( m_Volume->getItkData(), m_Volume->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> mainIt( resampleFilter->GetOutput(), resampleFilter->GetOutput()->GetBufferedRegion());
    //itk::ImageRegionIterator<Volume::ItkImageType> maskIt( m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> maskIt( binaryDilate->GetOutput(), binaryDilate->GetOutput()->GetBufferedRegion());

    //COmpute image statistics
    const unsigned int MeasurementVectorLength = 1;
    typedef itk::Vector< Volume::ItkImageType::PixelType, MeasurementVectorLength > MeasurementVectorType;
    typedef itk::Statistics::ListSample< MeasurementVectorType > SampleType;
    SampleType::Pointer sample = SampleType::New();
    sample->SetMeasurementVectorSize( MeasurementVectorLength );
    MeasurementVectorType mv;
    mainIt.GoToBegin();
    maskIt.GoToBegin();

    //std::cout<<"Threshold:"<<m_upperVentriclesThreshold<<" "<<m_lowerVentriclesThreshold<<std::endl;
    std::vector<unsigned int> hist(m_upperVentriclesThreshold - m_lowerVentriclesThreshold + 1,0);
    while(!mainIt.IsAtEnd())
    {
        if(mainIt.Value() >= m_lowerVentriclesThreshold && mainIt.Value() < m_upperVentriclesThreshold)
        {
            mv[0] = mainIt.Value();
            sample->PushBack( mv );
            hist[mv[0] - m_lowerVentriclesThreshold]++;
        }
        ++mainIt;
    }

    //std::cout<<"FI histo!!"<<std::endl;
    //std::ofstream fout("HistogramaGM.dat", ios::out);
    unsigned int max=0;
    int mean = 0;
    for(unsigned int k=0;k<hist.size();k++)
    {
        //fout<<hist[k]<<std::endl;
        //std::cout<<k<<": "<<hist[k]<<std::endl;
        if(hist[k]>max)
        {
            max=hist[k];
            mean=k + m_lowerVentriclesThreshold;//L'hem restat en la generació de listograma
        }
    }
    std::cout<<"estimed mean: "<<mean<<std::endl;

    typedef itk::Statistics::CovarianceCalculator< SampleType > CovarianceAlgorithmType;
    CovarianceAlgorithmType::Pointer covarianceAlgorithm = CovarianceAlgorithmType::New();

    covarianceAlgorithm->SetInputSample( sample );
    covarianceAlgorithm->SetMean( 0 );
    covarianceAlgorithm->Update();

    std::cout << "Using the one pass algorithm:" << std::endl;
    std::cout << "Mean = " << std::endl ;
    std::cout << *(covarianceAlgorithm->GetMean()) << std::endl;

    std::cout << "Covariance = " << std::endl ;
    std::cout << *(covarianceAlgorithm->GetOutput()) << std::endl;
    //double mean = (*covarianceAlgorithm->GetMean())[0];
    //double variance = (*covarianceAlgorithm->GetOutput())[0][0];

    //Cas Comas Pey!!!!!!
    //mean = 30;
    //variance = 50;
    // !!!!!!!!!!!!!!!!!!!!!

    m_mean=mean;
    m_variance=sqrt((*covarianceAlgorithm->GetOutput())[0][0]);
    std::cout <<"Mean: "<<m_mean<<", Variance: "<<m_variance << std::endl;
    //computeSpeedMap(speedMapVolume);

  typedef itk::CastImageFilter< Volume::ItkImageType, InternalImageType> InputCastingFilterType;
  InputCastingFilterType::Pointer cast = InputCastingFilterType::New();

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();

  thresholder->SetLowerThreshold( -1000.0 );
  thresholder->SetUpperThreshold(     0.0 );

  thresholder->SetOutsideValue( m_outsideMaskValue );
  thresholder->SetInsideValue(  m_insideMaskValue  );

  typedef  itk::FastMarchingImageFilter< InternalImageType, InternalImageType >
    FastMarchingFilterType;

  FastMarchingFilterType::Pointer  fastMarching = FastMarchingFilterType::New();


  typedef  itk::ErfcLevelSetImageFilter< InternalImageType, InternalImageType,  Volume::ItkImageType > ErfcLevelSetImageFilterType;
  ErfcLevelSetImageFilterType::Pointer erfcSegmentation = ErfcLevelSetImageFilterType::New();

  erfcSegmentation->SetPropagationScaling( 1.0 );
  erfcSegmentation->SetCurvatureScaling( m_stoppingTime );

  erfcSegmentation->SetMaximumRMSError( 0.01 );
  erfcSegmentation->SetNumberOfIterations( 500 );

  erfcSegmentation->SetLowerThreshold( m_lowerVentriclesThreshold );
  erfcSegmentation->SetUpperThreshold( m_upperVentriclesThreshold );
  erfcSegmentation->SetIsoSurfaceValue(0.0);
  erfcSegmentation->SetMean(m_mean);
  erfcSegmentation->SetVariance(m_variance);
  erfcSegmentation->SetConstant(m_constant);
  erfcSegmentation->SetMultiplier(m_multiplier);
  erfcSegmentation->SetAlpha(m_alpha);
  erfcSegmentation->SetMaskInsideValue( m_insideMaskValue );

  erfcSegmentation->SetInput( fastMarching->GetOutput() );
  //cast->SetInput( m_Volume->getItkData() );
  cast->SetInput( resampleFilter->GetOutput() );
  //erfcSegmentation->SetFeatureImage( m_Volume->getItkData() );
  erfcSegmentation->SetFeatureImage( cast->GetOutput() );
  //Introduim la màscara dilatada per evitar el PV effect
  erfcSegmentation->SetMaskImage( binaryDilate->GetOutput() );
//   erfcSegmentation->SetMaskImage( m_Mask->getItkData() );
  thresholder->SetInput( erfcSegmentation->GetOutput() );

  typedef FastMarchingFilterType::NodeContainer           NodeContainer;
  typedef FastMarchingFilterType::NodeType                NodeType;

  NodeContainer::Pointer seeds = NodeContainer::New();

    InternalImageType::IndexType  seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    //m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
    resampleMaskFilter->GetOutput()->TransformPhysicalPointToIndex(seedPoint, seedIndex);


  const double initialDistance = 20.0;

  NodeType node;

  const double seedValue = - initialDistance;

  node.SetValue( seedValue );
  node.SetIndex( seedIndex );

  seeds->Initialize();
  seeds->InsertElement( 0, node );

  fastMarching->SetTrialPoints(  seeds  );

  fastMarching->SetSpeedConstant( 1.0 );

  try
    {
        //Falta fastaMarching setOutputOrigin i Spacing!!!
      fastMarching->SetOutputOrigin(resampleMaskFilter->GetOutput()->GetOrigin());
      fastMarching->SetOutputSpacing(resampleMaskFilter->GetOutput()->GetSpacing());
      fastMarching->SetOutputSize(resampleMaskFilter->GetOutput()->GetBufferedRegion().GetSize() );
      thresholder->Update();
    }
  catch( itk::ExceptionObject & excep )
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }
  // Software Guide : EndCodeSnippet

  // Print out some useful information
  std::cout << std::endl;
  std::cout << erfcSegmentation << std::endl;
  std::cout << std::endl;
  std::cout << "Max. no. iterations: " << erfcSegmentation->GetNumberOfIterations() << std::endl;
  std::cout << "Max. RMS error: " << erfcSegmentation->GetMaximumRMSError() << std::endl;
  std::cout << "No. elapsed iterations: " << erfcSegmentation->GetElapsedIterations() << std::endl;
  std::cout << "RMS change: " << erfcSegmentation->GetRMSChange() << std::endl;
  std::cout << std::endl;

  typedef itk::ImageFileWriter< InternalImageType > InternalWriterType;

  InternalWriterType::Pointer mapWriter = InternalWriterType::New();
  mapWriter->SetInput( fastMarching->GetOutput() );
  mapWriter->SetFileName("fastMarchingImage.mhd");
  mapWriter->Update();

  OutputCastingFilterType::Pointer outcast = OutputCastingFilterType::New();
  typedef itk::ImageFileWriter< Volume::ItkImageType > VolumeWriterType;
  VolumeWriterType::Pointer speedWriter = VolumeWriterType::New();
  outcast->SetInput( erfcSegmentation->GetSpeedImage() );
  speedWriter->SetInput( outcast->GetOutput() );
  speedWriter->SetFileName("speedTermImage.mhd");
  speedWriter->Update();

  VolumeWriterType::Pointer maskWriter = VolumeWriterType::New();
  maskWriter->SetInput( thresholder->GetOutput() );
  maskWriter->SetFileName("outputMaskImage.mhd");
  maskWriter->Update();

    //Posem l'origin i l'spacing correctes a la sortida del filtre
    //No sé perquè no ho fa directament les ITKs!!!!
//     thresholder->GetOutput()->SetOrigin(resampleMaskFilter->GetOutput()->GetOrigin());
//     thresholder->GetOutput()->SetSpacing(resampleMaskFilter->GetOutput()->GetSpacing());
//
    Volume::ItkImageType::PointType newor = thresholder->GetOutput()->GetOrigin();
    Volume::ItkImageType::SpacingType newsp = thresholder->GetOutput()->GetSpacing();
    newsize = thresholder->GetOutput()->GetBufferedRegion().GetSize();
    std::cout<<"thresholder: ["<<newor<<" ,"<<newsp<<" ,"<<newsize<<std::endl;


    //Resamplagem la màscara per tal de que tingui la mida original
    ResampleFilterType::Pointer resampleMaskFilter2 = ResampleFilterType::New();
    TransformType::Pointer transformMask2 = TransformType::New();
    resampleMaskFilter2->SetTransform( transformMask2 );
    InterpolatorType::Pointer interpolatorMask2 = InterpolatorType::New();
    resampleMaskFilter2->SetInterpolator( interpolatorMask2 );
    resampleMaskFilter2->SetDefaultPixelValue( m_outsideMaskValue );

    m_Mask->getSpacing(spacing);
    resampleMaskFilter2->SetOutputSpacing( spacing );

    m_Mask->getOrigin(origin);
    resampleMaskFilter2->SetOutputOrigin( origin );

    size = m_Mask->getItkData()->GetBufferedRegion().GetSize();
    resampleMaskFilter2->SetSize( size );
    resampleMaskFilter2->SetInput( thresholder->GetOutput() );
    resampleMaskFilter2->Update();

    std::cout<<"resample: ["<<origin[0]<<","<<origin[1]<<","<<origin[2]<<"] ,["<<spacing[0]<<","<<spacing[1]<<","<<spacing[2]<<"] ,"<<size<<std::endl;

    std::cout<<"Mask Set!!"<<std::endl;
    //lesionMask->setData( thresholder->GetOutput());
    lesionMask->setData( resampleMaskFilter2->GetOutput());

  VolumeWriterType::Pointer maskWriter2 = VolumeWriterType::New();
  maskWriter2->SetInput( resampleMaskFilter2->GetOutput() );
  maskWriter2->SetFileName("outputMaskResampledImage.mhd");
  maskWriter2->Update();


/*    itk::ImageRegionIterator<Volume::ItkImageType> lesionIt( lesionMask->getItkData(), lesionMask->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> hematomaIt( m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());

    //Compute mask difference
    lesionIt.GoToBegin();
    hematomaIt.GoToBegin();
    m_edemaCont = 0;

    std::cout<<"Difference: "<<m_insideMaskValue<<", "<<m_outsideMaskValue<<std::endl;

//     while(!lesionIt.IsAtEnd())
//     {
//         if(lesionIt.Value() == m_insideMaskValue)
//         {
//             if(hematomaIt.Value() == m_insideMaskValue)
//             {
//                 lesionIt.Set( m_outsideMaskValue);
//             }
//             else
//             {
//                 m_edemaCont++;
//             }
//         }
//         ++lesionIt;
//         ++hematomaIt;
//     }

    while(!lesionIt.IsAtEnd())
    {
        if(lesionIt.Value() == m_insideMaskValue)
        {
            m_edemaCont++;
        }
        ++lesionIt;
    }
*/
    lesionMask->getVtkData()->Update();
    std::cout<<"Lesion: "<<lesionMask->getItkData()->GetOrigin()<<" ,"<<lesionMask->getItkData()->GetSpacing()<<" ,"<<lesionMask->getItkData()->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Volume: "<<m_Volume->getItkData()->GetOrigin()<<" ,"<<m_Volume->getItkData()->GetSpacing()<<" ,"<<m_Volume->getItkData()->GetBufferedRegion().GetSize()<<std::endl;


    std::cout<<"End method!!"<<std::endl;

    double volspacing[3];
    m_Volume->getSpacing(volspacing);
    //std::cout<<"spacing"<<spacing[0]<<" "<<spacing[1]<<" "<<spacing[2]<<std::endl;
    std::cout<<"Volume: "<<m_edemaCont<<",* "<<volspacing[0]*volspacing[1]*volspacing[2]<<std::endl;

    return (double)m_edemaCont*volspacing[0]*volspacing[1]*volspacing[2];
}

double rectumSegmentationMethod::erfc(double x)
{
    // Compute the complementary error function erfc(x).
    // Erfc(x) = (2/sqrt(pi)) Integral(exp(-t^2))dt between x and infinity
    //
    //--- Nve 14-nov-1998 UU-SAP Utrecht
    // The parameters of the Chebyshev fit
    const double a1 = -1.26551223,   a2 = 1.00002368,
    a3 =  0.37409196,   a4 = 0.09678418,
    a5 = -0.18628806,   a6 = 0.27886807,
    a7 = -1.13520398,   a8 = 1.48851587,
    a9 = -0.82215223,  a10 = 0.17087277;

    double v = 1.0; // The return value
    double z = std::fabs(x);

    if (z <= 0) return v; // erfc(0)=1

    double t = 1.0/(1.0+0.5*z);

    v = t*std::exp((-z*z) +a1+t*(a2+t*(a3+t*(a4+t*(a5+t*(a6+t*(a7+t*(a8+t*(a9+t*a10)))))))));

    if (x < 0) v = 2.0-v; // erfc(-x)=2-erfc(x)

    return v;
 }

double rectumSegmentationMethod::applyVentriclesMethod()
{
    typedef   float           InternalPixelType;
    typedef itk::Image< InternalPixelType, 3 >  InternalImageType;

    typedef itk::CastImageFilter< Volume::ItkImageType, InternalImageType >            InputCastingFilterType;
    typedef itk::CastImageFilter< InternalImageType, Volume::ItkImageType >            OutputCastingFilterType;
    typedef itk::ConnectedThresholdImageFilter< InternalImageType, InternalImageType > ConnectedFilterType;
    typedef itk::VolumeCalculatorImageFilter< Volume::ItkImageType >                   VolumeCalcFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
    VolumeCalcFilterType::Pointer volumeCalc= VolumeCalcFilterType::New();

    incaster->SetInput( m_Volume->getItkData() );
    connectedThreshold->SetInput( incaster->GetOutput() );//Comentem aquesta l�ia per fer el filtratge
    outcaster->SetInput( connectedThreshold->GetOutput() );
    volumeCalc->SetInput(outcaster->GetOutput());

    connectedThreshold->SetLower(  m_lowerThreshold  );
    connectedThreshold->SetUpper(  m_upperThreshold  );
    connectedThreshold->SetReplaceValue( m_insideMaskValue );

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
    connectedThreshold->SetSeed( seedIndex );

    volumeCalc->SetInsideValue( m_insideMaskValue );

    std::cout<<"Init filter"<<std::endl;
    std::cout<<"Parameters: "<<seedIndex<<std::endl;
    std::cout<<"Histogram parameters: "<<m_lowerThreshold<<" "<<m_upperThreshold<<std::endl;


    try
    {
        volumeCalc->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    m_volume = volumeCalc->GetVolume();
    m_cont = volumeCalc->GetVolumeCount();

    m_Mask->setData( volumeCalc->GetOutput() );
    m_Mask->getVtkData()->Update();

    return m_volume;
}

}
