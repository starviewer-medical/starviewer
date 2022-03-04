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
 *************************************************************************************/

#include "strokesegmentationmethod.h"

#include "itkErfcLevelSetImageFilter.h"
#include "itkVolumeCalculatorImageFilter.h"

// Per la utilització de clock()
#include <ctime>

#include <itkImage.h>
#include <itkImageRegionIterator.h>
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
#include <itkBinaryErodeImageFilter.h>
#include <itkBinaryDilateImageFilter.h>
#include <itkBinaryBallStructuringElement.h>

#include <itkVector.h>
#include <itkListSample.h>
#include <itkCovarianceSampleFilter.h>
#include <itkMedianImageFilter.h>
#include <itkBinaryMedianImageFilter.h>

#include <itkImageFileWriter.h>

#include <itkResampleImageFilter.h>
#include <itkAffineTransform.h>
#include <itkNearestNeighborInterpolateImageFunction.h>

#include <vtkImageThreshold.h>
#include <vtkImageData.h>

#include "logging.h"

namespace udg {

StrokeSegmentationMethod::StrokeSegmentationMethod()
{
    m_Volume = 0;
    m_Mask = 0;
    m_filteredInputImage = 0;

    m_px = 0.0;
    m_py = 0.0;
    m_pz = 0.0;

    m_lowerThreshold = 0;
    m_upperThreshold = 0;

    m_insideMaskValue = 255;
    m_outsideMaskValue = 0;

}

StrokeSegmentationMethod::~StrokeSegmentationMethod()
{
}

void StrokeSegmentationMethod::setSeedPosition(double x, double y, double z)
{
    m_px = x;
    m_py = y;
    m_pz = z;
}

void StrokeSegmentationMethod::setHistogramLowerLevel(int x)
{
    m_lowerThreshold = x;
}

void StrokeSegmentationMethod::setHistogramUpperLevel(int x)
{
    m_upperThreshold = x;
}

double StrokeSegmentationMethod::applyMethod()
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;
    typedef itk::CastImageFilter<InternalImageType, Volume::ItkImageType> OutputCastingFilterType;

    typedef itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType> CurvatureFlowImageFilterType;
    typedef itk::ConnectedThresholdImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();

    incaster->SetInput(m_Volume->getItkData());
    //smoothing->SetInput(incaster->GetOutput());
    //connectedThreshold->SetInput(smoothing->GetOutput());
    // Comentem aquesta línia per fer el filtratge
    connectedThreshold->SetInput(incaster->GetOutput());
    outcaster->SetInput(connectedThreshold->GetOutput());

    smoothing->SetNumberOfIterations(3);
    smoothing->SetTimeStep(0.0625);
    //smoothing->SetNumberOfIterations(numiterations);
    //smoothing->SetTimeStep(timeStep);

    connectedThreshold->SetLower(m_lowerThreshold);
    connectedThreshold->SetUpper(m_upperThreshold);
    connectedThreshold->SetReplaceValue(m_insideMaskValue);

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
    connectedThreshold->SetSeed(seedIndex);

    DEBUG_LOG("Init filter");
    DEBUG_LOG(QString("Parameters: %1").arg(*seedIndex.GetIndex()));
    std::cerr << "Parameters: " << seedIndex << std::endl;
    DEBUG_LOG(QString("Histogram parameters: %1,%2").arg(m_lowerThreshold).arg(m_upperThreshold));

    typedef itk::VolumeCalculatorImageFilter<Volume::ItkImageType> VolumeCalcFilterType;
    VolumeCalcFilterType::Pointer volumeCalc = VolumeCalcFilterType::New();
    volumeCalc->SetInput(outcaster->GetOutput());
    volumeCalc->SetInsideValue(m_insideMaskValue);

    try
    {
        //outcaster->Update();
        volumeCalc->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }

    m_volume = volumeCalc->GetVolume();
    m_cont = volumeCalc->GetVolumeCount();
    DEBUG_LOG(QString("MCONT>%1").arg(m_cont));

    //m_Mask->setData(outcaster->GetOutput());
    m_Mask->setData(volumeCalc->GetOutput());
    // m_Mask  = outcaster->GetOutput();

    return m_volume;
}

double StrokeSegmentationMethod::applyMethodVTK()
{
    double origin[3];
    double spacing[3];
    int index[3];
    m_cont = 0;
    vtkImageThreshold *imageThreshold = vtkImageThreshold::New();
    imageThreshold->SetInputData(m_Volume->getVtkData());
    imageThreshold->ThresholdBetween(m_lowerThreshold, m_upperThreshold);
    imageThreshold->SetInValue(m_insideMaskValue - 100);
    imageThreshold->SetOutValue(m_outsideMaskValue);
    DEBUG_LOG(QString("min: %1, mout: %2").arg(m_insideMaskValue).arg(m_outsideMaskValue));
    imageThreshold->Update();
    vtkImageData *imMask = imageThreshold->GetOutput();

    m_Volume->getSpacing(spacing);
    m_Volume->getOrigin(origin);
    index[0] = (int)(((double)m_px - origin[0]) / spacing[0]);
    index[1] = (int)(((double)m_py - origin[1]) / spacing[1]);
    index[2] = (int)(((double)m_pz - origin[2]) / spacing[2]);
    DEBUG_LOG(QString("Tractant llesca %1").arg(index[2]));

    applyMethodVTKRecursive(imMask, index[0], index[1], index[2], 0);

    DEBUG_LOG(QString("Tractant llesca %1").arg(index[2]));

    m_Mask->setData(imMask);
    imageThreshold->Delete();

    return m_cont * spacing[0] * spacing[1] * spacing[2];
}

void StrokeSegmentationMethod::applyMethodVTKRecursive(vtkImageData* imMask, int a, int b, int c, int prof)
{
    if ((a >= m_Volume->getExtent()[0]) && (a <= m_Volume->getExtent()[1]) && (b >= m_Volume->getExtent()[2]) && (b <=
        m_Volume->getExtent()[3]) && (c >= m_Volume->getExtent()[4]) && (c <= m_Volume->getExtent()[5]))
    {
        double maskValue = imMask->GetScalarComponentAsDouble(a, b, c, 0);
        if (maskValue == m_insideMaskValue - 100)
        {
            imMask->SetScalarComponentFromDouble(a, b, c, 0, m_insideMaskValue);
            m_cont++;
            applyMethodVTKRecursive(imMask, a, b, c + 1, prof + 1);
            applyMethodVTKRecursive(imMask, a, b, c - 1, prof + 1);
            applyMethodVTKRecursive(imMask, a + 1, b, c, prof + 1);
            applyMethodVTKRecursive(imMask, a - 1, b, c, prof + 1);
            applyMethodVTKRecursive(imMask, a, b + 1, c, prof + 1);
            applyMethodVTKRecursive(imMask, a, b - 1, c, prof + 1);
        }
    }
}

