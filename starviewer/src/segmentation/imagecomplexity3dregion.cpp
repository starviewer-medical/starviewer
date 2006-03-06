/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGIMAGECOMPLEXITY3DREGION_CPP
#define UDGIMAGECOMPLEXITY3DREGION_CPP

#include "imagecomplexity3dregion.h"

// CPP
#include <iostream>
namespace udg {



template < typename TInputImage >
ImageComplexity3DRegion<TInputImage>::ImageComplexity3DRegion()
{
    IndexType index;
    SizeType size;

    index.Fill( 0 );
    size.Fill( 0 );
    m_region.SetIndex( index );
    m_region.SetSize( size );

    //m_histogram = 0;

    m_level = 0;
    m_globalProportion = 0;
    m_localProportion = 0;

    // Automatic h_
    //copyData();
}

template < typename TInputImage >
ImageComplexity3DRegion<TInputImage>::ImageComplexity3DRegion( const ImageComplexity3DRegion& region )
{
    *this = region;

}

template < typename TInputImage >
ImageComplexity3DRegion<TInputImage>::~ImageComplexity3DRegion()
{
}

template < typename TInputImage >
ImageComplexity3DRegion<TInputImage>&
ImageComplexity3DRegion<TInputImage>::operator =( const ImageComplexity3DRegion& region )
{
    m_localProportion = region.m_localProportion;
    m_globalProportion = region.m_globalProportion;
    m_region = region.m_region;
    m_histogram = region.m_histogram;
    m_sort = region.m_sort;
    m_level = region.m_level;
    
    
    return *this;
}

template < typename TInputImage >
double
ImageComplexity3DRegion<TInputImage>::getEntropy()
{
/*
    if( !m_histogram )
    {
        std::cerr << "ImageComplexity3DRegion::getEntropy() There's no histogram set, needed to compute the entropy. Returning 0.0 " << std::endl;
        return 0.0;
    }
    else
    {
    */
//        std::cout << " returning histograms entropy" << std::endl;
        return m_histogram.entropy();
//     }

}

template < typename TInputImage >
double
ImageComplexity3DRegion<TInputImage>::coefficientUH()
{
/*
    if( !m_histogram )
    {
        std::cerr << "ImageComplexity3DRegion::coefficientUH() >> There's no histogram set, needed to compute the coefficient. Returning 0.0" << std::endl;
        return 0.0;
    }
    else
    */
        return m_histogram.coefficientUH();
}

template < typename TInputImage >
bool
ImageComplexity3DRegion<TInputImage>::operator <( ImageComplexity3DRegion& region )
{
    if ( m_sort == Any ) std::cerr << "REGION::operator< :: No sort criteria" << std::endl;

    if ( m_sort & Coordenates )
    {
        if( this->getX() < region.getX() )
            return true;
        else if ( this->getX() > region.getX() )
            return false;
        else if ( this->getY() < region.getY() )
            return true;
        else if ( this->getY() > region.getY() )
            return false;
        else if ( this->getZ() < region.getZ() )
            return true;
        else if ( this->getZ() > region.getZ() )
            return false;
    }

    if ( m_sort & Level )
    {
        if ( m_level < region.m_level ) return true;
        else if ( m_level > region.m_level ) return false;
    }

    if ( m_sort & Volume )
    {
        unsigned myVolume = volume(),
                yourVolume = region.volume();
        if  ( myVolume < yourVolume ) return true;
        else if ( myVolume > yourVolume ) return false;
    }

    if ( m_sort & Shape )
    {
        double myShape, yourShape;
        myShape = shape( m_sort & Orientation );
        yourShape = region.shape( m_sort & Orientation );

        if ( myShape < yourShape ) return true;
        else if ( myShape > yourShape ) return false;
    }

    if ( m_sort & FirstData ) // entropia
    {
        if( this->getEntropy() < region.getEntropy() )
            return true;
        else if( this->getEntropy() > region.getEntropy() )
            return false;
    }

    return false;
}
template < typename TInputImage >
double
//ImageComplexity3DRegion<TInputImage>::bipartition( InputImageType* imageData, ImageRegionSetType& set )
ImageComplexity3DRegion<TInputImage>::bipartition( TInputImage* imageData, ImageRegionSetType& set )
{

    // Pre: Entropy, local proportion and global proportion in data (this order).
    // Add a best Mutual Information gain bipartition X or Y in set.
    // Full the respective histograms from data matrix (width).
    // Put entropies in first data.
    // Put proportions in second data.
    // Scale de third data by the second.
    // Return the MI gain with set partition.

    if ( set.size() ) std::cerr << " ImageComplexity3D::bipartition >> Set is not empty" << std::endl;

    if ( this->getEntropy() == 0 ) return 0;

    // Informació mútua a guanyar => Partició Segura
    
    int xMax = this->getX() + this->getWidth() - 1; 
    int yMax = this->getY() + this->getHeight() - 1;
    int zMax = this->getZ() + this->getDepth() - 1; 
    
    int xInit = this->getX();
    int yInit = this->getY();
    int zInit = this->getZ();
    
    char partition = '?';
    char cutR = '!';
    unsigned coordenate = 0;
    double minimum = 1e+303; // REAL minimum = _INFINITY_POSITIVE_;
    double subEntropies; // REAL subEntropies; // Both.
    ImageComplexity3DRegion r[2]; // Les dues regions tallades resultants

    //HISTOGRAM h[4]; // 0,1 càlculs temporals, 3,4 resultants
    ImageComplexity3DHistogram h[4]; // 0,1 càlculs temporals, 3,4 resultants

    h[0] = m_histogram; // inicialment una sola regió = regió 'pare' actual, per si no hi ha talls
    double entropy[4]; // 0,1 càlculs temporals, 3,4 resultants

    typename InputImageType::IndexType pixelIndex; // l'índex per recórrer la imatge
/*
    std::cout << "INFO REGIÓ PARE BEFORE DOING ANYTHING!!!!!!!!!!!!" << std::endl;
    print();
*/
    // Analitzem els talls en l'eix X
    if ( this->getWidth() > 1 ) // Restricció d'amplada,sinó saltarem
    {
        h[0].reset();
        for ( pixelIndex[0] = xInit;  pixelIndex[0] <= xMax; pixelIndex[0]++ )
        {
            for( pixelIndex[1] = yInit; pixelIndex[1] <= yMax; pixelIndex[1]++ )
                for( pixelIndex[2] = zInit; pixelIndex[2] <= zMax; pixelIndex[2]++ )
                    h[0].in( imageData->GetPixel( pixelIndex ) );

            h[1] = m_histogram;
            h[1] -= h[0];
            
            entropy[0] = h[0].entropy();
            entropy[1] = h[1].entropy();

            subEntropies = ( entropy[0] * ( pixelIndex[0] - xInit + 1 ) + entropy[1] * ( xMax - pixelIndex[0] ) ) / this->getWidth();

            if ( subEntropies < minimum )
            {
                minimum = subEntropies;
                partition = 'X';
                coordenate = pixelIndex[0];
                h[2] = h[0];
                h[3] = h[1];
                entropy[2] = entropy[0];
                entropy[3] = entropy[1];
            }
        }
    }
    // Analysis of Y
    if ( this->getHeight() > 1 ) 
    {
        h[0].reset();
        for ( pixelIndex[1] = yInit;  pixelIndex[1] <= yMax; pixelIndex[1]++ )
        {
            for( pixelIndex[2] = zInit; pixelIndex[2] <= zMax; pixelIndex[2]++ )
                for( pixelIndex[0] = xInit; pixelIndex[0] <= xMax; pixelIndex[0]++ )

                    h[0].in( imageData->GetPixel( pixelIndex ) );
                    
        
            h[1] = m_histogram;
            h[1] -= h[0];

            entropy[0] = h[0].entropy();
            entropy[1] = h[1].entropy();

            subEntropies = ( entropy[0] * ( pixelIndex[1] - yInit + 1 ) + entropy[1] * ( yMax - pixelIndex[1] ) ) / this->getHeight();
            if ( subEntropies < minimum )
            {
                minimum = subEntropies;
                partition = 'Y';
                coordenate = pixelIndex[1];
                h[2] = h[0];
                h[3] = h[1];
                entropy[2] = entropy[0];
                entropy[3] = entropy[1];
            }
        }
    }
     // Analysis of Z
    if ( this->getDepth() > 1 )
    {
        h[0].reset();
        for( pixelIndex[2] = zInit; pixelIndex[2] <= zMax; pixelIndex[2]++ )
        {
            for ( pixelIndex[1] = yInit;  pixelIndex[1] <= yMax; pixelIndex[1]++ )
                for( pixelIndex[0] = xInit; pixelIndex[0] <= xMax; pixelIndex[0]++ )
                    h[0].in( imageData->GetPixel( pixelIndex ) );
                    
            h[1] = m_histogram;
            h[1] -= h[0];
            
            entropy[0] = h[0].entropy();
            entropy[1] = h[1].entropy();

            subEntropies = ( entropy[0] * ( pixelIndex[2] - zInit + 1 ) + entropy[1] * ( zMax - pixelIndex[2] ) ) / this->getDepth();

            if ( subEntropies < minimum )
            {
                minimum = subEntropies;
                partition = 'Z';
                coordenate = pixelIndex[2];
                h[2] = h[0];
                h[3] = h[1];
                entropy[2] = entropy[0];
                entropy[3] = entropy[1];
            }
        }
    }

    /*
        Quan el tall és indiferent fer-lo a X, Y o Z, alternem el tall a X Y Z per tal de no fer-lo sempre al mateix eix, sempre que sigui possible degut a l'amplada
    */
    if ( minimum == this->getEntropy() ) //if (equal(minimum, data_[0]))
    {
        static char cut = 'x';

        if( cut == 'x' && this->getWidth() == 1 ) cut = 'y';
        if( cut == 'y' && this->getHeight() == 1 ) cut = 'z';
        if( cut == 'z' && this->getDepth() == 1 ) cut = 'x'; // això pot ser un problema
        
        cutR = cut;
        
        switch( cut )
        {
        case 'x':
//             std::cout << "Cutting by x" << std::endl;
            r[0].setOrigin( this->getX() , this->getY() , this->getZ() );
            r[1].setOrigin( this->getX() + this->getWidth() / 2 , this->getY() , this->getZ() );
            r[0].setSize( this->getWidth() / 2 , this->getHeight() , this->getDepth() );
            r[1].setSize( this->getWidth() - r[0].getWidth() , this->getHeight() , this->getDepth() );
            
            r[0].setHistogram( m_histogram ); 
            r[1].setHistogram( m_histogram );
            
            r[0].setLocalProportion( double( r[0].getWidth()) / this->getWidth() );
            r[1].setLocalProportion( double(r[1].getWidth()) / this->getWidth() ); 
            cut = 'y';
            // Això en principi no cal perquè es calcula sol quan sigui necessari
            /*
            r[0].in(data, width, true); //r[0].in(data, width, true);
            r[1].in(data, width, true); //r[1].in(data, width, true);
            r[0].data_[0] = r[0].h_.entropy(); //r[0].data_[0] = r[0].h_.entropy();
            r[1].data_[0] = r[1].h_.entropy(); //r[1].data_[0] = r[1].h_.entropy();
            */
        break;
        case 'y':
//             std::cout << "Cutting by y" << std::endl;
            r[0].setOrigin( this->getX() , this->getY() , this->getZ() );
            r[1].setOrigin( this->getX() , this->getY() + this->getHeight() / 2, this->getZ() );
            r[0].setSize( this->getWidth() , this->getHeight() / 2 , this->getDepth() );
            r[1].setSize( this->getWidth() , this->getHeight() - r[0].getHeight() , this->getDepth() );
            r[0].setHistogram( m_histogram );
            r[1].setHistogram( m_histogram );
            
            r[0].setLocalProportion( double( r[0].getHeight()) / this->getHeight() ); 
            r[1].setLocalProportion( double(r[1].getHeight()) / this->getHeight() ); 
            cut = 'z';
            // Això en principi no cal perquè es calcula sol quan sigui necessari
            /*
            r[0].in(data, width, true);
            r[1].in(data, width, true);
            r[0].data_[0] = r[0].h_.entropy();
            r[1].data_[0] = r[1].h_.entropy();
            */
        break;
        case 'z':
//             std::cout << "Cutting by z" << std::endl;
            r[0].setOrigin( this->getX() , this->getY() , this->getZ() );
            r[1].setOrigin( this->getX() , this->getY() , this->getZ() + this->getDepth() / 2 );
            r[0].setSize( this->getWidth() , this->getHeight() , this->getDepth() / 2 );
            r[1].setSize( this->getWidth() , this->getHeight() , this->getDepth() - r[0].getDepth() );
            r[0].setHistogram( m_histogram );
            r[1].setHistogram( m_histogram );
            
            r[0].setLocalProportion( double( r[0].getDepth()) / this->getDepth() );
            r[1].setLocalProportion( double( r[1].getDepth()) / this->getDepth() );
            cut = 'x';
        break;
        default:
        break;
        }
//         std::cout << "Demanant entropia regio 0 i 1 [calcul minimum]" << std::endl;
        minimum = r[0].getEntropy() * r[0].getLocalProportion() + r[1].getEntropy() * r[1].getLocalProportion();
        

    }
    else if ( partition == 'X' )
    {
    cutR = '!';

        r[0].setOrigin( this->getX() , this->getY(), this->getZ() );
        r[1].setOrigin( coordenate + 1, this->getY(), this->getZ() );

        r[0].setSize( coordenate - this->getX() + 1, this->getHeight(), this->getDepth() );
        r[1].setSize( xMax - coordenate , this->getHeight(), this->getDepth() );

        r[0].setHistogram( h[2] );
        r[1].setHistogram( h[3] );

        r[0].setLocalProportion( double( r[0].getWidth() ) / this->getWidth() ); 
        r[1].setLocalProportion( double( r[1].getWidth() ) / this->getWidth() );
        
        // Això en principi no cal perquè es calcula sol quan sigui necessari
        //r[0].data_[0] = entropy[2]; //r[0].data_[0] = entropy[2];
        //r[1].data_[0] = entropy[3]; //r[1].data_[0] = entropy[3];

    }
    else if ( partition == 'Y' )
    {
    cutR = '!';

        r[0].setOrigin( this->getX() , this->getY(), this->getZ() );
        r[1].setOrigin( this->getX() , coordenate + 1 , this->getZ() );

        r[0].setSize( this->getWidth() , coordenate - this->getY() + 1, this->getDepth() );
        r[1].setSize( this->getWidth() , yMax - coordenate, this->getDepth() );

        r[0].setHistogram( h[2] );
        r[1].setHistogram( h[3] );

        r[0].setLocalProportion( double(r[0].getHeight()) / this->getHeight() ); 
        r[1].setLocalProportion( double(r[1].getHeight()) / this->getHeight() ); 
        // Això en principi no cal perquè es calcula sol quan sigui necessari
        //r[0].setEntropy( entropy[2] ); //r[0].data_[0] = entropy[2];
        //r[1].setEntropy( entropy[3] ); //r[1].data_[0] = entropy[3];
    }
    else if ( partition == 'Z' )
    {
    cutR = '!';

        r[0].setOrigin( this->getX() , this->getY() , this->getZ() );
        r[1].setOrigin( this->getX() , this->getY() , coordenate + 1 );

        r[0].setSize( this->getWidth() , this->getHeight() , coordenate - this->getZ() + 1 );
        r[1].setSize( this->getWidth() , this->getHeight() , zMax - coordenate );

        r[0].setHistogram( h[2] );
        r[1].setHistogram( h[3] );

        r[0].setLocalProportion( double(r[0].getDepth()) / this->getDepth() );
        r[1].setLocalProportion( double(r[1].getDepth()) / this->getDepth() );
    }
    else std::cerr << "ImageComplexity3DRegion::bipartition >> Cut is not do" << std::endl;

    r[0].m_level = m_level + 1; 
    r[1].m_level = m_level + 1; 
    r[0].setGlobalProportion( m_globalProportion * r[0].getLocalProportion() );
    r[1].setGlobalProportion( m_globalProportion * r[1].getLocalProportion() );

    set.push_back( r[0] );
    set.push_back( r[1] );

    return this->getEntropy() - minimum;

}



};  // end namespace udg

#endif
