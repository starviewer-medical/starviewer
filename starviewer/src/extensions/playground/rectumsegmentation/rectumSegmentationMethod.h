/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGRECTUMSEGMENTATIONMETHOD_H
#define UDGRECTUMSEGMENTATIONMETHOD_H

#include "volume.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class rectumSegmentationMethod{

public:

    typedef itk::Image< Volume::ItkImageType::PixelType, 3 >  IntermediateImageType;
    typedef itk::Image< Volume::ItkImageType::PixelType, 2 >  InternalImageType;
    typedef itk::Image< unsigned char, 2 >  ExternalImageType;

    rectumSegmentationMethod();

    ~rectumSegmentationMethod();

    double applyMethod();

    void applyMethodNextSlice( unsigned int slice, int step );

    void regionGrowingRecursive( int indexX, int indexY, int prof );

    void applyFilter(Volume* output);

    int getNumberOfVoxels() {return m_cont;}

    void setVolume(Volume * vol){m_Volume=vol;}

    void setMask(Volume * vol){m_Mask=vol;}

    void setSeedPosition (double x, double y, double z);

    void setHistogramLowerLevel (int x);

    void setHistogramUpperLevel (int x);

    void setInsideMaskValue (int x) {m_insideMaskValue=x;};

    void setOutsideMaskValue (int x) {m_outsideMaskValue=x;};

    void setMultiplier (double x) {m_multiplier=x;};

    void setMinROI (int x[2]) {m_minROI[0]=x[0];m_minROI[1]=x[1];};

    void setMaxROI (int x[2]) {m_maxROI[0]=x[0];m_maxROI[1]=x[1];};


private:

    ///Volum que volem segmentar
    Volume* m_Volume;

    ///Volum on guardarem el resultat de la segmetnació que volem segmentar
    Volume* m_Mask;
    Volume* m_filteredInputImage;

	InternalImageType::Pointer m_maskrecursive;
	InternalImageType::Pointer m_imrecursive;

    ///Posició de la llavor
    double m_px, m_py, m_pz;

    ///llindars de l'histograma
    int m_lowerThreshold, m_upperThreshold;

    ///valors interns i externs de la màscara
    int m_insideMaskValue, m_outsideMaskValue;

    ///valors interns i externs de la màscara
    double m_volume;
    int    m_cont;

    ///indexs extrems de la ROI
    int m_minROI[2];
    int m_maxROI[2];


    ///valors mètode edema
    double m_multiplier;

    ///Retorna quants voxels != de 0 hi ha a la màscara
    int computeSizeMask();

};

}

#endif
