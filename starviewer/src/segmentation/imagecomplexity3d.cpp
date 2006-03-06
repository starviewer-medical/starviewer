/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGIMAGECOMPLEXITY3D_CPP
#define UDGIMAGECOMPLEXITY3D_CPP

#include "imagecomplexity3d.h"
#include "imagecomplexity3dnode.h"

// upper i lower bound
#include <itkMinimumMaximumImageCalculator.h>

// Pel bipartition
#include "imagecomplexity3dhistogram.h"
#include "mathtools.h" // logTwo, binaryEntropy
#include <iostream> // cout, cerr

namespace udg {

template <typename TInputImage>
ImageComplexity3D <TInputImage>::ImageComplexity3D()
{
    m_outputImage = 0;
    m_inputImage = 0;
    // valors per defecte dels paràmetres
    m_histogramBins = 256;
    m_uniformEntropy = 1.0;
    m_cut = Bipartition;
    
    m_splitStopCriteria = 2;
    m_splitCoefficient = 0.2;

    m_mergeStopCriteria = 0;
    m_mergeCoefficient = 6;

    m_imageHistogram = 0;
    m_clusteringMatrix = 0; // la matriu no apunta enlloc
        
    m_lastUpdate = 0;
    m_imageStatisticsFilter = 0;

}

template <typename TInputImage>
ImageComplexity3D <TInputImage>::~ImageComplexity3D()
{
}

template <typename TInputImage>
void ImageComplexity3D <TInputImage>
::setInputImage( TInputImage* inputImage )
{
    m_inputImage = inputImage;
    m_imageStatisticsFilter = StatisticsImageFilterType::New();
    m_imageStatisticsFilter->SetInput( m_inputImage );
    m_imageStatisticsFilter->Update();
}


template <typename TInputImage>
bool ImageComplexity3D <TInputImage>
::applyMethod()
{
    clean();
    bool ok = true;
    input();
    comput();
    output(); // desa la imatge, mostra estadístics
    return ok;
}

template <typename TInputImage>
void ImageComplexity3D <TInputImage>
::clean()
{
     
    m_splitLeaves.clear();
    while( m_splitCandidates.size() )
        m_splitCandidates.pop();
    
    m_mergeLeaves.clear();        
    m_mergeCandidates.clear();
    
    delete m_clusteringMatrix;
    m_clusteringMatrix = 0;
    
    delete m_imageHistogram;
    m_imageHistogram = 0;
    
    m_lastUpdate = 0;
}

template <typename TInputImage>
bool ImageComplexity3D <TInputImage>
::input()
{
    bool ok = true;
    
    std::cout << "Initial Parameters" << std::endl;
    std::cout << "-------------------------" << std::endl;
    std::cout << "Histogram Bins: " << m_histogramBins << std::endl;
    std::cout << "Uniform Entropy (UH): " << m_uniformEntropy << std::endl;
    std::cout << "Cut Type: " << m_cut << std::endl;
    std::cout << "Split Stop Criteria: ";
    switch( m_splitStopCriteria )
    {
    case ImageComplexity3D::NumberOfRegions: std::cout << "Number Of Regions" << std::endl; break;
    case ImageComplexity3D::NumberOfColours: std::cout << "Number Of Colours" << std::endl; break;
    case ImageComplexity3D::NormalizedMutualInformation: std::cout << "Normalized Mutual Information" << std::endl; break;
    case ImageComplexity3D::MutualInformation: std::cout << "Mutual Information" << std::endl; break;
    case ImageComplexity3D::MutualInformationRatio: std::cout << "Mutual Information Ratio" << std::endl; break;
    }
    std::cout << "Split Coefficient: " << m_splitCoefficient << std::endl;
    
    std::cout << "Merge Stop Criteria: "; 
    switch( m_mergeStopCriteria )
    {
    case ImageComplexity3D::NumberOfRegions: std::cout << "Number Of Regions" << std::endl; break;
    case ImageComplexity3D::NumberOfColours: std::cout << "Number Of Colours" << std::endl; break;
    case ImageComplexity3D::NormalizedMutualInformation: std::cout << "Normalized Mutual Information" << std::endl; break;
    case ImageComplexity3D::MutualInformation: std::cout << "Mutual Information" << std::endl; break;
    case ImageComplexity3D::MutualInformationRatio: std::cout << "Mutual Information Ratio" << std::endl; break;
    case ImageComplexity3D::MutualInformationPercent: std::cout << "Mutual Information Percent" << std::endl; break;
    }
    std::cout << "Merge Coefficient: " << m_mergeCoefficient << std::endl;

    return ok;
}

template <typename TInputImage>
void ImageComplexity3D <TInputImage>
::output()
{
    std::cout << std::endl << " >>>>>>>> Resultats Finals >>>>>>>>>>>" << std::endl;
    this->print();

}


template <typename TInputImage>
typename ImageComplexity3D <TInputImage>::OutputImageType*
ImageComplexity3D <TInputImage>::getOutputImage(int type)
{
    if( m_lastUpdate == type )
    {
        return m_outputImage;
    }
    else
    {
        update( type );
        return m_outputImage;
    }
}

template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::update(int type)
{
    // Preparem la imatge de sortida
    m_outputImage = OutputImageType::New();

    m_outputImage->SetRegions( m_inputImage->GetLargestPossibleRegion() );
    m_outputImage->SetSpacing( m_inputImage->GetSpacing() );
    m_outputImage->SetOrigin( m_inputImage->GetOrigin() );
    m_outputImage->Allocate();
    
    PixelType avg;
    SplitRegionListIteratorType it;
    RegionType r;
    switch( type )
    {
    case 1: // fa els clusters
    
        for( it = m_splitLeaves.begin(); it != m_splitLeaves.end(); it++ )
        {
            r = *it;                     
            avg = average( r.getItkRegion() );
            fill( r.getItkRegion(), avg );
        }
    
    break;

    case 2: // només contorns
        for( it = m_splitLeaves.begin(); it != m_splitLeaves.end(); it++ )
        {
            r = *it;                     
            contour( r.getItkRegion() );
        }
    
    break;
    
    case 3:  // clustering histograma. Agafarem el valor màxim d'aquell bin
        repaint();
    break;
    }
    
    m_lastUpdate = type;
    

}

template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::repaint()
{
    std::cout << "Repainting: " << std::endl;
  
    unsigned x, xMax;
    //double avg, minimum, maximum;
    unsigned area = 0;
    
    fillStruct *filling;
    filling = new fillStruct[ m_mergeLeaves.size()];
    int i = 0;
    std::cout << "repaint::[m_mergeLeaves.size()]" << m_mergeLeaves.size() << std::endl;
    // Obtenim els rangs de l'histograma i repintem la imatge amb cada color ( mitjana de l'histograma )
    for ( typename MergeRegionsList::iterator p = m_mergeLeaves.begin(); p != m_mergeLeaves.end(); p++ ) 
    {	  
        x = p->getX();
        xMax =  x + p->getWidth() - 1;
        ImageComplexity3DHistogram histogram( *m_imageHistogram, x, xMax );
        p->setProxyEntropy( filling[i].average = histogram.mean() ); // p->put(avg = h.mean(), 1);
        std::cout << "Average_____::" << filling[i].average << std::endl;
        p->setLocalProportion( filling[i].minimum = histogram.getMinimumBin(x) ); // p->put( minimum = h.getMinimumBin(x), 2 ); 
        p->setGlobalProportion( filling[i].maximum = histogram.getMaximumBin(xMax) ); //p->put(maximum = h.getMaximumBin(xMax), 3);
        if ( xMax == m_histogramBins ) 
            filling[i].maximum += 1; // Included in last.
        
        i++;
      
        //area += fill( minimum, maximum, avg ); 
    }	
    // repintem amb cada nou color la imatge	  
    area += fill( filling ); 
    if ( area != m_size ) 
        std::cerr << "ImageComplexity3D::repaint() >> Inconsistent total area" << std::endl;  
    
    std::cout << "OK" << std::endl;
    
  
/*
void repaint()
{
  fprintf(out.std, "\nRepainting: ");
  unsigned x, xMax;
  REAL avg, minimum, maximum;
  unsigned area = 0;
  
  for (REGION::SET::iterator p = shared.leaves.begin();	  p != shared.leaves.end();	  p++) {	  
	       x = p->getX();
		  xMax =  x + p->getWidth() - 1;
		  HISTOGRAM h(shared.h, x, xMax);
		  p->put(avg = h.mean(), 1);
		  p->put(minimum = h.getMinimumBin(x), 2);
		  p->put(maximum = h.getMaximumBin(xMax), 3);
		  if (xMax == parameters.histogramBins) maximum += 1; // Included in last.
            area += fill(minimum, maximum, avg); }		  

  if (area != shared.size) messageUser(PROGRAM, "Inconsistent total area", "ICMain::repaint");  
  fprintf(out.std, "OK\n");
}
    */
}

template <typename TInputImage>
unsigned 
ImageComplexity3D <TInputImage>::fill( fillStruct* filling )
{
// omplim la imatge amb un valor si es troba dins d'un determinat rang
    int i;
    bool found;
    
    unsigned n = 0;
    OutputImageIteratorType outputIt( m_outputImage , m_outputImage->GetLargestPossibleRegion() );
    outputIt.GoToBegin();
    
    InputImageIteratorType inputIt( m_inputImage , m_inputImage->GetLargestPossibleRegion() );
    inputIt.GoToBegin();
    
    while( ! outputIt.IsAtEnd() )
    {
    // per cada pixel de la imatge original mirem per quin dels nous colors hem de substituir
        i = 0; found = false;
        while( i < m_mergeLeaves.size() && !found )
        {
            if( filling[i].minimum <= inputIt.Get() && inputIt.Get() < filling[i].maximum )
            {
                found = true;
                outputIt.Set( filling[i].average );
                n++;
            }
            i++;
        }

        ++outputIt;
        ++inputIt;
    }
    	
    return n;								    
}
  /*
template <typename TInputImage>
unsigned 
ImageComplexity3D <TInputImage>::fill(const double minimum, const double maximum, const double data)
{
// omplim la imatge amb un valor si es troba dins d'un determinat rang
    
    unsigned n = 0;
    OutputImageIteratorType outputIt( m_outputImage , m_outputImage->GetLargestPossibleRegion() );
    outputIt.GoToBegin();
    
    InputImageIteratorType inputIt( m_inputImage , m_inputImage->GetLargestPossibleRegion() );
    inputIt.GoToBegin();
    
    while( ! outputIt.IsAtEnd() )
    {
        if( minimum <= inputIt.Get() && inputIt.Get() < maximum )
        {
        //std::cout << "putting data::" << data << std::endl;
            outputIt.Set( data );
            n++;
        }
            
        ++outputIt;
        ++inputIt;
    }
    	
    return n;								
}
*/    
/*
    unsigned n = 0,k; 
    for (unsigned j = 0;  j < shared.height;   j++) 
        for (unsigned i = 0;  i < shared.width;  i++) 
        {
            k = j * shared.width + i;
            if (minimum <= shared.image[k] && shared.image[k] < maximum) 
            {
                n++;
                shared.image[k] = data; 
            }
        }
    return n;								    
*/


template <typename TInputImage>
typename ImageComplexity3D <TInputImage>::PixelType
ImageComplexity3D <TInputImage>::average( ItkRegionType itkRegion )
{

    // Checkejar que la regió està contenida a la imatge ( no hauria de fallar )
    if ( ! m_inputImage->GetRequestedRegion().IsInside( itkRegion ) )
    {
        std::cerr << std::endl << "Error[AVERAGE]" << std::endl;
        std::cerr << "The itkRegion " << itkRegion << " is not contained within the input image itkRegion "
                << m_inputImage->GetRequestedRegion() << std::endl << std::flush;
        return 0;
    }
    
    InputImageIteratorType it( m_inputImage , itkRegion );
    PixelType sum = 0;
     
    it.GoToBegin();
    while( ! it.IsAtEnd() )
    {
        sum += it.Get();
        ++it; 
    }

    return static_cast< PixelType >( sum / ( itkRegion.GetSize()[0] * itkRegion.GetSize()[1] * itkRegion.GetSize()[2] ) );
    
}


template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::fill( ItkRegionType itkRegion, PixelType data )
{

    // Checkejar que la regió està contenida a la imatge ( no hauria de fallar )
    if ( ! m_outputImage->GetRequestedRegion().IsInside( itkRegion ) )
    {
        std::cerr << std::endl << "Error[FILL]" << std::endl;
        std::cerr << "The itkRegion " << itkRegion << "is not contained within the output image itkRegion "
                << m_outputImage->GetRequestedRegion() << " so not FILLING the area at all" << std::endl << std::flush;
        return;
    }
    OutputImageIteratorType it( m_outputImage , itkRegion );
    it.GoToBegin();
    
    while( ! it.IsAtEnd() )
    {
       it.Set( data );
        ++it;
    }
    
}


template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::contour( ItkRegionType itkRegion )
{
    
    // Checkejar que la regió està contenida a la imatge ( no hauria de fallar )
    if ( ! m_outputImage->GetRequestedRegion().IsInside( itkRegion ) )
    {
        std::cerr << std::endl << "Error[CONTOUR]" << std::endl;
        std::cerr << "The itkRegion " << itkRegion << "is not contained within the output image itkRegion "
                << m_outputImage->GetRequestedRegion() << " so not FILLING the area at all" << std::endl << std::flush;
        return;
    }
//     typename StatisticsImageFilterType::Pointer imageStatistics = StatisticsImageFilterType::New();
//     imageStatistics->SetInput( m_inputImage );
//     imageStatistics->Update();
//     std::cout << "Màxim!::" << m_imageStatisticsFilter->GetMaximum() << "Mínim!::" << m_imageStatisticsFilter->GetMinimum() <<  "Mitjana!::" << m_imageStatisticsFilter->GetMean() << std::endl;
//     
    InputImageIteratorType inputIt( m_inputImage , itkRegion );
    inputIt.GoToBegin();
    
    OutputImageIteratorWithIndexType it( m_outputImage , itkRegion );
    it.GoToBegin();
    OutputImageIndexType index;
    int lowerBound[3], upperBound[3];
    lowerBound[0] = itkRegion.GetIndex()[0];
    lowerBound[1] = itkRegion.GetIndex()[1];
    lowerBound[2] = itkRegion.GetIndex()[2];
    upperBound[0] = lowerBound[0] + itkRegion.GetSize()[0] - 1;
    upperBound[1] = lowerBound[1] + itkRegion.GetSize()[1] - 1;
    upperBound[2] = lowerBound[2] + itkRegion.GetSize()[2] - 1;
    
    while( ! it.IsAtEnd() )
    {
        index = it.GetIndex();
        
        if( index[2] == lowerBound[2] || index[1] == lowerBound[1] || index[0] == lowerBound[0] )
        {
            it.Set( /*2000*/ m_imageStatisticsFilter->GetMaximum() ); // el valor aquest podria canviar
        }
        else
        {
            it.Set( inputIt.Get() );
        }
        ++it;
        ++inputIt;
    }
    
}


template <typename TInputImage>
void ImageComplexity3D <TInputImage>
::initSplitData()
{
    // crear l'histograma inicial, insertar la regió inicial ( tota la imatge ) a la llista
    // fer els càlculs inicials de teoria de la info
    // i inicialitzar els paràmetres inicials

    if( ! m_inputImage )
    {
        std::cerr << "ImageComplexity3D::initSplitData() >> No hi ha assignada cap imatge d'entrada" << std::endl;
    }
    else
    {
        RegionType root;
        typename InputImageType::RegionType imageRegion;
        imageRegion = m_inputImage->GetLargestPossibleRegion();
        root.setOrigin( imageRegion.GetIndex() );
        root.setSize( imageRegion.GetSize() );
        
        typedef itk::MinimumMaximumImageCalculator< InputImageType > MinMaxCalculatorType;
        typename MinMaxCalculatorType::Pointer minMaxCalculator = MinMaxCalculatorType::New();
        minMaxCalculator->SetImage( m_inputImage );
        minMaxCalculator->Compute();

        
        m_histogramMin =  minMaxCalculator->GetMinimum() ; 
        m_histogramMax =  minMaxCalculator->GetMaximum() ; 


        m_imageHistogram = new ImageComplexity3DHistogram ( m_histogramBins, m_histogramMin, m_histogramMax );
        
        InputImageIteratorType imageIterator( m_inputImage, m_inputImage->GetLargestPossibleRegion() );
        imageIterator.GoToBegin();
        while( ! imageIterator.IsAtEnd() )
        {

            m_imageHistogram->in( imageIterator.Get() );
            ++imageIterator;
        }
        
        //root.setHistogram( histogram );
        root.setHistogram( *m_imageHistogram );

        m_imageEntropy = root.getEntropy();

        root.setLocalProportion( 1 );
        root.setGlobalProportion( 1 );

        insert( root );

        m_numberOfRegions = 1;  //<--------- Init.
        m_mutualInformationImageRegions = 0;  //<--------- Init.
        m_conditionalEntropyImageRegions = m_imageEntropy;  //<--------- Init.
        m_regionsEntropy = 0;  //<--------- Init.
        m_jointEntropyImageRegions = m_imageEntropy;  // imageEntropy + hR - miCR <--------- Init.
        m_normalizedMutualInformation = 0;  //<--------- Init.

        if( m_splitStopCriteria != 1 )
            m_miMin = 0;
        else // desigualtat de Fano
        {
            m_miMin = m_imageEntropy - MathTools::binaryEntropy( m_splitCoefficient ) - m_splitCoefficient * MathTools::logTwo( m_histogramBins - 1 );
            std::cout << std::endl << "Computing of Imin = H(X)-H(Pe)-Pe*log2(B-1) == " << m_imageEntropy << " - " << MathTools::binaryEntropy( m_splitCoefficient ) << " - " << m_splitCoefficient << " * log2(" << m_histogramBins - 1 << " ) == " << m_miMin << std::endl;
            
        }
           

        if ( m_miMin < 0)
        {
            std::cerr<< "ImageComplexity3D::initSplitData >> The probability error for MI (negative) is excesive: I minimum is zero" << std::endl;
            m_miMin = 0;
        }

        m_proportion = computSplitProportion(); //<--------- Init

    }
}

template <typename TInputImage>
void ImageComplexity3D <TInputImage>
::initMergeData()
{
   //REGION* region = loadFile();  // Aquí es carreguen les dades, és el conjunt de regions finals
   // el que fa loadFile bàsicament és
   /*
   REGION* region = new REGION[shared.regions]; 
  for (unsigned i = 0;
       i < shared.regions;
	  i++) {
	       region[i] = set.front();
		  set.pop_front(); }
  és a dir que posem totes les regions en un array de regions. Aquest pas el podem obviar 
  i aquest recorregut pel conjunt de regions el farem al passar les dades dels histogrames a la matriu de clustering
  */
 // set == m_splitLeaves
 
 /* 
 // aixo sembla que es podria obviar, altres tipus d'inits que no ens afecte directament
  if (parameters.avgValues) {
       shared.image = new REAL[shared.size]; //<--------- Init.
       updateAvg(region); }
  else {
       BMI image(shared.imageName);
       if (shared.width != image.getWidth()) messageUser(PRECONDITION, "Inconsistent width-1", "ICMain::setData");
       if (shared.height != image.getHeight()) messageUser(PRECONDITION, "Inconsistent height-1", "ICMain::setData");
       shared.image = image.filterData(shared.filter); }  //<--------- Init.
*/

/*
  // Update regions.
  //HISTOGRAM h(parameters.histogramBins, parameters.histogramMin, parameters.histogramMax);
  fprintf(out.std, "Loading %u segments-2D: ", shared.regions);
// shared.region és un array!!! de tots els histogrames de les regions ( es on farem els clusterings ). Les mides seran el nombre de regions per el nombre de bins dels histogrames
*/


    ImageComplexity3DHistogram histogram( m_histogramBins , m_histogramMin, m_histogramMax );
    m_clusteringMatrix = new unsigned[ m_numberOfRegions * m_histogramBins ];
    unsigned total = 0, frequency;
    
    SplitRegionListIteratorType it;
    RegionType r;
    // recorrem les regions i omplim la matriu dels histogrames
    unsigned i = 0;
    for( it = m_splitLeaves.begin(); it != m_splitLeaves.end(); it++ )
    {
        r = *it;                     
        histogram = r.getHistogram();
        for( unsigned bin = 0; bin < m_histogramBins; bin++ )        
        {            
            m_clusteringMatrix[ i*m_histogramBins + bin ] = frequency = histogram.get( bin+1 );
            total += frequency;
        }
        i++;
    }

/*
  shared.region = new unsigned[shared.regions * parameters.histogramBins]; 
  unsigned total = 0,f;
  // el bucle el que fa és recalcular els histogrames ( en el nostre cas no caldrà , ja estan calculats )
  // i llavors els transfereix a la matriu
  for (unsigned i = 0; i < shared.regions; i++) 
  {
      region[i].put(h);
      region[i].in(shared.image, shared.width);
      h = region[i].getHistogram();
      for (unsigned bin = 0; bin < parameters.histogramBins; bin++) 
      {
          shared.region[i * parameters.histogramBins + bin] = f = h.get(bin + 1); //<--------- Init.
          total += f; 
      }
  }
  */
  
  /*
  Comprovacions de correctesa,,, ens les podem saltar ja que se supisa que tot aixo ja ha d'estar be			  
  if (total != shared.size) messageUser(PROGRAM, "Inconsistent histogram regions", "ICMain::setData");
  if (parameters.avgValues && !parameters.avgRepaint) {
       delete [] shared.image;
       BMI image(shared.imageName);
       if (shared.width != image.getWidth()) messageUser(PRECONDITION, "Inconsistent width-2", "ICMain::setData");
       if (shared.height != image.getHeight()) messageUser(PRECONDITION, "Inconsistent height-2", "ICMain::setData");
       shared.image = image.filterData(shared.filter); }  //<--------- Init.
  else if (!parameters.avgValues && parameters.avgRepaint) {
       updateAvg(region); }  //<--------- Init.
  
  delete [] region;  // From loadFile().
  fprintf(out.std, "OK\n");
  */
  
  /*
  // aquí tindríem la regió origen, es a dir, tota la imatge. Aquesta ja la hem calculat per tant aquest pas també es pot obviar
  // Root.
  REGION root(1, 1, shared.width, shared.height);
  // Region Data: 1 = Region Entropy => Mean.
  //              2 = 0 => Minimum range.
  //              3 = Global proportion => Maximum range.
  //              4 = Area = frequency (more precission) => Area.
  root.put(h);
  root.in(shared.image, shared.width);
  shared.h = root.getHistogram(); //<--------- Init.
  root.display();
  shared.h.displayRows(divReal(80.0, shared.h.maximum(), true, 1));
*/
    typename InputImageType::RegionType imageRegion;
    imageRegion = m_inputImage->GetLargestPossibleRegion();
        
// En aquesta fase de l'algorisme, el sentit que tenien les regions en la fase d'split és diferent
// podem aprofitar la classe ImageComplexity3DRegion, però podríem crear també una altre estructura més senzilla
// ara les regions, es refereixen als agrupaments que fem en la matriu de clustering, per tant les regions són 2D
// la nova classe/estructura la podríem anomenar Cluster, per diferenciar
// Això afectaria a l'estructura MergeNode 
        
// Candidats
    MergeNode node;
    node.region.setX( 0 );
    node.region.setY( 0 ); // també afegir les Z???
    node.region.setZ( 0 );
    node.region.setSize( 1 , 1 , 1 ); // exactament no sabem perquè fem això
    // la mida significa que només agafem un sol bin per separat sense juntar al principi
    node.mutualInformation = -1;
    HN hn;
    double entropyR = 0;
    m_size = imageRegion.GetSize()[0] * imageRegion.GetSize()[1] * imageRegion.GetSize()[2];
    // Tindrem tants nodes com m_histogramBins
    // cadascun representa l'entropia d'un determinat bin per totes les regions, és a dir, si aquell bin està més o menys 'dispers' en totes les regions. 
    for (unsigned i = 1;  i <= m_histogramBins; i++) 
    {
        hn = mergeEntropy(i, i);
        node.region.setX( i );
        node.region.setProxyEntropy( hn.entropy );
        node.region.setGlobalProportion( double(hn.area) / m_size ); 
        node.region.setArea( hn.area ); 
        m_mergeCandidates.push_back( node );
        entropyR -= node.region.getGlobalProportion() * MathTools::logTwo( node.region.getGlobalProportion() ); 
    }
/*
  // Candidates.
  NODE node; // aquesta estructura de NODE és diferent. Aquí guardem dues regions ( region i fusion ) i la info mútua
  node.region.putY(0); // ????
  node.region.putWidth(1); // ????
  node.region.putHeight(1);// ??
  node.mi = -1; // init la mi

  HN hn;    // aquesta struct es l'entropia + n
  REAL entropyR = 0;
  for (unsigned i = 1;  i <= parameters.histogramBins; i++) 
  {
	       hn = entropy(i, i);
	       node.region.putX(i);
            node.region.put(hn.entropy, 1); // 1 :: region entropy --> mean
            node.region.put(double(hn.n) / shared.size, 3); // 3 :: Global Proportion => MAX RANGE
            node.region.put(hn.n, 4); // 4:: Area = frequency
		  shared.candidates.push_back(node);
		  entropyR -= node.region.getData(3) * logTwo(node.region.getData(3)); 
  }
  */
  MergeNodesListIteratorType to = m_mergeCandidates.begin();
  MergeNodesListIteratorType from = to;
  // per cada node calculem les corresponents fusions dels bins contigus
  for( from++; from != m_mergeCandidates.end(); fusion( from++, to++) );
  
  /*
  // Aquí shared.candidates és list<NODE> i diferent a l'anterior de l'split(que era una priority_queue)
  // parameters.histogramBins >= 2.
  list<NODE>::iterator to = shared.candidates.begin(); 
  list<NODE>::iterator from = to;
  for (from++;
       from != shared.candidates.end();
	  fusion(from++, to++));
*/

// calculem l'entropia de tota la matriu de clustering 
    hn = mergeEntropy( 1, m_histogramBins );
    if ( hn.area != m_size ) 
        std::cerr << "ImageComplexity3D::initMergeData >> Inconsistent histogram global" << std::endl;
        
    m_merge_n = m_histogramBins;  //<--------- Init.
    m_merge_hC = hn.entropy; //<--------- Init.
    m_merge_hcCR = mergeEntropyMinimum();  //<--------- Init.
    m_merge_miCR = m_merge_hC - m_merge_hcCR;  //<--------- Init.
    m_merge_miCRMax = m_merge_miCR;  //<--------- Init.
    m_merge_hR = entropyR;  //<--------- Init.
    m_merge_hjCR = m_merge_hR + m_merge_hcCR;  // hC + hR - miCR <--------- Init.
    m_merge_pih = MathTools::divReal( m_merge_miCR, m_merge_hjCR );  //<--------- Init.
    
    m_miMin = ( m_mergeStopCriteria != 1) ? //<--------- Init.
                0 :
                m_merge_hC - MathTools::binaryEntropy( m_mergeCoefficient ) 
                - m_mergeCoefficient * MathTools::logTwo( m_histogramBins - 1 );
    if ( m_miMin < 0) 
    {
        std::cout << "The probability error for MI (negative) is excesive: I minimum is zero" << std::endl;
        m_miMin = 0; 
    }
    
    m_proportion = computMergeProportion(); //<--------- Init
/*
// inicialitzar mesures de la teoria de la info 
  // IT. 
  hn = entropy(1, parameters.histogramBins);
  if (hn.n != shared.size) messageUser(PROGRAM, "Inconsistent histogram global", "ICMain::setData");
  shared.it.n = parameters.histogramBins;  //<--------- Init.
  shared.it.hC = hn.entropy; //<--------- Init.
  shared.it.hcCR = entropyMinimum();  //<--------- Init.
  shared.it.miCR = shared.it.hC - shared.it.hcCR;  //<--------- Init.
  shared.it.miCRMax = shared.it.miCR;  //<--------- Init.
  shared.it.hR = entropyR;  //<--------- Init.
  shared.it.hjCR = shared.it.hR + shared.it.hcCR;  // hC + hR - miCR <--------- Init.
  shared.it.pih = divReal(shared.it.miCR, shared.it.hjCR);  //<--------- Init.

  shared.miMin = (parameters.test != 1) ? //<--------- Init.
			  0 :
			  shared.it.hC - binaryEntropy(parameters.coefficient) 
			  - parameters.coefficient * logTwo(parameters.histogramBins - 1);
  if (shared.miMin < 0) {
       fprintf(out.std, "The probability error for MI (negative) is excesive: I minimum is zero.\n");
       shared.miMin = 0; }

  shared.proportion = computProportion(); //<--------- Init
  shared.time = 0;  //<--------- Init.
*/


/*
// inicialitzar la mesura de test

  fprintf(out.std, "\nThe equation is: H(C) =    I(C;R) +    H(C|R)      I-Max %% I-Max       (Goal)\n");
  REAL goal = 0;
  switch (parameters.test) {
       case 0: goal = parameters.coefficient; break;
	  case 1: goal = shared.miMin; break;
	  case 2: goal = parameters.coefficient * shared.it.hC; break;
	  case 3: goal = parameters.coefficient * shared.it.miCRMax; break;
	  case 4: goal = parameters.coefficient; break;
	  case 5: goal = parameters.coefficient; break;
       default: messageUser(PROGRAM, "Option impossible", "ICMain::setData"); } 
  fprintf(out.std, "            %9.6f = %9.6f + %9.6f  %9.6f  %6.2f  (%9.6f)\n\n", 
          shared.it.hC, shared.it.miCR, shared.it.hcCR, shared.it.miCRMax, divReal(shared.it.miCRMax * 100, shared.it.hcCR), goal);*/
}

template <typename TInputImage>
unsigned
ImageComplexity3D <TInputImage>::mergeArea(const unsigned i , const unsigned from, const unsigned to)
{
// i: ens indica que estem accedint a l'histograma de la regió #i
// from, to: ens indica el rang que volem abarcar d'aquest histograma, és a dir, els clusters veïns ( o contigus)
// n: és la suma de les freqüències entre from i to
    unsigned n = 0;
    for ( unsigned bin = from - 1; bin < to; bin++ ) 
            n += m_clusteringMatrix[ i * m_histogramBins + bin ];

    return n;
}

template <typename TInputImage>
typename ImageComplexity3D <TInputImage>::HN
ImageComplexity3D <TInputImage>::mergeEntropy( const unsigned from, const unsigned to )
{
// Calculem l'entropia d'un mateix rang de bins (from..to), per totes les regions
// el que fa és calcular l'àrea ( suma de frqüències ) d'un rang de bins per cada regió
// d'aquesta manera podrem saber el guany d'IM que obtenim al juntar certs bins de l'histograma i com afecta això a totes les regions

    // Use (-1/N)Sum(N_i Log(N_i)) + Log(N).
    unsigned frequency;
    HN hn;
    
    hn.entropy = 0;
    hn.area = 0;
    for ( unsigned i = 0; i < m_numberOfRegions; i++) 
    {
        frequency = mergeArea(i, from, to);  
        if ( frequency ) 
        {
            hn.entropy += frequency * MathTools::logTwo( frequency );
            hn.area += frequency; 
        }
    }
    
    hn.entropy = ( hn.area ) ? (- hn.entropy / hn.area) + MathTools::logTwo( hn.area ) : 0;
    
    return hn;
}

template <typename TInputImage>
double
ImageComplexity3D <TInputImage>::mergeEntropyMinimum(  )
{
    HN hn, hni;
	
    hn.entropy = 0;
    hn.area = 0;
    for (unsigned i = 1; i <= m_histogramBins; i++) 
    {
        hni = mergeEntropy(i, i);
        hn.entropy += hni.entropy * hni.area / m_size;
        hn.area += hni.area;  
    }
    
    if (hn.area != m_size) std::cerr << "ImageComplexity3D::mergeEntropyMinimum >> Inconsistent total" << std::endl;
    
    return hn.entropy;
}

template <typename TInputImage>
void 
ImageComplexity3D <TInputImage>::fusion( MergeNodesListIteratorType from , MergeNodesListIteratorType to )
{
    // Pre: control in call.
    // Pre: Entropy, local proportion, global proportion and area in region data (this order).
    // Put new data region information.
    // Put the MI global gain in node to.
    
    to->fusion = to->region; // regió i fusio son = inicialment
    unsigned width = to->region.getWidth() + from->region.getWidth(); // l'amplada = suma d'amplades del rang
    to->fusion.setWidth( width ); // la fusió tindrà aquesta amplada
    to->fusion.setLevel( to->fusion.getLevel() + 1 );
    // calculem l'entropia d'aquesta fusió
    HN hn = mergeEntropy( to->fusion.getX(), to->fusion.getX() + to->fusion.getWidth() - 1 );
    /*
    // comprovacions de correctesa : això sembla que no afecta
    if ( hn.area != unsigned( to->region.getArea() ) + unsigned( from->region.getArea() ) ) 
    {
        std::cout << "ImageComplexity3D::fusion >> Inconsistent n sum value :: hn.area = " << hn.area << " 'TO' region area: " << to->region.getArea() << " 'FROM' region area:  " << from->region.getArea() << std::endl << std::flush;
        to->region.print();
        from->region.print();
    }
    */
    to->fusion.setProxyEntropy( hn.entropy );
    to->fusion.setGlobalProportion( double( hn.area ) / m_size );
    to->fusion.setArea( hn.area );
    
    // càlcul del guany d'info mútua
    to->mutualInformation = to->fusion.getGlobalProportion()   * to->fusion.getProxyEntropy() 
            - to->region.getGlobalProportion()   * to->region.getProxyEntropy()
            - from->region.getGlobalProportion() * from->region.getProxyEntropy();
}

template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::splitCandidatesToLeaves()
{
// flush: passem els candidats que queden a les fulles
    while ( ! m_splitCandidates.empty() )
    {
        m_splitLeaves.push_back( m_splitCandidates.top().getRegion() );
        m_splitCandidates.pop();
    }
}

template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::comput()
{
    imagePartition();
    histogramQuantization();
    m_lastUpdate = 0;
}

template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::imagePartition()
{
    int i = 0;
    initSplitData();
    while ( splitTest() ) // condició per continuar
    {
        i++;
        split(); // partir i recalcular mesures Teoria Inf
    }
    std::cout << "Number of total iterations: " << i << std::endl;
    splitCandidatesToLeaves();
}

template <typename TInputImage>
void
ImageComplexity3D <TInputImage>::histogramQuantization()
{
    // comput original
    /* 
    setData();
    while (test()) {
       insert(select());
	  display(); }
    candidatesToLeaves();
    */
    initMergeData();
    while( mergeTest() ) 
    {
        insert( select() );
    }
    mergeCandidatesToLeaves();
}

template <typename TInputImage>
void 
ImageComplexity3D <TInputImage>::mergeCandidatesToLeaves()
{
// Flush: passem de la llista de regions fusionades candidates al conjunt final
    while ( !m_mergeCandidates.empty() ) 
    {
        m_mergeLeaves.push_back( m_mergeCandidates.front().region );
        m_mergeCandidates.pop_front(); 
    }
}     

template <typename TInputImage>
typename ImageComplexity3D <TInputImage>::MergeNodesListIteratorType
ImageComplexity3D <TInputImage>::select()
{
  // Minimum 2 (Control candidates in test) => 1 because the last have not fusion.
  
  // Select the candidate with minimum MI.
  MergeNodesListIteratorType minimum = m_mergeCandidates.begin();
  MergeNodesListIteratorType p = minimum;
  MergeNodesListIteratorType last = m_mergeCandidates.end();
  for ( p++, last--; p != last;	  p++) 
         if ( p->mutualInformation < minimum->mutualInformation ) 
             minimum = p;

  return minimum;
}

template <typename TInputImage>
void 
ImageComplexity3D <TInputImage>::insert(MergeNodesListIteratorType p)
{
    MergeNodesListIteratorType q = p;
    if ( ++q == m_mergeCandidates.end() ) 
        std::cout << "ImageComplexity3D::insert >> Inconsistent data" << std::endl;
    
    double areas = - p->fusion.getGlobalProportion() * MathTools::logTwo(p->fusion.getGlobalProportion() ) 
                    + p->region.getGlobalProportion() * MathTools::logTwo(p->region.getGlobalProportion() ) 
                + q->region.getGlobalProportion() * MathTools::logTwo(q->region.getGlobalProportion() );
    
    double mi = p->mutualInformation;
    m_mergeCandidates.erase(q);
    update( p ); 
    
    m_merge_n--;
    m_merge_miCR = MathTools::zero(m_merge_miCR - mi);
    m_merge_hcCR = MathTools::zero(m_merge_hcCR + mi);
    m_merge_hR = MathTools::zero(m_merge_hR + areas);  
    m_merge_hjCR = MathTools::zero(m_merge_hC + m_merge_hR - m_merge_miCR); 
    m_merge_pih = MathTools::divReal(m_merge_miCR, m_merge_hjCR);
    
    m_proportion = computMergeProportion(); 
}

template <typename TInputImage>
bool 
ImageComplexity3D <TInputImage>::update( MergeNodesListIteratorType p )
{
  bool change = false;
  MergeNodesListIteratorType q = p;
  p->region = p->fusion;
  p->mutualInformation = -1;  // The last only.
  
  // Update fusion in left:
  if ( p != m_mergeCandidates.begin() )  
  {
      fusion(p, --q);
	  change = true; 
  }

  // Update fusion with right:
  q = p;
  if ( ++q != m_mergeCandidates.end() ) 
  {
	  fusion(q, p);
	  change = true; 
  }

  // If no change => only p in the world => test will be false.
  return change;
}

template <typename TInputImage>
bool ImageComplexity3D <TInputImage>
::splitTest()
{
  bool more =  m_proportion < 1 ;
  std::cout << std::endl << "splitTest() >> m_proportion ::" << m_proportion << std::endl;
  if ( more && ! m_splitCandidates.size() ) // No more MI (with UH).
  {
      std::cout << "ImageComplexity3D::splitTest >> No more MI to gain. The goal is excesive." << std::endl;
      std::cout << "ImageComplexity3D::splitTest >> Decrease the -p value ( coefficient ) or try with other parametrization." << std::endl;
      
      return false;
  }

  // Here more => there is ever MI to gain! => candidates no empty!
  return more;
}

template <typename TInputImage>
bool ImageComplexity3D <TInputImage>
::mergeTest()
{
    /*
        bool more = !gequal(shared.proportion, 1);
  if (more && shared.candidates.size() < 2) {
       fprintf(out.std, "No more fusions are possible. The goal is excesive.\n");
	  fprintf(out.std, "Decrease the -p value or try with other parametrization.\n");
	  return false; }

  // Here more => there is ever MI to gain! => candidates no empty!
  return more;
    */

  bool more =  m_proportion < 1 ;
  std::cout << std::endl << "mergeTest() >> m_proportion ::" << m_proportion << std::endl;
  if ( more && m_mergeCandidates.size() < 2 ) 
  {
      std::cout << "ImageComplexity3D::mergeTest >> No more fusions are possible. The goal is excesive." << std::endl;
      std::cout << "ImageComplexity3D::mergeTest >> Decrease the -p value ( coefficient ) or try with other parametrization." << std::endl;
      
      return false;
  }

  // Here more => there is ever MI to gain! => candidates no empty!
  return more;
}

template <typename TInputImage>
void ImageComplexity3D <TInputImage>
::split()
{
    // agafar un dels candidats
    // recalcular mesures de la teoria de la informacio
    NodeType maximum = m_splitCandidates.top();  // Control existence in test.
    m_splitCandidates.pop();
    
    SplitRegionsList list = maximum.getPartitionList();
    while (list.size() > 0)
    {
        // Get the value of the "front" list item.
        RegionType r = list.front();
        m_regionsEntropy -= r.getGlobalProportion() *  MathTools::logTwo(r.getGlobalProportion()) ;
        insert( r );
        // Remove the item from the front of the list 
        list.pop_front();
    } 
    m_numberOfRegions += maximum.getPartitionList().size() - 1;
    m_mutualInformationImageRegions += maximum.getMutualInformation();
    m_conditionalEntropyImageRegions -= maximum.getMutualInformation();
    m_regionsEntropy += maximum.getRegion().getGlobalProportion() * MathTools::logTwo( maximum.getRegion().getGlobalProportion() );
    
    m_jointEntropyImageRegions = m_imageEntropy + m_regionsEntropy - m_mutualInformationImageRegions;
    m_normalizedMutualInformation =  m_mutualInformationImageRegions / m_jointEntropyImageRegions;

  //  maximum.getPartitionList().clear();
    m_proportion = computSplitProportion();
}



template <typename TInputImage>
bool ImageComplexity3D <TInputImage>
::insert( RegionType region )
{
    bool ok = true;
    /* Si el coeficient de la regió és major que l'especificat fiquem la regió a la llista
    El que vol dir és que si hem arribat a una regió que no s'ha de tallar més degut a les propietats que té, llavors la posem al conjunt de regions finals
    */
     
    if ( region.coefficientUH() >= m_uniformEntropy )
    {
        m_splitLeaves.push_back(region);
        ok = true;
    }
    else
    {
        // sinó, crearem noves particions, és a dir, la regió encara es pot tallar i és per això que crearem un nou node
    
        NodeType node;
        node.setRegion( region );
        node.setMutualInformation( 0 );  // MI initially local.

        typename NodeType::PartitionListType partitionList;

        // fem el tall
        
        switch ( m_cut )
        {
            case Bipartition:
            
                node.setMutualInformation( region.bipartition( m_inputImage, partitionList ) );
                node.setPartitionList( partitionList );
                
            break;
            case Octree:
                // encara per implementar
                std::cout <<" ImageComplexity3D::insert >> Octree cut not implemented yet " << std::endl;
                /*
                node.setMutualInformation( region.quadtree( shared.image, shared.width,
                node.partition, false ) );
                */
            break;
            /*
            case 4: node.setMutualInformation( region.quadtree( shared.image, shared.width,
            node.partition, true ) );
            break;
            */
            default:
                std::cerr << "ImageComplexity3D::insert >> Option impossible" << std::endl;
            break;
        }
        // si s'han pogut crear noves particions ( la llista no és buida )
        // llavors aportem la nova regió
        if ( ! node.getPartitionList().size() )
        {
            m_splitLeaves.push_back( region );
            ok = true;
        }
        else
        {
            // sinó posem el nou node com a candidat
            node.setMutualInformation( node.getMutualInformation() * region.getGlobalProportion() ) ;
            // MI now global.
            m_splitCandidates.push( node );
            ok = false;
        }
    }

    return ok;
}

template <typename TInputImage>
double ImageComplexity3D <TInputImage>
::computSplitProportion()
{
    double p = 0;  // Reduce warning.

    switch ( m_splitStopCriteria )
    {
    case ImageComplexity3D::NumberOfRegions: // Nombre de regions => ImageComplexity3D::NumberOfRegions
        p = m_numberOfRegions / m_splitCoefficient;
    break;
    case ImageComplexity3D::ErrorProbability: // Probalitat d'error ( desigualtat Fano ) => ImageComplexity3D::ErrorProbability
        
        p = MathTools::divReal( m_mutualInformationImageRegions, m_miMin , true , 1 ); 
//         std::cout << "computSplitProportion() >> I(C,R)/Imin = " << m_mutualInformationImageRegions << " / " << m_miMin << " Error Probability, proportion value: " << p << std::endl;
    break;
    case ImageComplexity3D::MutualInformationRatio: // Pr-MI: Rati MI, MIRp => ImageComplexity3D::MutualInformationRatio
    
        p = MathTools::divReal( m_mutualInformationImageRegions, m_splitCoefficient * m_imageEntropy , true , 1 ); 
//         std::cout << "computSplitProportion() >> MI Ratio, I(C,R)/(coef*H(C)):: I(C,R):" << m_mutualInformationImageRegions << " coefficient: " << m_splitCoefficient << " H(C):" << m_imageEntropy << " proportion value: " << p << std::endl;
    break;
    case ImageComplexity3D::MutualInformation: // Informació mútua => ImageComplexity3D::MutualInformation
        
        p = MathTools::divReal( m_mutualInformationImageRegions , m_splitCoefficient, true , 1 ); 
//         std::cout << "computSplitProportion() >> Mutual information, , proportion value: " << p << std::endl;
    break;
    case ImageComplexity3D::NormalizedMutualInformation: // Informació mútua normalitzada: I/H => ImageComplexity3D::NormalizedMutualInformation
        
        p = MathTools::divReal( m_normalizedMutualInformation, m_splitCoefficient, true , 1 ); 
//         std::cout << "computSplitProportion() >> Normalized mutual information, proportion value: " << p << std::endl;
    break;
    default:
        std::cerr << "ImageComplexity3D::computSplitProportion >> Option impossible" << std::endl;
    break;
    }
    return p;
/*
    case 1: p = divReal(shared.it.miCR, shared.miMin, true, 1); break;
    case 2: p = divReal(shared.it.miCR, parameters.coefficient * shared.it.imageEntropy, true, 1); break;
    case 3: p = divReal(shared.it.miCR, parameters.coefficient, true, 1); break;
    case 4: p = divReal(shared.it.pih, parameters.coefficient, true, 1); break;

*/
}

template <typename TInputImage>
double ImageComplexity3D <TInputImage>
::computMergeProportion()
{
/*
REAL p = 0;  // Reduce warning.
  switch (parameters.test) {
       case 0: p = (parameters.histogramBins - shared.it.n) / (parameters.histogramBins - parameters.coefficient); break;
	  case 1: p = divReal(shared.it.hC - shared.it.miCR, shared.it.hC - shared.miMin, true, 1); break;
	  case 2: p = divReal(shared.it.hC - shared.it.miCR, shared.it.hC - parameters.coefficient * shared.it.hC, true, 1); break;
	  
      case 3: p = divReal(shared.it.miCRMax - shared.it.miCR, shared.it.miCRMax - parameters.coefficient * shared.it.miCRMax, true, 1); break;
	  
      case 4: p = divReal(shared.it.hC - shared.it.miCR, shared.it.hC - parameters.coefficient, true, 1); break;
	  
      case 5: p = divReal(1 - shared.it.pih, 1 - parameters.coefficient, true, 1); break;
       default: messageUser(PROGRAM, "Option impossible", "ICMain::computProportion"); } 
  
  return p;
*/
    double p = 0;  // Reduce warning.

    switch ( m_mergeStopCriteria )
    {
    case ImageComplexity3D::NumberOfColours: // Nombre de colors?
        p = ( m_histogramBins - m_merge_n ) / ( m_histogramBins - m_mergeCoefficient );
        
//         std::cout << "computMergeProportion() >> Number Of Colours :: ( m_histogramBins - m_merge_n ) / ( m_histogramBins - m_mergeCoefficient ) = " << m_histogramBins - m_merge_n << " / " << m_histogramBins - m_mergeCoefficient << " , proportion value: " << p << std::endl;
    break;
    
    case ImageComplexity3D::ErrorProbability: // Probalitat d'error ( desigualtat Fano )
        p = MathTools::divReal( m_merge_hC - m_merge_miCR , m_merge_hC - m_miMin, true, 1 );
     
//         std::cout << "computMergeProportion() >> Error Probability :: H(C)-I(C,R)/H(C)-Imin = " << m_merge_hC - m_merge_miCR << " / " << m_merge_hC - m_miMin << " , proportion value: " << p << std::endl;
    break;
    
    case ImageComplexity3D::MutualInformationRatio: // Pr-MI: Rati MI, MIRp 
        p = MathTools::divReal( m_merge_hC - m_merge_miCR, m_merge_hC - m_mergeCoefficient * m_merge_hC, true, 1);
        
//         std::cout << "computMergeProportion() >> Mutual Information Ratio ::  ( m_merge_hC - m_merge_miCR ) / ( m_merge_hC - m_mergeCoefficient * m_merge_hC ) = " << m_merge_hC - m_merge_miCR << " / " << m_merge_hC - m_mergeCoefficient * m_merge_hC << " , proportion value: " << p << std::endl;
    break;
    
    case ImageComplexity3D::MutualInformationPercent: // % Informació mútua 
        p = MathTools::divReal( m_merge_miCRMax - m_merge_miCR, m_merge_miCRMax - m_mergeCoefficient * m_merge_miCRMax, true, 1);

//         std::cout << "computMergeProportion() >> Mutual Information % :: ( m_merge_miCRMax - m_merge_miCR ) / ( m_merge_miCRMax - m_mergeCoefficient * m_merge_miCRMax ) = " << m_merge_miCRMax - m_merge_miCR << " / " << m_merge_miCRMax - m_mergeCoefficient * m_merge_miCRMax << " ?(3)?, proportion value: " << p << std::endl;
    break;
    case ImageComplexity3D::MutualInformation: // Informació Mútua
        p = MathTools::divReal( m_merge_hC - m_merge_miCR , m_merge_hC - m_mergeCoefficient, true, 1);
//         std::cout << "computMergeProportion() >> Mutual Information (Abs. Value):: ( m_merge_hC - m_merge_miCR ) / ( m_merge_hC - m_mergeCoefficient ) = " << m_merge_hC - m_merge_miCR << " / " << m_merge_hC - m_mergeCoefficient << " , proportion value: " << p << std::endl;
    break;
    case ImageComplexity3D::NormalizedMutualInformation: // Informació mútua normalitzada: I/H
        p = MathTools::divReal(1 - m_merge_pih, 1 - m_mergeCoefficient, true, 1);
//         std::cout << "computMergeProportion() >> Normalized MutualInformation :: ( 1 - m_merge_pih ) / ( 1 - m_mergeCoefficient ) = " << 1 - m_merge_pih << " / " << 1 - m_mergeCoefficient << " ?(5)?, proportion value: " << p << std::endl;
    break;
    default:
        std::cerr << "ImageComplexity3D::computMergeProportion >> Option impossible" << std::endl;
    break;
    }
    return p;

}

};  // end namespace udg

#endif
