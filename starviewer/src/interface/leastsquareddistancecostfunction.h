/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gr�ics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGLEASTSQUAREDDISTANCECOSTFUNCTION_H
#define UDGLEASTSQUAREDDISTANCECOSTFUNCTION_H

#include <itkSingleValuedCostFunction.h>
#include <itkPoint.h>
#include <itkArray.h>
#include <itkVectorContainer.h>
#include <itkVersorRigid3DTransform.h>

namespace udg
{

/**
 *
 * This class calculate the sum of of the distances between two set of points.
 *
 */

  template< class TTransform = itk::VersorRigid3DTransform<double> >
  class LeastSquaredDistanceCostFunction
  : public itk::SingleValuedCostFunction
  {

  public:

    typedef LeastSquaredDistanceCostFunction      Self;
    typedef itk::SingleValuedCostFunction         Superclass;
    typedef itk::SmartPointer<Self>               Pointer;
    typedef itk::SmartPointer< const Self >       ConstPointer;

    typedef itk::Point<double,3>                  PointType;
    typedef itk::VectorContainer<int,PointType >  PointSetType;
    typedef PointSetType::Pointer                 PointSetPointer;

    typedef TTransform                            TransformType;
    typedef typename TransformType::Pointer       TransformPointer;

    typedef double                                MeasureType;

    itkNewMacro(Self);

    itkTypeMacro(LeastSquaredDistanceCostFunction,SingleValuedCostFunction);
  
    void SetFixedPointSet( PointSetPointer fixedPointSet );

    void SetMovingPointSet( PointSetPointer movingPointSet );

    void SetMovingCenter( PointType movingCenter );

    double GetValue( const ParametersType &parameters ) const;

    void GetDerivative( const ParametersType &parameters,
                        DerivativeType &derivative ) const;

    unsigned int GetNumberOfParameters(void) const;

  protected:

    bool                m_Valid;
    TransformPointer    m_Transform;

    PointSetPointer     m_FixedPointSet;
    PointSetPointer     m_MovingPointSet;

    PointType           m_MovingCenter;

    LeastSquaredDistanceCostFunction();

    virtual ~LeastSquaredDistanceCostFunction();

    virtual void PrintSelf( std::ostream &os, itk::Indent indent ) const;

  private:

   LeastSquaredDistanceCostFunction(const Self&);  //purposely not implemented
   void operator=(const Self&);                   //purposely not implemented

  };

} //end of namespace udg

#include "leastsquareddistancecostfunction.cpp"

#endif //_itkLeastSquaredDistanceCostFunction__h
