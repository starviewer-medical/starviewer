/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGPERFUSIONMAPCALCULATORMAINTHREAD_H
#define UDGPERFUSIONMAPCALCULATORMAINTHREAD_H

#include <itkImage.h>

#include <QThread>
#include <QVector>

namespace udg {

class Volume;
/**
 * Thread principal per al càlcul d'obscurances. Controla els altres threads.
 */
class PerfusionMapCalculatorMainThread : public QObject {

    Q_OBJECT

public:
    typedef itk::Image<bool, 3> BoolImageType;
    typedef itk::Image<double, 3> DoubleImageType;
    typedef itk::Image<double, 4> DoubleTemporalImageType;

    PerfusionMapCalculatorMainThread( QObject *parent = 0 );
    virtual ~PerfusionMapCalculatorMainThread();

    void setDSCVolume(Volume* volume){m_DSCVolume=volume;}
    Volume* getCBVVolume(){return m_map0Volume;}
    Volume* getCBFVolume(){return m_map2Volume;}
    Volume* getMTTVolume(){return m_map1Volume;}
    DoubleImageType::Pointer getCBVImage(){return cbvImage;}
    DoubleImageType::Pointer getCBFImage(){return cbfImage;}
    DoubleImageType::Pointer getMTTImage(){return mttImage;}
    DoubleTemporalImageType::Pointer getDeltaRImage(){return deltaRImage;}
    BoolImageType::Pointer getCheckImage(){return checkImage;}

    QVector<double> getAIF(){return m_aif;}
    QVector<int> getAIFIndex(){return m_aifIndex;}

    QVector<QVector<double> > getMeanDeltaRPerSlice(){return m_meanseries;}

public slots:

    void stop();
    void run();

    void setAIFIndex(double x,double y,double z)
            {m_aifIndex[0]=x;m_aifIndex[1]=y;m_aifIndex[2]=z;m_AIFIsSet=true;}

signals:

    void progress( int percent );
    void computed();

//protected:


private:
    static const double TE;
    static const double TR;

    void computeDeltaR( );
    void computeMeanDeltaRPerSlice( );
    void computeMoments( );
    //Xapussa: posem directament el tipus dels voxels del Volume
    void computeMomentsVoxel( QVector<double> v, double &m0, double &m1, double &m2);
    void findAIF( );
    void updateAIF( );
    void fftAIF( );
    void getOmega( );
    void computePerfusion( );
    void deconvolve( QVector<double> tissue, QVector<double>& residuefunc);
    void changeMap( int value );


    Volume *m_DSCVolume;
    Volume* m_map0Volume;
    Volume* m_map1Volume;
    Volume* m_map2Volume;

    BoolImageType::Pointer checkImage;
    DoubleTemporalImageType::Pointer deltaRImage;
    //DoubleImageType::Pointer deltaRImage;

    DoubleImageType::Pointer m0Image;
    DoubleImageType::Pointer m1Image;
    DoubleImageType::Pointer m2Image;
    DoubleImageType::Pointer cbfImage;
    DoubleImageType::Pointer cbvImage;
    DoubleImageType::Pointer mttImage;

    QVector<double> m_aif;
    QVector<int> m_aifIndex;
    double m_m0aif;
    QVector<double> fftaifreal;
    QVector<double> fftaifimag;
    QVector<double> omega;

    QVector<QVector<double> > m_meanseries;

    double reg_fact, reg_exp;

    bool m_stopped;

    bool m_AIFIsSet;

};

}

#endif
