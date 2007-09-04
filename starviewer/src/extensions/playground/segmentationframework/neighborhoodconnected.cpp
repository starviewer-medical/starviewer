#include "neighborhoodconnected.h"
#include "volume.h"
#include <iostream.h>
#include "logging.h"
namespace udg {

///Constructor
NeighborhoodConnected::NeighborhoodConnected()
{

    m_inputVolume       = new udg::Volume();
    m_segmentedImage    = VolumeImageType::New();

    m_casterInput       = CastingFilterTypeIn::New();
    m_casterOutput      = CastingFilterTypeOut::New();

    m_smoothing         = CurvatureFlowImageFilterType::New();
    m_connected         = NeighborhoodConnectedImageFilterType::New();



}

///Destructor
NeighborhoodConnected::~NeighborhoodConnected()
{
}

/// Afegeix una llavor al vector de llavors, l'atribut m_seedsVector
void NeighborhoodConnected::addSeed( int x, int y, int z )
{

    //std::cout<<"llavors a addSedd "<< x <<std::endl;
    VolumeImageIndexType seed;
    seed[0] = x;
    seed[1] = y;
    seed[2] = z;
    m_seedsVector.push_back( seed );

}


/// Carrega el volum a tractar
void NeighborhoodConnected::setInputVolume( Volume* v )
{
    m_inputVolume = v;
}


/** Carrega els par�metres necessaris per configurar el filtre de suavitzaci�
   ( iteracions i TimeStep )*/
void NeighborhoodConnected::setSmoothingParameters( )
{
    m_smoothing->SetNumberOfIterations( 2 );
    m_smoothing->SetTimeStep( 0.125 );
}


/** Carrega els par�metres necessaris per configurar el filtre de segmentaci�
   ( lower, upper, radi )*/
void NeighborhoodConnected::setNeighborhoodParameters( int lower, int upper, double radi )
{
    InternalImageType::SizeType   radius;

    m_connected->SetLower( lower );
    m_connected->SetUpper( upper );
    m_connected->SetReplaceValue( 255 );

    radius[0] = radi;  // eix X
    radius[1] = radi;  // eix Y
    radius[2] = radi;  // eox Z
    m_connected->SetRadius( radius );


    m_connected->SetSeed( m_seedsVector[0] );


    for (unsigned int i=1; i<m_seedsVector.size(); i++)
    {
	m_connected->AddSeed( m_seedsVector[i] );
    }

	m_seedsVector.clear();

}


/// Aplica el metode de segmentaci�
bool NeighborhoodConnected::applyMethod()
{
CurvatureFlowImageFilterType::Pointer smooth = CurvatureFlowImageFilterType::New();


    /// Crea la PIPELINE
    m_casterInput->SetInput( m_inputVolume->getItkData() );
    m_smoothing->SetInput( m_casterInput->GetOutput() );
    m_connected->SetInput( m_smoothing->GetOutput() );
    //smooth->SetInput(m_connected->GetOutput() );
    //m_casterOutput->SetInput( smooth->GetOutput() );
    m_casterOutput->SetInput(m_connected->GetOutput() );


    /// Executa la segmentaci�
   // std::cout << "****START: SEGMENTACIO NEIGHBORHOOD CONNECTED ****** " << std::endl;
    //INFO_LOG( "****START: SEGMENTACIO NEIGHBORHOOD CONNECTED ****** ");
    try
    {
        m_casterOutput->Update();
    }
    catch( itk::ExceptionObject & e )
    {
        //std::cerr << "ERROR: No ha anat b� la segmentaci�. (NeighborhoodConnected)" << std::endl;
        //std::cerr << "Exception caught! " << std::endl;
        //std::cerr << e << std::endl;
        return false;
    }
    //INFO_LOG( "****END: SEGMENTACIO NEIGHBORHOOD CONNECTED****** ");
    //std::cout << "****END: SEGMENTACIO NEIGHBORHOOD CONNECTED****** " << std::endl;

    ///-------------------- Resultats ------------------

    m_segmentedImage = m_casterOutput->GetOutput();
    m_segmentedImage->Update();

    return true;
}


/// Retorna el volum segmentat
udg::Volume* NeighborhoodConnected::getSegmentedVolume()
{
    udg::Volume* volume = new udg::Volume();
    volume->setData( m_segmentedImage );
    //TODO això es necessari perquè tingui la informació de la sèrie, estudis, pacient...
    volume->setImages( m_inputVolume->getImages() );
    return volume;
}






};
