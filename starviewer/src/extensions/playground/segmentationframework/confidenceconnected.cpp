#include "confidenceconnected.h"
#include "volume.h"
#include <iostream.h>
#include "logging.h"
namespace udg {

///Constructor
ConfidenceConnected::ConfidenceConnected()
{

    m_inputVolume       = new udg::Volume();
    m_segmentedImage    = VolumeImageType::New();
        
    m_casterInput       = CastingFilterTypeIn::New();
    m_casterOutput      = CastingFilterTypeOut::New();
    
 
    m_smoothing         = CurvatureFlowImageFilterType::New();
    m_confidence         = ConfidenceConnectedImageFilterType::New();  
  

}

///Destructor
ConfidenceConnected::~ConfidenceConnected()
{
}

/// Afegeix una llavor al vector de llavors, l'atribut m_seedsVector
void ConfidenceConnected::addSeed( int x, int y, int z )
{
    
    VolumeImageIndexType seed;
    seed[0] = x;
    seed[1] = y;
    seed[2] = z;
    m_seedsVector.push_back( seed );
    
}


/// Carrega el volum a tractar
void ConfidenceConnected::setInputVolume( Volume* v )
{
    m_inputVolume = v;
}


/** Carrega els par�metres necessaris per configurar el filtre de suavitzaci�
   ( iteracions i TimeStep )*/
void ConfidenceConnected::setSmoothingParameters( )
{
    m_smoothing->SetNumberOfIterations( 2 );
    m_smoothing->SetTimeStep( 0.125 );
}


/** Carrega els par�metres necessaris per configurar el filtre de segmentaci�
   ( radi, multi,itera)*/
void ConfidenceConnected::setConfidenceParameters( int radius, float multi, int itera )
{
    
    m_confidence->SetMultiplier( multi );
    m_confidence->SetNumberOfIterations( itera );
    m_confidence->SetInitialNeighborhoodRadius(radius);
    m_confidence->SetReplaceValue( 255 );
    m_confidence->SetSeed( m_seedsVector[0] );
    

    for (unsigned int i=1; i<m_seedsVector.size(); i++)
    {
	

        m_confidence->AddSeed( m_seedsVector[i] );
    }

	m_seedsVector.clear();
    
}


/// Aplica el metode de segmentaci�
bool ConfidenceConnected::applyMethod()
{
CurvatureFlowImageFilterType::Pointer smooth = CurvatureFlowImageFilterType::New();

    
    /// Crea la PIPELINE
    m_casterInput->SetInput( m_inputVolume->getItkData() );
    m_smoothing->SetInput( m_casterInput->GetOutput() );
    m_confidence->SetInput( m_smoothing->GetOutput() );
    //smooth->SetInput(m_confidence->GetOutput() );
    //m_casterOutput->SetInput( smooth->GetOutput() );
    m_casterOutput->SetInput( m_confidence->GetOutput() );


    /// Execuci� de la segmentaci�
    //INFO_LOG("****START: SEGMENTACIO CONFIDENCE CONNECTED****** ");
    //std::cout << "****START: SEGMENTACIO CONFIDENCE CONNECTED****** " << std::endl;
    try
    {
        m_casterOutput->Update();
    }
    catch( itk::ExceptionObject & e )
    {
        //std::cerr << "ERROR: No ha anat b� la segmentaci�. (ConfidenceConnected)" << std::endl;
        //std::cerr << "Exception caught! " << std::endl;
        //std::cerr << e << std::endl;
        return false;
    }
    // INFO_LOG("****END: SEGMENTACIO CONFIDENCE CONNECTED****** ");
    //std::cout << "****END: SEGMENTACIO CONFIDENCE CONNECTED****** " << std::endl;
     
    ///-------------------- Resultats ------------------
    
    m_segmentedImage = m_casterOutput->GetOutput();
    m_segmentedImage->Update();
  
    return true; 
}


/// Retorna el volum segmentat
udg::Volume* ConfidenceConnected::getSegmentedVolume()
{
    udg::Volume* volume = new udg::Volume();
    volume->setData( m_segmentedImage );
    //std::cout << "GetSegmented1 Ok " << std::endl;
    return volume;
}






};