double StrokeSegmentationMethod::applyCleanSkullMethod()
{
    DEBUG_LOG("Clean Skull!!");
    typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType, Volume::VDimension> StructuringElementType;
    typedef itk::BinaryErodeImageFilter<Volume::ItkImageType, Volume::ItkImageType, StructuringElementType> ErodeFilterType;
    typedef itk::BinaryDilateImageFilter<Volume::ItkImageType, Volume::ItkImageType, StructuringElementType> DilateFilterType;

    typedef itk::ResampleImageFilter<Volume::ItkImageType, Volume::ItkImageType> ResampleFilterType;
    typedef itk::AffineTransform<double, 3> TransformType;
    typedef itk::NearestNeighborInterpolateImageFunction<Volume::ItkImageType, double> InterpolatorType;

    // Resamplagem la imatge per tal de que tingui menys vòxels i trigui menys a calcular
    ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    TransformType::Pointer transform = TransformType::New();
    resampleFilter->SetTransform(transform);
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resampleFilter->SetInterpolator(interpolator);
    resampleFilter->SetDefaultPixelValue(0);
    double spacing[3], newspacing[3];
    // Fem el vòxel isomètric
    double reducedSize = 3.0;
    m_Volume->getSpacing(spacing);
    // Pixel spacing in millimeters along X
    newspacing[0] = reducedSize * spacing[0];
    // Pixel spacing in millimeters along Y
    newspacing[1] = reducedSize * spacing[1];
    // Pixel spacing in millimeters along Z
    newspacing[2] = spacing[2];

    resampleFilter->SetOutputSpacing(newspacing);

    double origin[3], neworigin[3];
    m_Volume->getOrigin(origin);
    // X space coordinate of origin
    neworigin[0] = origin[0];
    // Y space coordinate of origin
    neworigin[1] = origin[1];
    // Z space coordinate of origin
    neworigin[2] = origin[2];

    resampleFilter->SetOutputOrigin(neworigin);
    Volume::ItkImageType::SizeType size, newsize;
    size = m_Volume->getItkData()->GetBufferedRegion().GetSize();

    newsize[0] = (long unsigned int)((size[0]) / reducedSize);
    newsize[1] = (long unsigned int)((size[1]) / reducedSize);
    newsize[2] = (size[2]);

    resampleFilter->SetSize(newsize);
    resampleFilter->SetInput(m_Mask->getItkData());
    unsigned long t1, t2, t3, t4, t5, t6, t7, t8;
    t7 = clock();
    resampleFilter->Update();
    t8 = clock();
    DEBUG_LOG(QString("resampleVolumeOriginal: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(origin[0]).arg(origin[1]).arg(origin[2])
        .arg(spacing[0]).arg(spacing[1]).arg(spacing[2])
        .arg(*size.GetSize()));
    DEBUG_LOG(QString("resampleVolumeNew: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(neworigin[0]).arg(neworigin[1]).arg(neworigin[2])
        .arg(newspacing[0]).arg(newspacing[1]).arg(newspacing[2])
        .arg(*newsize.GetSize()));

    ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();
    DilateFilterType::Pointer binaryDilatePre = DilateFilterType::New();

    binaryErode->SetErodeValue(m_insideMaskValue);
    binaryDilate->SetDilateValue(m_insideMaskValue);
    binaryDilatePre->SetDilateValue(m_insideMaskValue);
    binaryErode->SetBackgroundValue(m_outsideMaskValue);
    binaryDilate->SetBackgroundValue(m_outsideMaskValue);
    binaryDilatePre->SetBackgroundValue(m_outsideMaskValue);

    StructuringElementType::SizeType radiusDilatePre;
    radiusDilatePre[0] = 1;
    radiusDilatePre[1] = 1;
    radiusDilatePre[2] = 1;
    StructuringElementType structuringElementDilatePre;
    // 3x3 structuring element
    structuringElementDilatePre.SetRadius(radiusDilatePre);
    structuringElementDilatePre.CreateStructuringElement();

    binaryDilatePre->SetKernel(structuringElementDilatePre);

    //binaryDilatePre->SetInput(m_Mask->getItkData());
    binaryDilatePre->SetInput(resampleFilter->GetOutput());

    StructuringElementType::SizeType radiusErode;
    radiusErode[0] = 6;
    radiusErode[1] = 6;
    radiusErode[2] = 1;

    StructuringElementType structuringElementErode;
    // 3x3 structuring element
    structuringElementErode.SetRadius(radiusErode);
    structuringElementErode.CreateStructuringElement();

    binaryErode->SetKernel(structuringElementErode);

    //binaryErode->SetInput(m_Mask->getItkData());
    binaryErode->SetInput(binaryDilatePre->GetOutput());

    StructuringElementType::SizeType radiusDilate;
    radiusDilate[0] = 8;
    radiusDilate[1] = 8;
    radiusDilate[2] = 2;
    StructuringElementType structuringElementDilate;
    // 3x3 structuring element
    structuringElementDilate.SetRadius(radiusDilate);
    structuringElementDilate.CreateStructuringElement();

    binaryDilate->SetKernel(structuringElementDilate);

    binaryDilate->SetInput(binaryErode->GetOutput());
    t1 = clock();
    //medianFilter->Update();
    binaryDilate->Update();
    t2 = clock();

    // Resamplagem la imatge per tal de que tingui menys vòxels i trigui menys a calcular
    ResampleFilterType::Pointer resample2Filter = ResampleFilterType::New();
    TransformType::Pointer transform2 = TransformType::New();
    resample2Filter->SetTransform(transform2);
    InterpolatorType::Pointer interpolator2 = InterpolatorType::New();
    resample2Filter->SetInterpolator(interpolator2);
    resample2Filter->SetDefaultPixelValue(0);
    m_Mask->getSpacing(spacing);
    resample2Filter->SetOutputSpacing(spacing);

    m_Mask->getOrigin(origin);
    resample2Filter->SetOutputOrigin(origin);

    size = m_Mask->getItkData()->GetBufferedRegion().GetSize();
    resample2Filter->SetSize(size);
    resample2Filter->SetInput(binaryDilate->GetOutput());
    t5 = clock();
    resample2Filter->Update();
    t6 = clock();
    DEBUG_LOG(QString("resampleVolumeOriginal: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(origin[0]).arg(origin[1]).arg(origin[2])
        .arg(spacing[0]).arg(spacing[1]).arg(spacing[2])
        .arg(*size.GetSize()));
    DEBUG_LOG(QString("resampleVolumeNew: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(neworigin[0]).arg(neworigin[1]).arg(neworigin[2])
        .arg(newspacing[0]).arg(newspacing[1]).arg(newspacing[2])
        .arg(*newsize.GetSize()));

    Volume::ItkImageType::Pointer maskAux = Volume::ItkImageType::New();
    maskAux->SetSpacing(m_Mask->getItkData()->GetSpacing());
    maskAux->SetOrigin(m_Mask->getItkData()->GetOrigin());
    maskAux = resample2Filter->GetOutput();
    //maskAux = binaryDilate->GetOutput();
    //maskAux = medianFilter->GetOutput();

    itk::ImageRegionIterator<Volume::ItkImageType> maskIt(m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> auxIt(maskAux, maskAux->GetBufferedRegion());
    auxIt.GoToBegin();
    maskIt.GoToBegin();

    // Fem la intersecció de les dues màscares
    while (!auxIt.IsAtEnd())
    {
        if (maskIt.Value()!= m_insideMaskValue && auxIt.Value() == m_insideMaskValue)
        {
            // L'únic cas que canvia
            auxIt.Set(m_outsideMaskValue);
        }
        ++auxIt;
        ++maskIt;
    }
    t3 = clock();

    typedef itk::ConnectedThresholdImageFilter<Volume::ItkImageType, Volume::ItkImageType> ConnectedFilterType;

    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();

    connectedThreshold->SetInput(maskAux);

    connectedThreshold->SetLower(m_insideMaskValue - 1);
    connectedThreshold->SetUpper(m_insideMaskValue + 1);
    connectedThreshold->SetReplaceValue(m_insideMaskValue);

    Volume::ItkImageType::IndexType seedIndex;
    Volume::ItkImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
    connectedThreshold->SetSeed(seedIndex);

    typedef itk::VolumeCalculatorImageFilter<Volume::ItkImageType> VolumeCalcFilterType;
    VolumeCalcFilterType::Pointer volumeCalc = VolumeCalcFilterType::New();
    volumeCalc->SetInput(connectedThreshold->GetOutput());
    //volumeCalc->SetInput(resample2Filter->GetOutput());
    volumeCalc->SetInsideValue(m_insideMaskValue);

    try
    {
        volumeCalc->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }
    t4 = clock();
    DEBUG_LOG("Estudi temps:");
    DEBUG_LOG(QString("Resample1    : %1").arg((double)(t8 - t7) / 1000000.0));
    DEBUG_LOG(QString("Dilate/Erode : %1").arg((double)(t2 - t1) / 1000000.0));
    DEBUG_LOG(QString("Resample2    : %1").arg((double)(t6 - t5) / 1000000.0));
    DEBUG_LOG(QString("Interseccio  : %1").arg((double)(t3 - t6) / 1000000.0));
    DEBUG_LOG(QString("ConnectedThrd: %1").arg((double)(t4 - t3) / 1000000.0));
    DEBUG_LOG(QString("TOTAL        : %1").arg((double)(t4 - t7) / 1000000.0));

    m_volume = volumeCalc->GetVolume();
    m_cont = volumeCalc->GetVolumeCount();

    m_Mask->setData(volumeCalc->GetOutput());

    return m_volume;
}

void StrokeSegmentationMethod::applyFilter(Volume *output)
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;
    typedef itk::CastImageFilter<InternalImageType, Volume::ItkImageType> OutputCastingFilterType;

    typedef itk::CurvatureFlowImageFilter<InternalImageType, InternalImageType> CurvatureFlowImageFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    CurvatureFlowImageFilterType::Pointer smoothing = CurvatureFlowImageFilterType::New();

    incaster->SetInput(m_Volume->getItkData());
    smoothing->SetInput(incaster->GetOutput());
    outcaster->SetInput(smoothing->GetOutput());

    smoothing->SetNumberOfIterations(5);
    smoothing->SetTimeStep(0.0625);

    try
    {
        outcaster->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }

    // TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    output->setImages(m_Volume->getImages());

    output->setData(outcaster->GetOutput());

    return;
}

void StrokeSegmentationMethod::computeSpeedMap(itk::Image<float, 3>* speedMap)
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    InternalImageType::Pointer auxVolume = InternalImageType::New();
    auxVolume->SetRegions(m_Volume->getItkData()->GetLargestPossibleRegion());
    auxVolume->SetSpacing(m_Volume->getItkData()->GetSpacing());
    auxVolume->SetOrigin (m_Volume->getItkData()->GetOrigin());
    auxVolume->Allocate();

    m_filteredInputImage = new Volume();

    this->applyFilter(m_filteredInputImage);
    itk::ImageRegionIterator<InternalImageType> auxIt(auxVolume, auxVolume->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> mainIt(m_filteredInputImage->getItkData(), m_filteredInputImage->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> maskIt(m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());
    auxIt.GoToBegin();
    mainIt.GoToBegin();
    maskIt.GoToBegin();

    while (!auxIt.IsAtEnd())
    {
        if (maskIt.Value() == m_insideMaskValue)
        {
            auxIt.Set(1.0);
        }
        else if (mainIt.Value() < m_lowerVentriclesThreshold || mainIt.Value()> m_upperVentriclesThreshold)
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

//    speedMap = auxVolume; // useless assignation (maybe it was intended as an output parameter?)

    return;
}

double StrokeSegmentationMethod::applyMethodEdema(Volume *lesionMask)
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::BinaryThresholdImageFilter<InternalImageType, Volume::ItkImageType> ThresholdingFilterType;
    typedef itk::FastMarchingImageFilter<InternalImageType, InternalImageType> FastMarchingFilterType;

    typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType, Volume::VDimension> StructuringElementType;
    typedef itk::BinaryDilateImageFilter<Volume::ItkImageType, Volume::ItkImageType, StructuringElementType> DilateFilterType;

    typedef itk::ResampleImageFilter<Volume::ItkImageType, Volume::ItkImageType> ResampleFilterType;
    typedef itk::AffineTransform<double, 3> TransformType;
    typedef itk::NearestNeighborInterpolateImageFunction<Volume::ItkImageType, double> InterpolatorType;
    typedef itk::BinaryThresholdImageFilter<Volume::ItkImageType, Volume::ItkImageType> Thresholding2FilterType;
    typedef itk::LinearInterpolateImageFunction<Volume::ItkImageType, double> LinearInterpolatorType;

   // Variables per càlcul de temps
   unsigned long t1, t2, t3, t4, t5, t6, t7, t8;

   // Ampliem la màscara per evitar el partial volume effect
   DilateFilterType::Pointer binaryDilate = DilateFilterType::New();

    StructuringElementType::SizeType radiusDilate;
    radiusDilate[0] = 2;
    radiusDilate[1] = 2;
    radiusDilate[2] = 1;
    StructuringElementType structuringElementDilate;
    // 3x3 structuring element
    structuringElementDilate.SetRadius(radiusDilate);
    structuringElementDilate.CreateStructuringElement();

    binaryDilate->SetDilateValue(m_insideMaskValue);
    binaryDilate->SetKernel(structuringElementDilate);
    binaryDilate->SetInput(m_Mask->getItkData());
    //binaryDilate->SetInput(resampleMaskFilter->GetOutput());

    t1 = clock();
    binaryDilate->Update();

    // Resamplagem la imatge per tal de que tingui un vòxel isomètric (el mètode ho requereix)
    ResampleFilterType::Pointer resampleFilter = ResampleFilterType::New();
    TransformType::Pointer transform = TransformType::New();
    resampleFilter->SetTransform(transform);
    InterpolatorType::Pointer interpolator = InterpolatorType::New();
    resampleFilter->SetInterpolator(interpolator);
    resampleFilter->SetDefaultPixelValue(0);
    double spacing[3], newspacing[3];
    // Fem el vòxel isomètric
    double isometricSize = 4.0;
    m_Volume->getSpacing(spacing);
    // Pixel spacing in millimeters along X
    newspacing[0] = isometricSize * spacing[0];
    // Pixel spacing in millimeters along Y
    newspacing[1] = isometricSize * spacing[0];
    // Pixel spacing in millimeters along Z
    newspacing[2] = isometricSize * spacing[0];

    resampleFilter->SetOutputSpacing(newspacing);

    double origin[3], neworigin[3];
    m_Volume->getOrigin(origin);
    // X space coordinate of origin
    neworigin[0] = origin[0];
    // Y space coordinate of origin
    neworigin[1] = origin[1];
    // Z space coordinate of origin
    neworigin[2] = origin[2];

    resampleFilter->SetOutputOrigin(neworigin);
    Volume::ItkImageType::SizeType size, newsize;
    size = m_Volume->getItkData()->GetBufferedRegion().GetSize();

    // Number of pixels along X
    newsize[0] = (long unsigned int)((size[0] * spacing[0]) / newspacing[0]);
    // Number of pixels along Y
    newsize[1] = (long unsigned int)((size[1] * spacing[1]) / newspacing[1]);
    // Number of pixels along Z
    newsize[2] = (long unsigned int)((size[2] * spacing[2]) / newspacing[2]);

    resampleFilter->SetSize(newsize);
    resampleFilter->SetInput(m_Volume->getItkData());
    t2 = clock();
    resampleFilter->Update();
    DEBUG_LOG(QString("resampleVolumeOriginal: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(origin[0]).arg(origin[1]).arg(origin[2])
        .arg(spacing[0]).arg(spacing[1]).arg(spacing[2])
        .arg(*size.GetSize()));
    DEBUG_LOG(QString("resampleVolumeNew: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(neworigin[0]).arg(neworigin[1]).arg(neworigin[2])
        .arg(newspacing[0]).arg(newspacing[1]).arg(newspacing[2])
        .arg(*newsize.GetSize()));

    // Resamplagem la màscara per tal de que tingui un vòxel isomètric (el mètode ho requereix)
    ResampleFilterType::Pointer resampleMaskFilter = ResampleFilterType::New();
    TransformType::Pointer transformMask = TransformType::New();
    resampleMaskFilter->SetTransform(transformMask);
    InterpolatorType::Pointer interpolatorMask = InterpolatorType::New();
    resampleMaskFilter->SetInterpolator(interpolatorMask);
    resampleMaskFilter->SetDefaultPixelValue(m_outsideMaskValue);
    // Fem el vòxel isomètric
    m_Mask->getSpacing(spacing);
    // Pixel spacing in millimeters along X
    newspacing[0] = isometricSize * spacing[0];
    // Pixel spacing in millimeters along Y
    newspacing[1] = isometricSize * spacing[0];
    // Pixel spacing in millimeters along Z
    newspacing[2] = isometricSize * spacing[0];

    resampleMaskFilter->SetOutputSpacing(newspacing);

    m_Mask->getOrigin(origin);
    // X space coordinate of origin
    neworigin[0] = origin[0];
    // Y space coordinate of origin
    neworigin[1] = origin[1];
    // Z space coordinate of origin
    neworigin[2] = origin[2];

    resampleMaskFilter->SetOutputOrigin(neworigin);
    size = m_Mask->getItkData()->GetBufferedRegion().GetSize();

    // Number of pixels along X
    newsize[0] = (long unsigned int)((size[0] * spacing[0]) / newspacing[0]);
    // Number of pixels along Y
    newsize[1] = (long unsigned int)((size[1] * spacing[1]) / newspacing[1]);
    // Number of pixels along Z
    newsize[2] = (long unsigned int)((size[2] * spacing[2]) / newspacing[2]);

    resampleMaskFilter->SetSize(newsize);
    //resampleMaskFilter->SetInput(m_Mask->getItkData());
    resampleMaskFilter->SetInput(binaryDilate->GetOutput());
    resampleMaskFilter->Update();
    t3 = clock();

    DEBUG_LOG(QString("resampleMaskOriginal: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(origin[0]).arg(origin[1]).arg(origin[2])
        .arg(spacing[0]).arg(spacing[1]).arg(spacing[2])
        .arg(*size.GetSize()));
    DEBUG_LOG(QString("resampleMaskNew: [%1,%2,%3], [%4,%5,%6], %7")
        .arg(neworigin[0]).arg(neworigin[1]).arg(neworigin[2])
        .arg(newspacing[0]).arg(newspacing[1]).arg(newspacing[2])
        .arg(*newsize.GetSize()));
    // Fi resample

    itk::ImageRegionIterator<Volume::ItkImageType> mainIt(resampleFilter->GetOutput(), resampleFilter->GetOutput()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> maskIt(binaryDilate->GetOutput(), binaryDilate->GetOutput()->GetBufferedRegion());

    // Compute image statistics
    const unsigned int MeasurementVectorLength = 1;
    typedef itk::Vector<Volume::ItkImageType::PixelType, MeasurementVectorLength> MeasurementVectorType;
    typedef itk::Statistics::ListSample<MeasurementVectorType> SampleType;
    SampleType::Pointer sample = SampleType::New();
    sample->SetMeasurementVectorSize(MeasurementVectorLength);
    MeasurementVectorType mv;
    mainIt.GoToBegin();
    maskIt.GoToBegin();

    std::vector<unsigned int> hist(m_upperVentriclesThreshold - m_lowerVentriclesThreshold + 1, 0);
    while (!mainIt.IsAtEnd())
    {
        if (mainIt.Value() >= m_lowerVentriclesThreshold && mainIt.Value() < m_upperVentriclesThreshold)
        {
            mv[0] = mainIt.Value();
            sample->PushBack(mv);
            hist[mv[0] - m_lowerVentriclesThreshold]++;
        }
        ++mainIt;
    }

    unsigned int max = 0;
    int mean = 0;
    for (unsigned int k = 0; k < hist.size(); k++)
    {
        if (hist[k] > max)
        {
            max = hist[k];
            // L'hem restat en la generació de listograma
            mean = k + m_lowerVentriclesThreshold;
        }
    }
    DEBUG_LOG(QString("estimed mean: %1").arg(mean));

    typedef itk::Statistics::CovarianceSampleFilter<SampleType> CovarianceAlgorithmType;
    CovarianceAlgorithmType::Pointer covarianceAlgorithm = CovarianceAlgorithmType::New();

    covarianceAlgorithm->SetInput(sample);
    covarianceAlgorithm->Update();

    DEBUG_LOG("Using the one pass algorithm:");
    DEBUG_LOG("Mean = ");
    std::cout << covarianceAlgorithm->GetMean() << std::endl;

    DEBUG_LOG("Covariance = ");
    std::cout << covarianceAlgorithm->GetCovarianceMatrix() << std::endl;

    // Cas Comas Pey!!!!!!
    // mean = 30;
    // variance = 50;
    // !!!!!!!!!!!!!!!!!!!!!

    m_mean = mean;
    m_variance = sqrt(covarianceAlgorithm->GetCovarianceMatrix()[0][0]);
    DEBUG_LOG(QString("Mean: %1, Variance: %2").arg(m_mean).arg(m_variance));
    //computeSpeedMap(speedMapVolume);

    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;
    InputCastingFilterType::Pointer cast = InputCastingFilterType::New();

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();

    thresholder->SetLowerThreshold(-1000.0);
    thresholder->SetUpperThreshold(0.0);

    thresholder->SetOutsideValue(m_outsideMaskValue);
    thresholder->SetInsideValue(m_insideMaskValue);

    typedef itk::FastMarchingImageFilter<InternalImageType, InternalImageType> FastMarchingFilterType;

    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();

    typedef itk::ErfcLevelSetImageFilter<InternalImageType, InternalImageType, Volume::ItkImageType> ErfcLevelSetImageFilterType;
    ErfcLevelSetImageFilterType::Pointer erfcSegmentation = ErfcLevelSetImageFilterType::New();

    erfcSegmentation->SetPropagationScaling(1.0);
    //erfcSegmentation->SetCurvatureScaling(5.0);
    erfcSegmentation->SetCurvatureScaling(m_stoppingTime);

    erfcSegmentation->SetMaximumRMSError(0.001);
    //erfcSegmentation->SetNumberOfIterations(m_stoppingTime);
    erfcSegmentation->SetNumberOfIterations(500);

    erfcSegmentation->SetLowerThreshold(m_lowerVentriclesThreshold);
    erfcSegmentation->SetUpperThreshold(m_upperVentriclesThreshold);
    erfcSegmentation->SetIsoSurfaceValue(0.0);
    erfcSegmentation->SetMean(m_mean);
    erfcSegmentation->SetVariance(m_variance);
    erfcSegmentation->SetConstant(m_constant);
    erfcSegmentation->SetMultiplier(m_multiplier);
    erfcSegmentation->SetAlpha(m_alpha);
    erfcSegmentation->SetMaskInsideValue(m_insideMaskValue);

    erfcSegmentation->SetInput(fastMarching->GetOutput());
    cast->SetInput(resampleFilter->GetOutput());
    erfcSegmentation->SetFeatureImage(cast->GetOutput());
    // Introduim la màscara dilatada per evitar el PV effect
    //erfcSegmentation->SetMaskImage(binaryDilate->GetOutput());
    erfcSegmentation->SetMaskImage(resampleMaskFilter->GetOutput());
    thresholder->SetInput(erfcSegmentation->GetOutput());

    typedef FastMarchingFilterType::NodeContainer NodeContainer;
    typedef FastMarchingFilterType::NodeType NodeType;

    NodeContainer::Pointer seeds = NodeContainer::New();

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    resampleMaskFilter->GetOutput()->TransformPhysicalPointToIndex(seedPoint, seedIndex);

    const double initialDistance = 20.0;

    NodeType node;

    const double seedValue = -initialDistance;

    node.SetValue(seedValue);
    node.SetIndex(seedIndex);

    seeds->Initialize();
    seeds->InsertElement(0, node);

    fastMarching->SetTrialPoints(seeds);

    fastMarching->SetSpeedConstant(1.0);
    t4 = clock();

    try
    {
        // Falta fastaMarching setOutputOrigin i Spacing!!!
        fastMarching->SetOutputOrigin(resampleMaskFilter->GetOutput()->GetOrigin());
        fastMarching->SetOutputSpacing(resampleMaskFilter->GetOutput()->GetSpacing());
        fastMarching->SetOutputSize(resampleMaskFilter->GetOutput()->GetBufferedRegion().GetSize());
        thresholder->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }
    t5 = clock();

    // Print out some useful information
    std::cout << std::endl;
    std::cout << erfcSegmentation << std::endl;
    std::cout << std::endl;
    DEBUG_LOG(QString("Max. no. iterations: %1").arg(erfcSegmentation->GetNumberOfIterations()));
    DEBUG_LOG(QString("Max. RMS error: %1").arg(erfcSegmentation->GetMaximumRMSError()));
    DEBUG_LOG(QString("No. elapsed iterations: %1").arg(erfcSegmentation->GetElapsedIterations()));
    DEBUG_LOG(QString("RMS change: %1").arg(erfcSegmentation->GetRMSChange()));

    // Posem l'origin i l'spacing correctes a la sortida del filtre
    // No sé perquè no ho fa directament les ITKs!!!!

    Volume::ItkImageType::PointType newor = thresholder->GetOutput()->GetOrigin();
    Volume::ItkImageType::SpacingType newsp = thresholder->GetOutput()->GetSpacing();
    newsize = thresholder->GetOutput()->GetBufferedRegion().GetSize();
    DEBUG_LOG(QString("thresholder: (%1, %2, %3), (%4, %5, %6), (%7, %8, %9)")
        .arg(newor[0]).arg(newor[1]).arg(newor[2]).arg(newsp[0]).arg(newsp[1]).arg(newsp[2]).arg(newsize[0]).arg(newsize[1]).arg(newsize[2]));

    // Resamplagem la màscara per tal de que tingui la mida original
    ResampleFilterType::Pointer resampleMaskFilter2 = ResampleFilterType::New();
    TransformType::Pointer transformMask2 = TransformType::New();
    resampleMaskFilter2->SetTransform(transformMask2);
    //InterpolatorType::Pointer interpolatorMask2 = InterpolatorType::New();
    LinearInterpolatorType::Pointer interpolatorMask2 = LinearInterpolatorType::New();
    resampleMaskFilter2->SetInterpolator(interpolatorMask2);
    resampleMaskFilter2->SetDefaultPixelValue(m_outsideMaskValue);

    m_Mask->getSpacing(spacing);
    resampleMaskFilter2->SetOutputSpacing(spacing);

    m_Mask->getOrigin(origin);
    resampleMaskFilter2->SetOutputOrigin(origin);

    size = m_Mask->getItkData()->GetBufferedRegion().GetSize();
    resampleMaskFilter2->SetSize(size);
    resampleMaskFilter2->SetInput(thresholder->GetOutput());

    resampleMaskFilter2->Update();
    t8 = clock();

    Thresholding2FilterType::Pointer thresholder2 = Thresholding2FilterType::New();

    thresholder2->SetLowerThreshold((m_insideMaskValue - m_outsideMaskValue) / 2);
    thresholder2->SetUpperThreshold(m_insideMaskValue + 1);

    thresholder2->SetInsideValue(m_insideMaskValue);
    thresholder2->SetOutsideValue(m_outsideMaskValue);

    thresholder2->SetInput(resampleMaskFilter2->GetOutput());
    thresholder2->Update();

/*
    std::cout << "resample: [" << origin[0] << "," << origin[1] << "," << origin[2] << "] ,[" << spacing[0] << "," << spacing[1] << "," << spacing[2] << "] ,"
    // << size << std::endl;

    //Fem la mediana per tal d'"arrodonir" el resultat (així no queda voxelat)
    DilateFilterType::Pointer binaryDilate2 = DilateFilterType::New();
    unsigned long radiusDilate2[3];
    radiusDilate2[0]=2;
    radiusDilate2[1]=2;
    radiusDilate2[2]=0;
    StructuringElementType structuringElementDilate2;
    structuringElementDilate2.SetRadius(radiusDilate2); // 3x3 structuring element
    structuringElementDilate2.CreateStructuringElement();
    binaryDilate2->SetDilateValue(m_insideMaskValue);
    binaryDilate2->SetBackgroundValue(m_outsideMaskValue);
    binaryDilate2->SetKernel(structuringElementDilate2);
    binaryDilate2->SetInput(resampleMaskFilter2->GetOutput());

    typedef itk::BinaryErodeImageFilter<Volume::ItkImageType,Volume::ItkImageType,StructuringElementType> ErodeFilterType;
    ErodeFilterType::Pointer binaryErode = ErodeFilterType::New();
    binaryErode->SetErodeValue(m_insideMaskValue);
    unsigned long radiusErode[3];
    radiusErode[0]=4;
    radiusErode[1]=4;
    radiusErode[2]=0;
    StructuringElementType structuringElementErode;
    structuringElementErode.SetRadius(radiusErode); // 3x3 structuring element
    structuringElementErode.CreateStructuringElement();
    binaryErode->SetErodeValue(m_insideMaskValue);
    binaryErode->SetBackgroundValue(m_outsideMaskValue);
    binaryErode->SetKernel(structuringElementErode);
    binaryErode->SetInput(binaryDilate2->GetOutput());

    DilateFilterType::Pointer binaryDilate3 = DilateFilterType::New();
    binaryDilate3->SetDilateValue(m_insideMaskValue);
    unsigned long radiusDilate3[3];
    radiusDilate3[0]=2;
    radiusDilate3[1]=2;
    radiusDilate3[2]=0;
    StructuringElementType structuringElementDilate3;
    structuringElementDilate3.SetRadius(radiusDilate3); // 3x3 structuring element
    structuringElementDilate3.CreateStructuringElement();
    binaryDilate3->SetDilateValue(m_insideMaskValue);
    binaryDilate3->SetBackgroundValue(m_outsideMaskValue);
    binaryDilate3->SetKernel(structuringElementDilate3);
    binaryDilate3->SetInput(binaryErode->GetOutput());
    binaryDilate3->Update();
    */
    /*typedef itk::MedianImageFilter<Volume::ItkImageType,Volume::ItkImageType>  MedianFilterType;

    MedianFilterType::Pointer medianFilter = MedianFilterType::New();
    Volume::ItkImageType::SizeType indexRadius;

    indexRadius[0] = 2; // radius along x
    indexRadius[1] = 2; // radius along y
    indexRadius[2] = 0; // radius along y

    medianFilter->SetRadius(indexRadius);
    medianFilter->SetInput(resampleMaskFilter2->GetOutput());
    medianFilter->Update();
    */
    t6 = clock();

    DEBUG_LOG("Mask Set!!");
    //lesionMask->setData(resampleMaskFilter2->GetOutput());
    //lesionMask->setData(medianFilter->GetOutput());
    //lesionMask->setData(binaryDilate3->GetOutput());
    lesionMask->setData(thresholder2->GetOutput());

    itk::ImageRegionIterator<Volume::ItkImageType> lesionIt(lesionMask->getItkData(), lesionMask->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> hematomaIt(m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());

    // Compute mask difference
    lesionIt.GoToBegin();
    hematomaIt.GoToBegin();
    m_edemaCont = 0;

    DEBUG_LOG(QString("Difference: %1, %2").arg(m_insideMaskValue).arg(m_outsideMaskValue));

    while (!lesionIt.IsAtEnd())
    {
        if (lesionIt.Value() == m_insideMaskValue)
        {
            if (hematomaIt.Value() == m_insideMaskValue)
            {
                lesionIt.Set(m_outsideMaskValue);
            }
            else
            {
                m_edemaCont++;
            }
        }
        ++lesionIt;
        ++hematomaIt;
    }

    std::cout << "Lesion: " << lesionMask->getItkData()->GetOrigin() << " ," << lesionMask->getItkData()->GetSpacing() << " ," <<
                 lesionMask->getItkData()->GetBufferedRegion().GetSize() << std::endl;
    std::cout << "Volume: " << m_Volume->getItkData()->GetOrigin() << " ," << m_Volume->getItkData()->GetSpacing() << " ," <<
                 m_Volume->getItkData()->GetBufferedRegion().GetSize() << std::endl;

    DEBUG_LOG("End method!!");

    double volspacing[3];
    m_Volume->getSpacing(volspacing);
    DEBUG_LOG(QString("Volume: %1,* %2").arg(m_edemaCont).arg(volspacing[0] * volspacing[1] * volspacing[2]));
    t7 = clock();
    DEBUG_LOG("Estudi temps:");
    DEBUG_LOG(QString("Dilate          : %1").arg((double)(t2 - t1) / 1000000.0));
    DEBUG_LOG(QString("Resample inicial: %1").arg((double)(t3 - t2) / 1000000.0));
    DEBUG_LOG(QString("Mean i init     : %1").arg((double)(t4 - t3) / 1000000.0));
    DEBUG_LOG(QString("fast marching   : %1").arg((double)(t5 - t4) / 1000000.0));
    DEBUG_LOG(QString("resample 2      : %1").arg((double)(t8 - t5) / 1000000.0));
    DEBUG_LOG(QString("median filter   : %1").arg((double)(t6 - t8) / 1000000.0));
    DEBUG_LOG(QString("dif i volume    : %1").arg((double)(t7 - t6) / 1000000.0));
    DEBUG_LOG(QString("TOTAL           : %1").arg((double)(t7 - t1) / 1000000.0));

    return (double)m_edemaCont * volspacing[0] * volspacing[1] * volspacing[2];
}

double StrokeSegmentationMethod::applyMethodEdema2(Volume *lesionMask)
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

//    typedef itk::CastImageFilter<InternalImageType, Volume::ItkImageType> OutputCastingFilterType;
    typedef itk::BinaryThresholdImageFilter<InternalImageType, Volume::ItkImageType> ThresholdingFilterType;
    typedef itk::FastMarchingImageFilter<InternalImageType, InternalImageType> FastMarchingFilterType;

    typedef itk::BinaryBallStructuringElement<Volume::ItkPixelType, Volume::VDimension> StructuringElementType;
    typedef itk::BinaryDilateImageFilter<Volume::ItkImageType, Volume::ItkImageType, StructuringElementType> DilateFilterType;

    DilateFilterType::Pointer binaryDilate = DilateFilterType::New();

    StructuringElementType::SizeType radiusDilate;
    radiusDilate[0] = 2;
    radiusDilate[1] = 2;
    radiusDilate[2] = 1;
    StructuringElementType structuringElementDilate;
    // 3x3 structuring element
    structuringElementDilate.SetRadius(radiusDilate);
    structuringElementDilate.CreateStructuringElement();

    binaryDilate->SetDilateValue(m_insideMaskValue);
    binaryDilate->SetKernel(structuringElementDilate);
    binaryDilate->SetInput(m_Mask->getItkData());

    binaryDilate->Update();

    itk::ImageRegionIterator<Volume::ItkImageType> mainIt(m_Volume->getItkData(), m_Volume->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> maskIt(binaryDilate->GetOutput(), binaryDilate->GetOutput()->GetBufferedRegion());

    // COmpute image statistics
    const unsigned int MeasurementVectorLength = 1;
    typedef itk::Vector<Volume::ItkImageType::PixelType, MeasurementVectorLength> MeasurementVectorType;
    typedef itk::Statistics::ListSample<MeasurementVectorType> SampleType;
    SampleType::Pointer sample = SampleType::New();
    sample->SetMeasurementVectorSize(MeasurementVectorLength);
    MeasurementVectorType mv;
    mainIt.GoToBegin();
    maskIt.GoToBegin();

    std::vector<unsigned int> hist(m_upperVentriclesThreshold - m_lowerVentriclesThreshold + 1, 0);
    while (!mainIt.IsAtEnd())
    {
        if (mainIt.Value() >= m_lowerVentriclesThreshold && mainIt.Value() < m_upperVentriclesThreshold)
        {
            mv[0] = mainIt.Value();
            sample->PushBack(mv);
            hist[mv[0] - m_lowerVentriclesThreshold]++;
        }
        ++mainIt;
    }

    unsigned int max = 0;
    int mean = 0;
    for (unsigned int k = 0; k < hist.size(); k++)
    {
        if (hist[k] > max)
        {
            max = hist[k];
            // L'hem restat en la generació de listograma
            mean = k + m_lowerVentriclesThreshold;
        }
    }
    DEBUG_LOG(QString("estimed mean: %1").arg(mean));

    typedef itk::Statistics::CovarianceSampleFilter<SampleType> CovarianceAlgorithmType;
    CovarianceAlgorithmType::Pointer covarianceAlgorithm = CovarianceAlgorithmType::New();

    covarianceAlgorithm->SetInput(sample);
    covarianceAlgorithm->Update();

    DEBUG_LOG("Using the one pass algorithm:");
    DEBUG_LOG("Mean = ");
    std::cout << covarianceAlgorithm->GetMean() << std::endl;

    DEBUG_LOG("Covariance = ");
    std::cout << covarianceAlgorithm->GetCovarianceMatrix() << std::endl;
    const double sqrt2 = 1.41421356;
    //double mean = (*covarianceAlgorithm->GetMean())[0];
    //double variance = (*covarianceAlgorithm->GetOutput())[0][0];

    // Cas Comas Pey!!!!!!
    // mean = 30;
    // variance = 50;
    // !!!!!!!!!!!!!!!!!!!!!

    m_mean = mean;
    m_variance = sqrt(covarianceAlgorithm->GetCovarianceMatrix()[0][0]);
    DEBUG_LOG(QString("Mean: %1, Variance: %2").arg(m_mean).arg(m_variance));
    //computeSpeedMap(speedMapVolume);

    InternalImageType::Pointer speedMapVolume = InternalImageType::New();
    speedMapVolume->SetRegions(m_Volume->getItkData()->GetLargestPossibleRegion());
    speedMapVolume->SetSpacing(m_Volume->getItkData()->GetSpacing());
    speedMapVolume->SetOrigin (m_Volume->getItkData()->GetOrigin());
    speedMapVolume->Allocate();

    itk::ImageRegionIterator<InternalImageType> auxIt(speedMapVolume, speedMapVolume->GetBufferedRegion());

    auxIt.GoToBegin();
    mainIt.GoToBegin();
    maskIt.GoToBegin();

/*    const double multiplier = 1.0; */
//     double constVis = (double)m_insideMaskValue;
    //double constVis = 1.0;+
    double auxValue;
    //double alpha = 1.0;

    while (!auxIt.IsAtEnd())
    {
        if (maskIt.Value() == m_insideMaskValue)
        {
            auxIt.Set(1.0);
            //auxIt.Set(constVis);
        }
        else if (mainIt.Value() < m_lowerVentriclesThreshold || mainIt.Value() > m_upperVentriclesThreshold)
        {
            auxIt.Set(0.0);
        }
        else
        {
            auxValue = (0.5 * erfc(((double)mainIt.Value() - (m_mean - m_constant * m_variance)) / (m_variance * sqrt2)));
            auxIt.Set(m_multiplier * pow(auxValue, m_alpha));
            //auxIt.Set(constVis*pow(auxValue, m_alpha));
        }
        ++auxIt;
        ++mainIt;
        ++maskIt;
    }
    //std::cout << "FI Speed!!" << std::endl;

//     const double initialDistance = 5.0;
//     const int  timeThreshold = 70;
//     const double stoppingTime = 150.0;

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
    thresholder->SetLowerThreshold(0.0);
    thresholder->SetUpperThreshold(10000.0);
    thresholder->SetOutsideValue(m_outsideMaskValue);
    thresholder->SetInsideValue(m_insideMaskValue);

    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();

    typedef FastMarchingFilterType::NodeContainer NodeContainer;
    typedef FastMarchingFilterType::NodeType NodeType;

    NodeContainer::Pointer seeds = NodeContainer::New();

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);

    NodeType node;

    const double seedValue = -m_initialDistance;

    node.SetValue(seedValue);
    node.SetIndex(seedIndex);

    seeds->Initialize();
    seeds->InsertElement(0, node);

    fastMarching->SetTrialPoints(seeds);

    fastMarching->SetSpeedConstant(1.0);
    fastMarching->SetStoppingValue(m_stoppingTime);

    fastMarching->SetInput(speedMapVolume);
    thresholder->SetInput(fastMarching->GetOutput());

//     typedef itk::VolumeCalculatorImageFilter< Volume::ItkImageType> VolumeCalcFilterType;
//     VolumeCalcFilterType::Pointer volumeCalc= VolumeCalcFilterType::New();
//     volumeCalc->SetInput(thresholder->GetOutput());
//     volumeCalc->SetInsideValue(m_insideMaskValue);

    try
    {
        fastMarching->SetOutputSize(m_Volume->getItkData()->GetBufferedRegion().GetSize());
        thresholder->Update();
        //volumeCalc->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }
    //std::cout << "FI fastMarching!!" << std::endl;

    /*std::cout << std::endl;
    std::cout << "Max. no. iterations: " << thresholdSegmentation->GetNumberOfIterations() << std::endl;
    std::cout << "Max. RMS error: " << thresholdSegmentation->GetMaximumRMSError() << std::endl;
    std::cout << std::endl;
    std::cout << "No. elpased iterations: " << thresholdSegmentation->GetElapsedIterations() << std::endl;
    std::cout << "RMS change: " << thresholdSegmentation->GetRMSChange() << std::endl;
*/

/*    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    outcaster->SetInput(speedMapVolume);
    outcaster->Update();
    lesionMask->setData(outcaster->GetOutput());*/
    DEBUG_LOG("Mask Set!!");
    lesionMask->setData(thresholder->GetOutput());

    itk::ImageRegionIterator<Volume::ItkImageType> lesionIt(lesionMask->getItkData(), lesionMask->getItkData()->GetBufferedRegion());
    itk::ImageRegionIterator<Volume::ItkImageType> hematomaIt(m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());

    // Compute mask difference
    lesionIt.GoToBegin();
    hematomaIt.GoToBegin();
    m_edemaCont = 0;

    std::cout << "Difference: " << m_insideMaskValue << ", " << m_outsideMaskValue << std::endl;

    while (!lesionIt.IsAtEnd())
    {
        if (lesionIt.Value() == m_insideMaskValue)
        {
            if (hematomaIt.Value() == m_insideMaskValue)
            {
                lesionIt.Set(m_outsideMaskValue);
            }
            else
            {
                m_edemaCont++;
            }
        }
        ++lesionIt;
        ++hematomaIt;
    }

    std::cout << "End method!!" << std::endl;

    double spacing[3];
    m_Volume->getSpacing(spacing);
    //std::cout << "spacing" << spacing[0] << " " << spacing[1] << " " << spacing[2] << std::endl;
    //std::cout << "Volume: " << m_edemaCont << ",* " << spacing[0]*spacing[1]*spacing[2] << std::endl;
    return (double)m_edemaCont * spacing[0] * spacing[1] * spacing[2];
}

double StrokeSegmentationMethod::erfc(double x)
{
    // Compute the complementary error function erfc(x).
    // Erfc(x) = (2/sqrt(pi)) Integral(exp(-t^2))dt between x and infinity
    //
    // --- Nve 14-nov-1998 UU-SAP Utrecht
    // The parameters of the Chebyshev fit
    const double a1 = -1.26551223, a2 = 1.00002368,
    a3 =  0.37409196, a4 = 0.09678418,
    a5 = -0.18628806, a6 = 0.27886807,
    a7 = -1.13520398, a8 = 1.48851587,
    a9 = -0.82215223, a10 = 0.17087277;

    // The return value
    double v = 1.0;
    double z = std::fabs(x);

    if (z <= 0)
    {
        // erfc(0)=1
        return v;
    }

    double t = 1.0 / (1.0 + 0.5 * z);

    v = t * std::exp((-z * z) + a1 + t * (a2 + t * (a3 + t * (a4 + t * (a5 + t * (a6 + t * (a7 + t * (a8 + t * (a9 + t * a10)))))))));

    if (x < 0)
    {
        // erfc(-x)=2-erfc(x)
        v = 2.0 - v;
    }

    return v;
 }

double StrokeSegmentationMethod::applyVentriclesMethod()
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;
    typedef itk::CastImageFilter<InternalImageType, Volume::ItkImageType> OutputCastingFilterType;
    typedef itk::ConnectedThresholdImageFilter<InternalImageType, InternalImageType> ConnectedFilterType;
    typedef itk::VolumeCalculatorImageFilter<Volume::ItkImageType> VolumeCalcFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    ConnectedFilterType::Pointer connectedThreshold = ConnectedFilterType::New();
    VolumeCalcFilterType::Pointer volumeCalc = VolumeCalcFilterType::New();

    incaster->SetInput(m_Volume->getItkData());
    // Comentem aquesta línia per fer el filtratge
    connectedThreshold->SetInput(incaster->GetOutput());
    outcaster->SetInput(connectedThreshold->GetOutput());
    volumeCalc->SetInput(outcaster->GetOutput());

    connectedThreshold->SetLower(m_lowerThreshold);
    connectedThreshold->SetUpper(m_upperThreshold);
    connectedThreshold->SetReplaceValue(m_insideMaskValue);

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);
    connectedThreshold->SetSeed(seedIndex);

    volumeCalc->SetInsideValue(m_insideMaskValue);

    std::cout << "Init filter" << std::endl;
    std::cout << "Parameters: " << seedIndex << std::endl;
    std::cout << "Histogram parameters: " << m_lowerThreshold << " " << m_upperThreshold << std::endl;

    try
    {
        volumeCalc->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }

    m_volume = volumeCalc->GetVolume();
    m_cont = volumeCalc->GetVolumeCount();

    m_Mask->setData(volumeCalc->GetOutput());

    return m_volume;
}

void StrokeSegmentationMethod::applyMethod2()
{

    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;
//    typedef itk::CastImageFilter<InternalImageType, Volume::ItkImageType> OutputCastingFilterType;

    typedef itk::BinaryThresholdImageFilter<InternalImageType, Volume::ItkImageType> ThresholdingFilterType;
    typedef itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType> CurvatureAnisotropicFilterType;
    typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<InternalImageType, InternalImageType> GradientFilterType;
    typedef itk::SigmoidImageFilter<InternalImageType, InternalImageType> SigmoidFilterType;
    typedef itk::FastMarchingImageFilter<InternalImageType, InternalImageType> FastMarchingFilterType;

    const int timeThreshold = 70;
    const double sigma = 0.5;
    const double alpha = -0.3;
    const double beta = 2.0;
    const double stoppingTime = 200.0;

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
    thresholder->SetLowerThreshold(0.0);
    thresholder->SetUpperThreshold(timeThreshold);

    thresholder->SetOutsideValue(0);
    thresholder->SetInsideValue(255);

    CurvatureAnisotropicFilterType::Pointer smoothing = CurvatureAnisotropicFilterType::New();
    GradientFilterType::Pointer gradientMagnitude = GradientFilterType::New();
    SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
    sigmoid->SetOutputMinimum(0.0);
    sigmoid->SetOutputMaximum(1.0);
    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();
    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();

    incaster->SetInput(m_Volume->getItkData());
    //smoothing->SetInput(incaster->GetOutput());
    //gradientMagnitude->SetInput(smoothing->GetOutput());
    gradientMagnitude->SetInput(incaster->GetOutput());
    sigmoid->SetInput(gradientMagnitude->GetOutput());
    fastMarching->SetInput(sigmoid->GetOutput());
    thresholder->SetInput(fastMarching->GetOutput());

    smoothing->SetTimeStep(0.0625);
    smoothing->SetNumberOfIterations(2);
    smoothing->SetConductanceParameter(9.0);

    gradientMagnitude->SetSigma(sigma);

    sigmoid->SetAlpha(alpha);
    sigmoid->SetBeta(beta);

    typedef FastMarchingFilterType::NodeContainer NodeContainer;
    typedef FastMarchingFilterType::NodeType NodeType;
    NodeContainer::Pointer seeds = NodeContainer::New();

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);

    NodeType node;
    const double seedValue = 0.0;

    node.SetValue(seedValue);
    node.SetIndex(seedIndex);

    seeds->Initialize();
    seeds->InsertElement(0, node);

    fastMarching->SetTrialPoints(seeds);
    fastMarching->SetOutputSize(m_Volume->getItkData()->GetBufferedRegion().GetSize());
    fastMarching->SetStoppingValue(stoppingTime);

    try
    {
        thresholder->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }

    DEBUG_LOG("Fi process!");

   //maskImageOut = thresholder->GetOutput();

/*   typename OutputCastingFilterType::Pointer smoothcaster = OutputCastingFilterType::New();
  //smoothcaster->SetInput(smoothing->GetOutput());
  //smoothcaster->SetInput(gradientMagnitude->GetOutput());
  //smoothcaster->SetInput(sigmoid->GetOutput());
  smoothcaster->SetInput(fastMarching->GetOutput());

  std::cout<<"Init filter"<<std::endl;
  try
    {
    smoothcaster->Update();
    }
  catch(itk::ExceptionObject & excep)
    {
    std::cerr << "Exception caught !" << std::endl;
    std::cerr << excep << std::endl;
    }
  */
    m_Mask->setData(thresholder->GetOutput());
  //m_Mask->setData(smoothcaster->GetOutput());

}

int StrokeSegmentationMethod::computeSizeMask()
{
    typedef itk::ImageRegionIterator<Volume::ItkImageType> IteratorType;
    IteratorType imIt(m_Mask->getItkData(), m_Mask->getItkData()->GetBufferedRegion());
    int cont = 0;

    imIt.GoToBegin();
    while (!imIt.IsAtEnd())
    {
        if (imIt.Value() == m_insideMaskValue)
        {
            cont++;
        }
        ++imIt;
    }
    DEBUG_LOG(QString("VolumePelo = %1").arg(cont));

    return cont;
}

int StrokeSegmentationMethod::applyMethod3()
{
    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;
    typedef itk::CastImageFilter<InternalImageType, Volume::ItkImageType> OutputCastingFilterType;

    typedef itk::BinaryThresholdImageFilter<InternalImageType, Volume::ItkImageType> ThresholdingFilterType;
    typedef itk::FastMarchingImageFilter<InternalImageType, InternalImageType> FastMarchingFilterType;
    typedef itk::ThresholdSegmentationLevelSetImageFilter<InternalImageType, InternalImageType> ThresholdSegmentationLevelSetImageFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    incaster->SetInput(m_Volume->getItkData());

    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();
    thresholder->SetLowerThreshold(-1000.0);
    thresholder->SetUpperThreshold(0.0);
    thresholder->SetOutsideValue(m_outsideMaskValue);
    thresholder->SetInsideValue(m_insideMaskValue);

    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();

    ThresholdSegmentationLevelSetImageFilterType::Pointer thresholdSegmentation = ThresholdSegmentationLevelSetImageFilterType::New();
    thresholdSegmentation->SetPropagationScaling(1.0);
    thresholdSegmentation->SetCurvatureScaling(1.0);
    thresholdSegmentation->SetMaximumRMSError(0.02);
    thresholdSegmentation->SetNumberOfIterations(1200);
    thresholdSegmentation->SetUpperThreshold((float)m_upperThreshold);
    thresholdSegmentation->SetLowerThreshold((float)m_lowerThreshold);
    thresholdSegmentation->SetIsoSurfaceValue(0.0);
    thresholdSegmentation->SetInput(fastMarching->GetOutput());
    thresholdSegmentation->SetFeatureImage(incaster->GetOutput());
    thresholder->SetInput(thresholdSegmentation->GetOutput());

    typedef FastMarchingFilterType::NodeContainer NodeContainer;
    typedef FastMarchingFilterType::NodeType NodeType;

    NodeContainer::Pointer seeds = NodeContainer::New();

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);

    const double initialDistance = 5.0;

    NodeType node;

    const double seedValue = -initialDistance;

    node.SetValue(seedValue);
    node.SetIndex(seedIndex);

    seeds->Initialize();
    seeds->InsertElement(0, node);

    fastMarching->SetTrialPoints(seeds);

    fastMarching->SetSpeedConstant(1.0);

    try
    {
        fastMarching->SetOutputSize(m_Volume->getItkData()->GetBufferedRegion().GetSize());
        thresholder->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }

    std::cout << std::endl;
    std::cout << "Max. no. iterations: " << thresholdSegmentation->GetNumberOfIterations() << std::endl;
    std::cout << "Max. RMS error: " << thresholdSegmentation->GetMaximumRMSError() << std::endl;
    std::cout << std::endl;
    std::cout << "No. elpased iterations: " << thresholdSegmentation->GetElapsedIterations() << std::endl;
    std::cout << "RMS change: " << thresholdSegmentation->GetRMSChange() << std::endl;

    //m_Mask->setData(thresholder->GetOutput());
    OutputCastingFilterType::Pointer outcaster = OutputCastingFilterType::New();
    outcaster->SetInput(fastMarching->GetOutput());
    outcaster->Update();
    DEBUG_LOG("Mask Set!!");
    m_Mask->setData(outcaster->GetOutput());
    // m_Mask  = outcaster->GetOutput();

    return this->computeSizeMask();
}

int StrokeSegmentationMethod::applyMethod4()
{

    typedef float InternalPixelType;
    typedef itk::Image<InternalPixelType, 3> InternalImageType;

    typedef itk::BinaryThresholdImageFilter<InternalImageType, Volume::ItkImageType> ThresholdingFilterType;
    typedef itk::CastImageFilter<Volume::ItkImageType, InternalImageType> InputCastingFilterType;

    InputCastingFilterType::Pointer incaster = InputCastingFilterType::New();
    ThresholdingFilterType::Pointer thresholder = ThresholdingFilterType::New();

    thresholder->SetLowerThreshold(-1000.0);
    thresholder->SetUpperThreshold(0.0);

    thresholder->SetOutsideValue(0);
    thresholder->SetInsideValue(255);

    typedef itk::CurvatureAnisotropicDiffusionImageFilter<InternalImageType, InternalImageType> SmoothingFilterType;
    typedef itk::GradientMagnitudeRecursiveGaussianImageFilter<InternalImageType, InternalImageType> GradientFilterType;
    typedef itk::SigmoidImageFilter<InternalImageType, InternalImageType> SigmoidFilterType;
    typedef itk::FastMarchingImageFilter<InternalImageType, InternalImageType> FastMarchingFilterType;

    SmoothingFilterType::Pointer smoothing = SmoothingFilterType::New();
    GradientFilterType::Pointer gradientMagnitude = GradientFilterType::New();
    SigmoidFilterType::Pointer sigmoid = SigmoidFilterType::New();
    FastMarchingFilterType::Pointer fastMarching = FastMarchingFilterType::New();

    sigmoid->SetOutputMinimum(0.0);
    sigmoid->SetOutputMaximum(1.0);

    typedef itk::GeodesicActiveContourLevelSetImageFilter<InternalImageType, InternalImageType> GeodesicActiveContourFilterType;
    GeodesicActiveContourFilterType::Pointer geodesicActiveContour = GeodesicActiveContourFilterType::New();

    const double propagationScaling = 2.0;

    geodesicActiveContour->SetPropagationScaling(propagationScaling);
    geodesicActiveContour->SetCurvatureScaling(1.0);
    geodesicActiveContour->SetAdvectionScaling(1.0);

    geodesicActiveContour->SetMaximumRMSError(0.02);
    geodesicActiveContour->SetNumberOfIterations(800);

    incaster->SetInput(m_Volume->getItkData());
    smoothing->SetInput(incaster->GetOutput());
    gradientMagnitude->SetInput(smoothing->GetOutput());
    sigmoid->SetInput(gradientMagnitude->GetOutput());

    geodesicActiveContour->SetInput(fastMarching->GetOutput());
    geodesicActiveContour->SetFeatureImage(sigmoid->GetOutput());

    thresholder->SetInput(geodesicActiveContour->GetOutput());

    smoothing->SetTimeStep(0.0625);
    smoothing->SetNumberOfIterations(5);
    smoothing->SetConductanceParameter(3.0);

    const double sigma = 3.0;
    gradientMagnitude->SetSigma(sigma);

    const double alpha = 15.0;
    const double beta = 45.0;

    sigmoid->SetAlpha(alpha);
    sigmoid->SetBeta(beta);

    typedef FastMarchingFilterType::NodeContainer NodeContainer;
    typedef FastMarchingFilterType::NodeType NodeType;

    NodeContainer::Pointer seeds = NodeContainer::New();

    InternalImageType::IndexType seedIndex;
    InternalImageType::PointType seedPoint;
    seedPoint[0] = m_px;
    seedPoint[1] = m_py;
    seedPoint[2] = m_pz;
    m_Volume->getItkData()->TransformPhysicalPointToIndex(seedPoint, seedIndex);

    const double initialDistance = 5.0;

    NodeType node;

    const double seedValue = -initialDistance;

    node.SetValue(seedValue);
    node.SetIndex(seedIndex);

    seeds->Initialize();
    seeds->InsertElement(0, node);

    fastMarching->SetTrialPoints(seeds);

    fastMarching->SetSpeedConstant(1.0);

    fastMarching->SetOutputSize(m_Volume->getItkData()->GetBufferedRegion().GetSize());

    try
    {
        thresholder->Update();
    }
    catch(itk::ExceptionObject &excep)
    {
        DEBUG_LOG("Exception caught !");
        DEBUG_LOG(excep.what());
    }
    // Software Guide : EndCodeSnippet

    // Print out some useful information
    std::cout << std::endl;
    std::cout << "Max. no. iterations: " << geodesicActiveContour->GetNumberOfIterations() << std::endl;
    std::cout << "Max. RMS error: " << geodesicActiveContour->GetMaximumRMSError() << std::endl;
    std::cout << std::endl;
    std::cout << "No. elpased iterations: " << geodesicActiveContour->GetElapsedIterations() << std::endl;
    std::cout << "RMS change: " << geodesicActiveContour->GetRMSChange() << std::endl;

    m_Mask->setData(thresholder->GetOutput());
    return this->computeSizeMask();

}

}
