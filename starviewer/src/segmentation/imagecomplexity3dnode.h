/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGIMAGECOMPLEXITY3DNODE_H
#define UDGIMAGECOMPLEXITY3DNODE_H

// STL
#include <list> 

#include "imagecomplexity3dregion.h"

namespace udg {

/**
Classe relacionada amb ImageComplexity3D. Representa un node de les regions de la BSP

Un node conté la regió que es talla, les regions resultants de tallar d'alguna manera
la regió pare i el guany d'informació mútua que això suposa.

@author Grup de Gràfics de Girona  ( GGG )
*/


template <typename TInputRegion>
class ImageComplexity3DNode {
public: 
 
    typedef TInputRegion InputRegionType;
    typedef std::list<TInputRegion> PartitionListType;
    
    ImageComplexity3DNode();
    ~ImageComplexity3DNode();

    /// Li assignem la informació mútua al node
    void setMutualInformation( double mi )
    { 
        m_mutualInformation = mi; 
    }
    /// Li assignem la regió que correspon al node
    void setRegion( InputRegionType region )
    { 
        m_region = region; 
    }
    
    /// Obtenim la llista/conjunt de particions possibles
    PartitionListType getPartitionList() const 
    { 
        return m_partition; 
    }
    /// Assignem la llista de particions
    void setPartitionList( PartitionListType list )
    { 
        m_partition = list; 
    }
    
    /// Ens retorna la informació mútua del node
    double getMutualInformation() const 
    { 
        return m_mutualInformation; 
    }
    /// Ens retorna la regió del node
    InputRegionType getRegion() const 
    { 
        return m_region; 
    }
    
    /// Operador de comparació, necessari per a l'ordenació de nodes
    inline bool operator< (const ImageComplexity3DNode& node) const
    {
        return m_mutualInformation < node.m_mutualInformation;
    }
    /// operador assignació
    ImageComplexity3DNode& operator = (const ImageComplexity3DNode& node)
    {
        m_region = node.m_region;
        m_partition = node.m_partition;
        m_mutualInformation = node.m_mutualInformation;
        return *this;
    }
    
private:
    /// La regió pare que tallarem ( és realment necessari guardar-la? )
    InputRegionType m_region; 
    /// Conté el conjunt de regions que hem tallat en aquest punt
    PartitionListType m_partition; 
    /// El guany d'informació mútua obtingut amb el tall aplicat a la regió
    double m_mutualInformation; 

};


};  //  end  namespace udg 

#include "imagecomplexity3dnode.cpp"

#endif
