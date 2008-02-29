/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gr�ics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef __REGLANDMARK_h
#define __REGLANDMARK_h

#include "itkImage.h"
#include "itkResampleImageFilter.h"
#include "itkCastImageFilter.h"
#include "itkCommand.h"
#include "itkImageRegionIteratorWithIndex.h"
#include "itkImageBase.h"
#include "itkCommand.h"
#include "itkLinearInterpolateImageFunction.h"
#include "itkVectorContainer.h"
#include "landmarkregistrator.h"
#include "itkAffineTransform.h"
#include "itkSquaredDifferenceImageFilter.h"

namespace udg
{

// Estructura que ens representa un punt noReal.
//class udg::ClickPoint;

struct ClickPoint 
{
  double x, y, z;  
  
  ClickPoint(double _x,double _y,double _z)
    : x(_x),y(_y),z(_z){}
};

template <typename TFixedImage, typename TMovingImage>
class RegLandmark
{
 public:  
    
    typedef TFixedImage  FixedImageType;
    typedef TMovingImage MovingImageType;  
    
    typename FixedImageType::Pointer     m_fixedImage;
    typedef typename FixedImageType::PixelType  FixedImagePixelType;
    typename MovingImageType::Pointer    m_movingImage;
    typename FixedImageType::Pointer     m_registeredImage;  
    
    typedef typename FixedImageType::IndexType  IndexTypeFixed;
    typedef typename MovingImageType::IndexType IndexTypeMoving;  
    
    // Definició del contenidor de Punts que ens servirà per poder guardar
    // els punts marcats amb format pq el mètode de registre funcioni correctament (Punts Reals)
    typedef itk::Point<double,3>                                PointType;  
    typedef itk::VectorContainer<int, PointType>                PointSetType; 
    typedef PointSetType::Pointer                               PointSetTypePointer; 
    
    // Definició del contenidor de Punts però amb format ClikPoint (el que agafem quan fem 
    // el clic amb el ratolí).
    typedef std::list< ClickPoint * >                           ClickPointType;   
    typedef std::list< ClickPoint *>::iterator                  ClickPointTypeIterator;
        
    // Definició dels paràmetres referents al mètode de registre Especial (LandmarkRegistration)
    typedef LandmarkRegistrator                       LandmarkRegistratorType;
    typedef LandmarkRegistratorType::LandmarkType     LandmarkType;
    typedef LandmarkRegistratorType::LandmarkSetType  LandmarkSetType;
    typedef LandmarkRegistratorType::ParametersType   LandmarkParametersType;
    typedef LandmarkRegistratorType::ScalesType       LandmarkScalesType;
    typedef LandmarkRegistratorType::TransformType    LandmarkRegTransformType;
    
     //-------------- PROVA -------------------------
     typedef itk::SquaredDifferenceImageFilter< 
                                  FixedImageType, 
                                  FixedImageType, 
                                  FixedImageType > DifferenceFilterType;
				  
	typename DifferenceFilterType::Pointer difference;

   //---------------------------------------
        
    // Definició del mètode de registre  
    typedef itk::LinearInterpolateImageFunction< FixedImageType, double > InterpolatorType ;
    typedef itk::AffineTransform<double, 3>               		          AffineTransformType;
    typedef itk::ResampleImageFilter<FixedImageType,FixedImageType>       ResampleImageFilterType;   
        
    // ---------- METODES --------------
    /// Constructor
    RegLandmark();
    
    /// Destructor
    ~RegLandmark();
    
    /// Pas dels models a Registrar
    void setInputImages(TFixedImage* fixedImage, TMovingImage* movingImage );
    
    /// Pas de les marques col·locades a cada model
    void setLandmarks(ClickPointType fixedPoints,ClickPointType movingPoints); 
    
    /// Execució del mètode de Registre
    bool applyMethod();  
    
    /// Retorna el model Registrat
    TFixedImage* getRegisteredImage();     
 
 private:    
   
   int m_contador; 
    
    // Landmarks finals (coordenades reals)
    PointSetTypePointer  m_fixedLandmarksReal;  
    PointSetTypePointer  m_movingLandmarksReal;
    
    ClickPointType m_pointSetFixed;
    ClickPointType m_pointSetMoving;
           
    typename LandmarkRegistratorType::Pointer 	 m_registrator;
    LandmarkScalesType  			 m_LandmarkScales;   
    unsigned int        			 m_LandmarkNumberOfIterations; 
    
    // Transformadors per poder agafar la transformació una vegada
    // s'ha realitzat el registre.
    typename LandmarkRegTransformType::Pointer   m_LandmarkRegTransform;
    typename AffineTransformType::Pointer        m_LandmarkAffineTransform;
    typename AffineTransformType::Pointer        m_FinalTransform;
    
    // Per el tractament de les imatges un cop estan registrades
    typename InterpolatorType::Pointer           m_interpolator;
    typename ResampleImageFilterType::Pointer    m_resample;    
};

}; 
 
#endif
