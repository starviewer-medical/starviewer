/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGSTROKESEGMENTATIONMETHOD_H
#define UDGSTROKESEGMENTATIONMETHOD_H

#include "volume.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class StrokeSegmentationMethod{

public:

    StrokeSegmentationMethod();

    ~StrokeSegmentationMethod();

    double applyMethod();

    /// Neteja els casos propers al crani
    double applyCleanSkullMethod();

    void applyFilter(Volume* output);

    void computeSpeedMap(itk::Image< float, 3 >* speedMap);

    double erfc(double x);

    double applyMethodEdema(Volume * lesionMask);

    double applyMethodEdema2(Volume * lesionMask);

    int getNumberOfVoxels() {return m_cont;}

    int getEdemaNumberOfVoxels() {return m_edemaCont;}

    double applyVentriclesMethod();

    void applyMethod2();

    int applyMethod3();

    int applyMethod4();

    void setVolume(Volume * vol){m_Volume=vol;}

    void setMask(Volume * vol){m_Mask=vol;}

    Volume::ItkImageType * getItkMask (){return m_Mask->getItkData();}

    Volume::VtkImageTypePointer getVtkMask (){return m_Mask->getVtkData();}

    void setSeedPosition (double x, double y, double z);

    void setHistogramLowerLevel (int x);

    void setHistogramUpperLevel (int x);

    void setInsideMaskValue (int x) {m_insideMaskValue=x;};

    void setOutsideMaskValue (int x) {m_outsideMaskValue=x;};

    void setInitialDistance (double x) {m_initialDistance=x;};

    void setTimeThreshold (int x) {m_timeThreshold=x;};

    void setStoppingTime (double x) {m_stoppingTime=x;};

    void setMultiplier (double x) {m_multiplier=x;};

    void setMean (double x) {m_mean=x;};

    void setVariance (double x) {m_variance=x;};

    void setConstant (double x) {m_constant=x;};

    void setAlpha (double x) {m_alpha=x;};

    void setLowerVentriclesThreshold (int x) {m_lowerVentriclesThreshold=x;};

    void setUpperVentriclesThreshold (int x) {m_upperVentriclesThreshold=x;};


private:

    ///Volum que volem segmentar
    Volume* m_Volume;

    ///Volum on guardarem el resultat de la segmetnaci�que volem segmentar
    Volume* m_Mask;
    Volume* m_filteredInputImage;

    ///Posici�de la llavor
    double m_px, m_py, m_pz;

    ///llindars de l'histograma
    int m_lowerThreshold, m_upperThreshold;

    ///llindars de l'histograma
    int m_lowerVentriclesThreshold, m_upperVentriclesThreshold;

    ///valors interns i externs de la m�cara
    int m_insideMaskValue, m_outsideMaskValue;

    ///valors interns i externs de la m�cara
    double m_volume;
    int    m_cont;
    int    m_edemaCont;


    ///valors mètode edema
    double m_initialDistance;
    int  m_timeThreshold;
    double m_stoppingTime;
    double m_multiplier;
    double m_mean;
    double m_variance;
    double m_constant;
    double m_alpha;

    ///Retorna quants voxels != de 0 hi ha a la m�cara
    int computeSizeMask();

};

}

#endif
