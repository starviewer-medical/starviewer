/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "volumecalculator.h"

#include <iostream>
#include "itkVolumeCalculatorImageFilter.h"

namespace udg {

VolumeCalculator::VolumeCalculator()
{
    m_mask = 0;
    m_insideValueSet = false;
}


VolumeCalculator::~VolumeCalculator()
{
}


void VolumeCalculator::setInput( Volume * mask )
{
    m_mask = mask;
}

void VolumeCalculator::setInsideValue( int value )
{
    m_insideValue = value;
    m_insideValueSet = true;
}

double VolumeCalculator::getVolume()
{
    if ( !m_mask )
    {
        std::cerr << "Error: L'input no està definit" << std::endl;
        return 0.0;
    }

    if ( !m_insideValueSet ) calculateInsideValue();

    typedef itk::VolumeCalculatorImageFilter< Volume::ItkImageType > VolumeCalcFilterType;
    VolumeCalcFilterType::Pointer volumeCalc = VolumeCalcFilterType::New();
    volumeCalc->SetInput( m_mask->getItkData() );
    volumeCalc->SetInsideValue( m_insideValue );

    try
    {
        volumeCalc->Update();
    }
    catch( itk::ExceptionObject & excep )
    {
        std::cerr << "Exception caught !" << std::endl;
        std::cerr << excep << std::endl;
    }

    return volumeCalc->GetVolume();
}

void VolumeCalculator::calculateInsideValue()
{
    Volume::ItkImageTypePointer maskItk = m_mask->getItkData();
    itk::ImageRegionIterator< Volume::ItkImageType > it( maskItk, maskItk->GetLargestPossibleRegion() );
    it.GoToBegin();

    if ( it.Get() == 0 )
        m_insideValue = 255;
    else
        m_insideValue = 0;

    m_insideValueSet = true;
}


}
