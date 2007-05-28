/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�ics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLANDMARKREGISTRATOR_H
#define UDGLANDMARKREGISTRATOR_H

#include "leastsquareddistancecostfunction.h"
#include "itkResampleImageFilter.h"
#include "itkOnePlusOneEvolutionaryOptimizer.h"
#include "itkNormalVariateGenerator.h"
#include "itkVersorRigid3DTransform.h"
#include "itkEuler3DTransform.h"
#include "itkArray.h"
#include "itkObject.h"

namespace udg
{

class LandmarkRegistrator : public itk::Object
  {

  public:

    typedef LandmarkRegistrator                           Self;
    typedef itk::Object                                   Superclass;
    typedef itk::SmartPointer<Self>                       Pointer;
    typedef itk::SmartPointer<const Self>                 ConstPointer;

    //typedef itk::VersorRigid3DTransform< double >         TransformType;
    typedef itk::Euler3DTransform< double >         TransformType;
    typedef udg::LeastSquaredDistanceCostFunction<
                                      TransformType >     MetricType;
    typedef itk::Point<double, 3>                         PointType;

    typedef MetricType::PointType                         LandmarkType ;
    typedef MetricType::PointSetType                      LandmarkSetType;

    typedef itk::OnePlusOneEvolutionaryOptimizer          OptimizerType;
    typedef itk::Statistics::NormalVariateGenerator       NormalGeneratorType;
    typedef TransformType::ParametersType                 ParametersType;
    typedef TransformType::ParametersType                 ScalesType;

    itkTypeMacro(LandmarkRegistrator,Object);

    itkNewMacro(LandmarkRegistrator);


    //
    itkSetObjectMacro(Metric,MetricType);
    MetricType * GetTypedMetric(void)
      {
      return m_Metric;
      }

    itkSetMacro(InitialTransformParameters,ParametersType);
    itkGetConstMacro(InitialTransformParameters,ParametersType);

    itkSetObjectMacro(Transform,TransformType);
    TransformType * GetTypedTransform(void)
      {
      return m_Transform;
      }

    itkSetMacro(OptimizerScales,ScalesType);
    itkGetMacro(OptimizerScales,ScalesType);
    itkSetMacro(OptimizerNumberOfIterations,unsigned int);
    itkGetMacro(OptimizerNumberOfIterations,unsigned int);

    itkSetObjectMacro(FixedLandmarkSet,LandmarkSetType);
    itkGetObjectMacro(FixedLandmarkSet,LandmarkSetType);

    itkSetObjectMacro(MovingLandmarkSet,LandmarkSetType);
    itkGetObjectMacro(MovingLandmarkSet,LandmarkSetType);

    PointType GetMovingCenter() {return m_movingCenter;}

    void StartRegistration();

  protected:

    virtual void PrintSelf( std::ostream &os, itk::Indent indent ) const;

    LandmarkRegistrator();

    ~LandmarkRegistrator();

    void CopyLandmarkSet( LandmarkSetType::Pointer source,
                          LandmarkSetType::Pointer dest ) const;

    LandmarkSetType::Pointer    m_FixedLandmarkSet;
    LandmarkSetType::Pointer    m_MovingLandmarkSet;
    MetricType::Pointer         m_Metric;
    TransformType::Pointer      m_Transform;
    ParametersType              m_InitialTransformParameters;
    NormalGeneratorType::Pointer    m_Generator;
    OptimizerType::Pointer      m_Optimizer;
    ScalesType                  m_OptimizerScales;
    unsigned int                m_OptimizerNumberOfIterations;
    itk::Point<double, 3>       m_movingCenter;

  private:

    LandmarkRegistrator(const Self&);  //purposely not implemented
    void operator=(const Self&);  //purposely not implemented
  };

}; // end namespace udg

#endif //__LandmarkRegistrator_h
