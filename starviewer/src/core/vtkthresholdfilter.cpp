/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 **************************************************************************/

#include "vtkthresholdfilter.h"
#include "vtkimagethreshold.h"
#include "volume.h"


namespace udg {

VtkThresholdFilter::VtkThresholdFilter( )
: Filter( )
{
    imageThreshold = vtkImageThreshold::New();

}

VtkThresholdFilter::~VtkThresholdFilter()
{
    imageThreshold->Delete();
}

void VtkThresholdFilter::thresholdByUpper (float thresh)
{
    imageThreshold->ThresholdByUpper(thresh);
}
void VtkThresholdFilter::thresholdByLower (float thresh)
{
    imageThreshold->ThresholdByLower(thresh);
}
void VtkThresholdFilter::thresholdBetween (float lower, float upper)
{
    imageThreshold->ThresholdBetween(lower,upper);
}
void VtkThresholdFilter::setInValue (double val)
{
    imageThreshold->SetInValue(val);
}
void VtkThresholdFilter::setOutValue (double val)
{
    imageThreshold->SetOutValue(val);
}

void VtkThresholdFilter::update()
{
    imageThreshold->Update();
    m_outputVolume = new Volume();
    m_outputVolume->setImages( m_inputVolume->getImages() );
    m_outputVolume->setData( imageThreshold->GetOutput() );    

}
void VtkThresholdFilter::setInput(Volume * input)
{
    imageThreshold->SetInput(input->getVtkData());
    m_inputVolume = input;
}

}