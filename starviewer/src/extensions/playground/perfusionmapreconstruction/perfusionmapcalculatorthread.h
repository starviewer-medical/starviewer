/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGPERFUSIONMAPCALCULATORTHREAD_H
#define UDGPERFUSIONMAPCALCULATORTHREAD_H

#include "volume.h"

#include <itkImage.h>

#include <QThread>
#include <QVector>

namespace udg {

class Volume;
/**
 * Thread que implementa els mètodes de càlcul de mapes de perfusió.
 *
 * \author Grup de Gràfics i Imatge de Girona (GILab) <vismed@ima.udg.edu>
*/
class PerfusionMapCalculatorThread : public QThread {

    Q_OBJECT

public:

    PerfusionMapCalculatorThread( int id, int numberOfThreads, QObject * parent = 0 );
    ~PerfusionMapCalculatorThread();

    typedef Volume::ItkImageType ImageType;
    typedef itk::Image<bool, 3> BoolImageType;
    typedef itk::Image<double, 3> DoubleImageType;
    typedef itk::Image<double, 4> DoubleTemporalImageType;

    void setDSCImage(ImageType::Pointer image){m_DSCImage=image;}
    void setCheckImage(BoolImageType::Pointer image){m_checkImage = image;}
    BoolImageType::Pointer getCheckImage(){return m_checkImage;}
    void setDeltaRImage(DoubleTemporalImageType::Pointer image){m_deltaRImage = image;}
    void setVolumeSize(int x,int y,int z,int t){m_sizex=x;m_sizey=y;m_sizez=z;m_sizet=t;}
    void setm0Aif(double x){m_m0aif=x;}
    void setFFTAif(QVector<double> x, QVector<double> y){fftaifreal=x; fftaifimag=y;}
    void setOmega(QVector<double> x){omega=x;}
    void setm0Image(DoubleImageType::Pointer image){m_m0Image = image;}
    void setCBVImage(DoubleImageType::Pointer image){m_cbvImage = image;}
    void setCBFImage(DoubleImageType::Pointer image){m_cbfImage = image;}
    void setMTTImage(DoubleImageType::Pointer image){m_mttImage = image;}
    void setCBVMapImage(ImageType::Pointer image){m_cbvMapImage = image;}
    void setCBFMapImage(ImageType::Pointer image){m_cbfMapImage = image;}
    void setMTTMapImage(ImageType::Pointer image){m_mttMapImage = image;}

    void setCheckImageMode();
    void setDeltaRImageMode();
    void setPerfusionImageMode();
    void runPerfusionImage();

protected:

    virtual void run();

private:
    static const double TE;
    static const double TR;

    enum{CheckImage, DeltaRImage, PerfusionImage};

    void runCheckImage();
    void runDeltaRImage();

    QVector<double> deconvolve( QVector<double> tissue);

    ImageType::Pointer m_DSCImage;
    BoolImageType::Pointer m_checkImage;
    DoubleTemporalImageType::Pointer m_deltaRImage;
    //DoubleImageType::Pointer deltaRImage;

    ImageType::Pointer m_cbfMapImage;
    ImageType::Pointer m_cbvMapImage;
    ImageType::Pointer m_mttMapImage;
    DoubleImageType::Pointer m_cbfImage;
    DoubleImageType::Pointer m_cbvImage;
    DoubleImageType::Pointer m_mttImage;

    DoubleImageType::Pointer m_m0Image;

    int m_id, m_numberOfThreads;
    int m_mode;
    int m_sizex,m_sizey,m_sizez,m_sizet;

    //QVector<double> m_aif;
    double m_m0aif;
    QVector<double> fftaifreal;
    QVector<double> fftaifimag;
    QVector<double> omega;

    double reg_fact, reg_exp;


};


}


#endif




