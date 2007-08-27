#include "isolatedconnected.h"
#include "volume.h"
#include <iostream.h>
#include "logging.h"
namespace udg {

///Constructor
IsolatedConnected::IsolatedConnected()
{

    m_inputVolume       = new udg::Volume();
    m_segmentedImage    = VolumeImageType::New();
        
    m_casterInput       = CastingFilterTypeIn::New();
    m_casterOutput      = CastingFilterTypeOut::New();
    
    m_smoothing         = CurvatureFlowImageFilterType::New();
    m_isolated         = IsolatedConnectedFilterType::New();  



}

///Destructor
IsolatedConnected::~IsolatedConnected()
{
}

/// Afegeix una llavor al vector de llavors, l'atribut m_seedsVector
void IsolatedConnected::addSeed( int x, int y, int z )
{
    
    VolumeImageIndexType seed;
    seed[0] = x;
    seed[1] = y;
    seed[2] = z;
    m_seedsVector.push_back( seed );
    
}
///Afageix la segona llavor dins el vector de llavors 2, m_seedsVector2
void IsolatedConnected::addSeed2( int x2, int y2, int z2 )
{
    
    VolumeImageIndexType seed;
    seed[0] = x2;
    seed[1] = y2;
    seed[2] = z2;
    m_seedsVector2.push_back( seed );
    
}


/// Carrega el volum a tractar
void IsolatedConnected::setInputVolume( Volume* v )
{
    m_inputVolume = v;
}



/** Carrega els par�metres necessaris per configurar el filtre de suavitzaci�
   ( iteracions i TimeStep )*/
void IsolatedConnected::setSmoothingParameters( )
{
    m_smoothing->SetNumberOfIterations( 2 );
    m_smoothing->SetTimeStep( 0.125 );
}


/** Carrega els par�metres necessaris per configurar el filtre de segmentaci�
   ( lower )*/
void IsolatedConnected::setIsolatedParameters( int lower )
{
    m_isolated->SetLower( lower );
    m_isolated->SetReplaceValue( 255 );
    m_isolated->SetSeed1( m_seedsVector[0] );
    m_isolated->SetSeed2( m_seedsVector2[0] );
    
    for (unsigned int i=1; i<m_seedsVector.size(); i++)
    {
	m_isolated->AddSeed1( m_seedsVector[i] );
    }
   
    m_seedsVector.clear();
}


/// Aplica el metode de segmentaci�
bool IsolatedConnected::applyMethod()
{
CurvatureFlowImageFilterType::Pointer smooth = CurvatureFlowImageFilterType::New();

    
    /// Crea la PIPELINE
    m_casterInput->SetInput( m_inputVolume->getItkData() );
    m_smoothing->SetInput( m_casterInput->GetOutput() );
    m_isolated->SetInput( m_smoothing->GetOutput() );
    //smooth->SetInput(m_isolated->GetOutput() );
    //m_casterOutput->SetInput( smooth->GetOutput() );
    m_casterOutput->SetInput(m_isolated->GetOutput() );

        
    /// Executa la segmentaci�
    //std::cout << "****START: SEGMENTACIO ISOLATED CONNECTED ****** " << std::endl;
    //INFO_LOG("****START: SEGMENTACIO ISOLATED CONNECTED ****** ");
    try
    {
        m_casterOutput->Update();
    }
    catch( itk::ExceptionObject & e )
    {
       // std::cerr << "ERROR: No ha anat b� la segmentaci�. (IsolatedConnected)" << std::endl;
       // std::cerr << "Exception caught! " << std::endl;
       // std::cerr << e << std::endl;
        return false;
    }
    //std::cout << "****END: SEGMENTACIO ISOLATED CONNECTED****** " << std::endl;
    // INFO_LOG("****END: SEGMENTACIO ISOLATED CONNECTED****** ");
    ///-------------------- Resultats ------------------
    
    m_segmentedImage = m_casterOutput->GetOutput();
    m_segmentedImage->Update();
  
    return true; 
}


/// Retorna el volum segmentat
udg::Volume* IsolatedConnected::getSegmentedVolume()
{
    udg::Volume* volume = new udg::Volume();
    volume->setData( m_segmentedImage );
    return volume;
}






};
