
#include "reglandmark.h"
#include <itkImageRegionConstIterator.h>
#include <itkNumericTraits.h>

namespace udg
{
// Constructor
template <typename TFixedImage, typename TMovingImage>
RegLandmark<TFixedImage,TMovingImage>
::RegLandmark() 
{  
    
    // Creaci贸 dels objectes necessaris  
    m_interpolator 	= InterpolatorType::New();
    m_resample 		= ResampleImageFilterType::New(); 
    m_fixedLandmarksReal  = PointSetType::New();
    m_movingLandmarksReal = PointSetType::New();
    m_registrator 	= LandmarkRegistratorType::New();
    
    // Configuraci贸 del LandmarkScales
    m_LandmarkScales.set_size(6) ; 
    m_LandmarkScales[0] = 200;
    m_LandmarkScales[1] = 200;
    m_LandmarkScales[2] = 200;
    m_LandmarkScales[3] = 1;
    m_LandmarkScales[4] = 1;
    m_LandmarkScales[5] = 1;
    m_LandmarkNumberOfIterations = 5000 ;
    
    // Creaci贸 i inicialitzaci贸 del transformadors necessaris per poder agafar 
    // la transformaci贸 una vegada s'ha realtizat el registre
    m_LandmarkRegTransform    = LandmarkRegTransformType::New() ;   
    m_LandmarkAffineTransform = AffineTransformType::New() ;
    m_LandmarkRegTransform->SetIdentity() ;
    m_LandmarkAffineTransform->SetIdentity() ;  
}


template <typename TFixedImage, typename TMovingImage>
void RegLandmark<TFixedImage,TMovingImage>
::setInputImages(TFixedImage* fixedImage, TMovingImage* movingImage )
{
    m_fixedImage = fixedImage;
    m_movingImage = movingImage; 
}



template <typename TFixedImage, typename TMovingImage>
void RegLandmark<TFixedImage,TMovingImage>
::setLandmarks(std::list< ClickPoint * > fixedPoints,std::list< ClickPoint * > movingPoints)
{   
    m_pointSetFixed=fixedPoints;
    m_pointSetMoving=movingPoints;  
    
    ClickPoint * cpFixed;
    ClickPoint * cpMoving;
    
    m_contador = 0;
    int size = m_pointSetFixed.size();
    
    PointType       pointFixed;  
    PointType       pointMoving;   
    
    while(m_contador<size)
    { 
        cpFixed  = m_pointSetFixed.front();
        cpMoving = m_pointSetMoving.front();
        m_pointSetFixed.pop_front();
        m_pointSetMoving.pop_front();
        
        pointFixed[0] = (double) cpFixed->x;   pointMoving[0] = (double) cpMoving->x;  
        pointFixed[1] = (double) cpFixed->y;   pointMoving[1] = (double) cpMoving->y;
        pointFixed[2] = (double) cpFixed->z;   pointMoving[2] = (double) cpMoving->z;
        
        m_fixedLandmarksReal->InsertElement(m_contador, pointFixed);     
        m_movingLandmarksReal->InsertElement(m_contador, pointMoving);     
        
        m_contador++;   
    }
}

template <typename TFixedImage, typename TMovingImage>
bool RegLandmark<TFixedImage,TMovingImage>
::applyMethod()
{
    m_registrator->SetFixedLandmarkSet(m_fixedLandmarksReal );
    m_registrator->SetMovingLandmarkSet(m_movingLandmarksReal );
    m_registrator->SetOptimizerScales( m_LandmarkScales );
    m_registrator->SetOptimizerNumberOfIterations( m_LandmarkNumberOfIterations );
        
    try
    {
        m_registrator->StartRegistration();
    }
    catch( itk::ExceptionObject &e )
    {
        std::cout << "ha petat aixo!! " << std::endl;
    }    
    
    // Agafem la transformaci贸 final que s'ha aplicat sobre la imatge m貌bil
    m_LandmarkRegTransform = m_registrator->GetTypedTransform();
    m_LandmarkAffineTransform->SetIdentity();
    m_LandmarkAffineTransform->SetMatrix(m_LandmarkRegTransform->GetRotationMatrix());
    m_LandmarkAffineTransform->SetOffset(m_LandmarkRegTransform->GetOffset());  
            
    std::cout << "11111!!! " << std::endl;
        
    m_interpolator->SetInputImage(m_movingImage);   
    m_resample->SetInput(m_movingImage); 
    m_resample->SetInterpolator(m_interpolator.GetPointer());  
    m_resample->SetSize(m_fixedImage->GetLargestPossibleRegion().GetSize());
    m_resample->SetOutputOrigin(m_fixedImage->GetOrigin());
    m_resample->SetOutputSpacing(m_fixedImage->GetSpacing());
    m_resample->SetDefaultPixelValue( 100 );
    typename AffineTransformType::Pointer LandmarkAffineInverseTransform;
    m_LandmarkAffineTransform->GetInverse(LandmarkAffineInverseTransform);
    m_resample->SetTransform(LandmarkAffineInverseTransform);  
    m_registeredImage = m_resample->GetOutput();                        
    m_resample->Update();      
    
    // C鄉cul de l'error Quadr鄑ic    
    itk::ImageRegionConstIterator<FixedImageType> imageFixed(m_fixedImage, m_fixedImage->GetLargestPossibleRegion());
    itk::ImageRegionConstIterator<FixedImageType> imageMoving(m_resample->GetOutput(), (m_resample->GetOutput())->GetLargestPossibleRegion());
    
    imageFixed.GoToBegin();
    imageMoving.GoToBegin();  
    int contador=0;
    int total=0;
        
    while (!imageFixed.IsAtEnd())
    {
    	FixedImagePixelType valueFixed = imageFixed.Get();
    	FixedImagePixelType valueMoving = imageMoving.Get();
    
 	total+=abs(valueFixed-valueMoving);
    	contador++;
    	++imageFixed;
    	++imageMoving;
   }  
   //std::cout << "prova: " <<  static_cast<typename itk::NumericTraits<FixedImagePixelType>::PrintType>(minFixed) << std::endl;

    std::cout << "Total: "<< total << " Numero pixels: " << contador << std::endl;
    
   /*
   // Error com閟 quan es realitza el registre (amb una imatge)
   
  difference= DifferenceFilterType::New();
  difference->SetInput1( m_fixedImage );
  difference->SetInput2( m_resample->GetOutput() );
  difference->Update();
  m_registeredImage=difference->GetOutput();
 */
    
  
return true;
}


template <typename TFixedImage, typename TMovingImage> 
TFixedImage* RegLandmark<TFixedImage,TMovingImage>
::getRegisteredImage()
{
    return m_registeredImage;
}

};
