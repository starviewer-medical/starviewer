/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "perfusionmapcalculatormainthread.h"
#include "perfusionmapcalculatorthread.h"

#include "logging.h"
#include "series.h"
#include "volume.h"
#include "mathtools.h" // pel PI

// Qt
#include <QTime>
#include <QPair>
// VTK
#include <vtkMultiThreader.h>
// ITK
#include <itkCastImageFilter.h>
#include <itkVnlFFTRealToComplexConjugateImageFilter.h>
#include <itkVnlFFTComplexConjugateToRealImageFilter.h>

//Fourier Transform
//#include <fftw3.h>
//#include <complex>
#include <cmath> // pel ceil

namespace udg {

const double PerfusionMapCalculatorMainThread::TE = 25.0;
const double PerfusionMapCalculatorMainThread::TR = 1.5;

PerfusionMapCalculatorMainThread::PerfusionMapCalculatorMainThread( QObject *parent )
 : QObject( parent ), m_DSCVolume(0), m_map0Volume(0), m_map1Volume(0), m_map2Volume(0), reg_fact(1.0), reg_exp(2.0)
{
}


PerfusionMapCalculatorMainThread::~PerfusionMapCalculatorMainThread()
{
}

void PerfusionMapCalculatorMainThread::stop()
{
    m_stopped = true;
}


void PerfusionMapCalculatorMainThread::run()
{
    Q_ASSERT( m_DSCVolume );

    QTime time;
    int deltaRtime = 0;
    int momentstime = 0;
    int findAiftime = 0;
    int computePerfusiontime = 0;
    time.restart();
    DEBUG_LOG("Compute deltaR");
    this->computeDeltaR();
    deltaRtime += time.elapsed();
    time.restart();
    DEBUG_LOG("Compute Moments");
    this->computeMoments();
    momentstime += time.elapsed();
    time.restart();
    DEBUG_LOG("Find AIF");
    this->findAIF();
    findAiftime += time.elapsed();
    time.restart();
    DEBUG_LOG("Compute Perfusion");
    //return;
    this->computePerfusion();
    DEBUG_LOG("Done!");
    computePerfusiontime += time.elapsed();
    DEBUG_LOG(QString( "TEMPS COMPUTANT DELTAR : %1ms " ).arg( deltaRtime ) );
    DEBUG_LOG(QString( "TEMPS COMPUTANT MOMENTS : %1ms " ).arg( momentstime ) );
    DEBUG_LOG(QString( "TEMPS COMPUTANT FINDING AIF : %1ms " ).arg( findAiftime ) );
    DEBUG_LOG(QString( "TEMPS COMPUTANT PERFUSION TIME : %1ms " ).arg( computePerfusiontime ) );
    emit computed();
}

void PerfusionMapCalculatorMainThread::computeDeltaR( )
{
    //TODO with threads!!! --> pasar a l'altre classe

    if(!m_DSCVolume)
    {
        return;
    }

    static const int Nbaselinestart = 1;
    static const int Nbaselineend = 10; //9 + 1;

    DoubleTemporalImageType::RegionType regiont;
    DoubleTemporalImageType::IndexType startt;
    startt[0]=0;
    startt[1]=0;
    startt[2]=0;
    startt[3]=0;

    DoubleTemporalImageType::SizeType sizet;
    sizet[0] = m_DSCVolume->getNumberOfPhases();  //les mostres temporals
    sizet[1] = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize()[0];  //les X
    sizet[2] = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize()[0];  //les Y
    sizet[3] = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();  //les Z
    //Ho definim així perquè l'iterador passi per totes les mostres temporals
    regiont.SetSize(sizet);
    regiont.SetIndex(startt);

    QTime time;
    int time1 = 0;
    int time2 = 0;

    deltaRImage = DoubleTemporalImageType::New();
    deltaRImage->SetRegions( regiont );
    deltaRImage->Allocate();

    Volume::ItkImageType::Pointer  inputImage = m_DSCVolume->getItkData();

    int i,j,k,t;
    int iend = m_DSCVolume->getDimensions()[0];
    int jend = m_DSCVolume->getDimensions()[1];
    int kend = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    int tend = m_DSCVolume->getNumberOfPhases();

    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize();
    //TODO: s'hauria de fer que només agafés el nombre de llesques (i no les phases)
    size[2]=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);

    checkImage = BoolImageType::New();
    checkImage->SetRegions( region );
    checkImage->Allocate();

    typedef itk::ImageRegionIterator<DoubleTemporalImageType> DoubleTempIterator;
    DoubleTempIterator imIter( deltaRImage, deltaRImage->GetBufferedRegion() );

    //Iniciem els Threads per calcular la check image
    time.restart();
    int numberOfThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();  /// \todo QThread::idealThreadCount() amb Qt >= 4.3
    DEBUG_LOG(QString("Number of threads = %1").arg(numberOfThreads));
    QVector<PerfusionMapCalculatorThread *> threads(numberOfThreads);

    for ( int i = 0; i < numberOfThreads; i++ )
    {
        PerfusionMapCalculatorThread * thread = new PerfusionMapCalculatorThread( i, numberOfThreads );
        thread->setDSCImage( m_DSCVolume->getItkData() );
        thread->setVolumeSize(iend, jend, kend, tend);
        thread->setCheckImage( checkImage );
        thread->setCheckImageMode();
        threads[i] = thread;
    }

    // iniciem els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        PerfusionMapCalculatorThread * thread = threads[j];
        thread->start();
    }

    // esperem que acabin els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        threads[j]->wait();
    }
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        delete threads[j];
    }

    //S'acaba els threads check image
    Volume::ItkImageType::IndexType index;
    Volume::ItkImageType::IndexType indexVoxel;
    int slice;
    QVector<signed int> timeseries(tend);
    double meanbl, stdbl;
    double min;
    bool valid;
    //std::ofstream fout("rcbv.dat", ios::out);
    double value;
/*    imIter.GoToBegin();
    m_aif = QVector<double>(tend);

    //unim resultats
    for (k=0;k<kend;k++)
    {
        indexVoxel[2]=k;
        for (j=0;j<jend;j++)
        {
            indexVoxel[1]=j;
            for (i=0;i<iend;i++)
            {
                indexVoxel[0]=i;
                checkImage->SetPixel(indexVoxel,threads[(i*numberOfThreads)/iend]->getCheckImage()->GetPixel(indexVoxel));
            }
        }
    }*/
    DEBUG_LOG("Done check image");
    //Init check image ABANS!!
    //Calculem els pixels vàlids
/*    for (k=0;k<kend;k++)
    {
        slice=k*tend;
        indexVoxel[2]=k;
        for (j=0;j<jend;j++)
        {
            index[1]=j;
            indexVoxel[1]=j;
            for (i=0;i<iend;i++)
            {
                index[0]=i;
                indexVoxel[0]=i;
                min=10e6;
                for (t=0;t<tend;t++)
                {
                    index[2]=slice+t;
                    //timeseries[t] = m_DSCVolume->getItkData()->GetPixel(index);
                    timeseries[t] = inputImage->GetPixel(index);
                    if(timeseries[t]<min)
                    {
                        min=timeseries[t];
                    }
                }
                meanbl = 0.0;
                for (t=Nbaselinestart;t<Nbaselineend;t++)
                {
                    meanbl += timeseries[t];
                }
                meanbl = meanbl / (double)(Nbaselineend - Nbaselinestart);
                stdbl = 0.0;
                for (t=Nbaselinestart;t<Nbaselineend;t++)
                {
                    stdbl += (timeseries[t]-meanbl)*(timeseries[t]-meanbl);
                }
                stdbl = sqrt(stdbl / (double)(Nbaselineend - Nbaselinestart - 1));
                //SNR of 10 at least --> else the voxel is discarded
                valid = (meanbl > 10*stdbl)&&(stdbl > 0)&&(min > 3*stdbl);
                checkImage->SetPixel(indexVoxel,valid);
            }
        }
    }*/
    time1 += time.elapsed();
    time.restart();

    // iniciem els threads
    for ( int i = 0; i < numberOfThreads; i++ )
    {
        PerfusionMapCalculatorThread * thread = new PerfusionMapCalculatorThread( i, numberOfThreads );
        thread->setDSCImage( m_DSCVolume->getItkData() );
        thread->setVolumeSize(iend, jend, kend, tend);
        thread->setCheckImage( checkImage );
        thread->setDeltaRImage( deltaRImage );
        thread->setDeltaRImageMode();
        threads[i] = thread;
    }
    // iniciem els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        PerfusionMapCalculatorThread * thread = threads[j];
        thread->start();
    }

    // esperem que acabin els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        threads[j]->wait();
    }

    //S'acaba els threads deltaR image

/*    //Init deltaR image ABANS!!
    typedef itk::ImageRegionIterator<BoolImageType> BoolIterator;
    BoolIterator boolIter( checkImage, checkImage->GetBufferedRegion() );

    boolIter.GoToBegin();
    imIter.GoToBegin();

    for (k=0;k<kend;k++)
    {
        slice=k*tend;
        indexVoxel[2]=k;
        for (j=0;j<jend;j++)
        {
            index[1]=j;
            indexVoxel[1]=j;
            for (i=0;i<iend;i++)
            {
                index[0]=i;
                min=10e6;
                indexVoxel[0]=i;
                if(boolIter.Get())
                {
                    for (t=0;t<tend;t++)
                    {
                        index[2]=slice+t;
                        //timeseries[t] = m_DSCVolume->getItkData()->GetPixel(index);
                        timeseries[t] = inputImage->GetPixel(index);
                    }
                    meanbl = 0.0;
                    for (t=Nbaselinestart;t<Nbaselineend;t++)
                    {
                        meanbl += timeseries[t];
                    }
                    meanbl = meanbl / (double)(Nbaselineend - Nbaselinestart);
                    for (t=0;t<tend;t++)
                    {
                        value = -log(timeseries[t]/meanbl)/TE;
                        //Sense iteradors
                        //index[2]=slice+t;
                        //deltaRImage->SetPixel(index, value);
                        //Amb iteradors
                        imIter.Set(value);
                        ++imIter;
                        //fout<<value<<std::endl;
                    }
                }
                else
                {
                    for (t=0;t<tend;t++)
                    {
                        //Sense iteradors
                        //index[2]=slice+t;
                        //deltaRImage->SetPixel(index, 0.0);
                        //Amb iteradors
                        imIter.Set(0.0);
                        ++imIter;
                    }
                }
                if(checkImage->GetPixel(indexVoxel) != boolIter.Get() ) 
                {
                    std::cout<<"."<<std::endl;
                }
                ++boolIter;
            }
        }
    }*/
    time2 += time.elapsed();

    DEBUG_LOG(QString( "-- TEMPS Computant checkImage : %1ms " ).arg( time1 ) );
    DEBUG_LOG(QString( "-- TEMPS Computant deltaR : %1ms " ).arg( time2 ) );

    // destruïm els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        delete threads[j];
    }
    //fout.close();
    this->computeMeanDeltaRPerSlice();
}

void PerfusionMapCalculatorMainThread::computeMeanDeltaRPerSlice( )
{
    int i,j,k,t;
    //prenem la i perquè sol ser el valor més gran
    //TODO:Compte quan un no és múltiple de l'altre!!!
    int iend = m_DSCVolume->getDimensions()[0];
    int jend = m_DSCVolume->getDimensions()[1];
    int kend = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    int tend = m_DSCVolume->getNumberOfPhases();
    m_meanseries = QVector<QVector<double> > (kend,QVector<double> (tend,0.0));
    QVector<signed int > contseries(kend,0);

    typedef itk::ImageRegionIteratorWithIndex<BoolImageType> BoolIterator;
    BoolIterator boolIter( checkImage, checkImage->GetBufferedRegion() );

    typedef itk::ImageRegionIteratorWithIndex<DoubleTemporalImageType> DoubleTempIterator;
    DoubleTempIterator imIter( deltaRImage, deltaRImage->GetBufferedRegion() );

    boolIter.GoToBegin();
    imIter.GoToBegin();

    for (k=0;k<kend;k++)
    {
        for (j=0;j<jend;j++)
        {
            for (i=0;i<iend;i++)
            {
                if( boolIter.Get() )
                {
                    for (t=0;t<tend;t++)
                    {
                        m_meanseries[k][t] += (double)imIter.Get( );
                        ++imIter;
                    }
                    contseries[k]++;
                }
                else
                {
                    for (t=0;t<tend;t++)
                    {
                        ++imIter;
                    }
                }
                ++boolIter;
            }
        }
    }
    for (k=0;k<kend;k++)
    {
        for (t=0;t<tend;t++)
        {
            m_meanseries[k][t] = m_meanseries[k][t]/(double)contseries[k];
        }
    }
}

void PerfusionMapCalculatorMainThread::computeMoments( )
{
    if(!m_DSCVolume)
    {
        return;
    }

    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize();
    size[2]=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    m0Image = DoubleImageType::New();
    m0Image->SetRegions( region );
    m0Image->Allocate();

    m1Image = DoubleImageType::New();
    m1Image->SetRegions( region );
    m1Image->Allocate();

    m2Image = DoubleImageType::New();
    m2Image->SetRegions( region );
    m2Image->Allocate();

    int i,j,k,t;
    int iend = m_DSCVolume->getDimensions()[0];
    int jend = m_DSCVolume->getDimensions()[1];
    int kend = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    int tend = m_DSCVolume->getNumberOfPhases();
    DoubleTemporalImageType::IndexType index;
    Volume::ItkImageType::IndexType indexMoment;
    //int slice;
    QVector<double> timeseries(tend);
    double m0,m1,m2;
//     std::ofstream fout0("m0.dat", ios::out);
//     std::ofstream fout1("m1.dat", ios::out);
//     std::ofstream fout2("m2.dat", ios::out);

    typedef itk::ImageRegionIterator<DoubleTemporalImageType> DoubleTempIterator;
    DoubleTempIterator imIter( deltaRImage, deltaRImage->GetBufferedRegion() );
    imIter.GoToBegin();
    int contvalid=0;
    int contm0=0;

    for (k=0;k<kend;k++)
    {
        index[3]=k;
        indexMoment[2]=k;
        for (j=0;j<jend;j++)
        {
            index[2]=j;
            indexMoment[1]=j;
            for (i=0;i<iend;i++)
            {
                index[1]=i;
                indexMoment[0]=i;
                if(checkImage->GetPixel(indexMoment))
                {
                    //slice=k*tend;
                    for (t=0;t<tend;t++)
                    {
                        //Sense iteradors
                        //index[2]=slice+t;
                        //timeseries[t] = deltaRImage->GetPixel(index);
                        //Amb iteradors
                        timeseries[t] = imIter.Get();
                        //std::cout<<imIter.Get()<<" ";
                        ++imIter;
                    }
                    //std::cout<<std::endl;
                    this->computeMomentsVoxel(timeseries, m0,m1,m2);
                    if(m0<=0.0)
                    {
                        //std::cout<<"m's = 0"<<std::endl;
                        contm0++;
                        m0=0.0;
                        m1=0.0;
                        m2=0.0;
                    }
                    contvalid++;
                }
                else
                {
                    m0=0.0;
                    m1=0.0;
                    m2=0.0;
                    //posem l'iterador a la posició que toca //Segurament millorable
                    for (t=0;t<tend;t++)
                    {
                        ++imIter;
                    }
                }
                //fout0<<m0<<std::endl;
                //fout1<<m1<<std::endl;
                //fout2<<m2<<std::endl;
                m0Image->SetPixel(indexMoment, m0);
                m1Image->SetPixel(indexMoment, m1);
                m2Image->SetPixel(indexMoment, m2);
            }
        }
    }
    DEBUG_LOG(QString("A compute moments hi ha %1 voxels valids").arg(contvalid));
    DEBUG_LOG(QString("A compute moments hi ha %1 voxels m0").arg(contm0));
    DEBUG_LOG(QString("A compute moments hi ha %1 voxels molt valids").arg(contvalid-contm0));
/*    fout0.close();
    fout1.close();
    fout2.close();*/
}

void PerfusionMapCalculatorMainThread::findAIF( )
{
    static const int firstSelection = 100;
    static const int secondSelection = 10;
    int i,j,k;
    int iend = m_DSCVolume->getDimensions()[0];
    int jend = m_DSCVolume->getDimensions()[1];
    int kend = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    Volume::ItkImageType::IndexType index;
    int indexint;
    double value;//, valuem2;
    m_aif = QVector<double>( m_DSCVolume->getNumberOfPhases() );

    QVector< QPair< double, int > > sortedMoment1;
    QVector< QPair< double, int > > sortedMoment0;
    int contm0=0;
    for (i=0;i<iend;i++)
    {
        index[0]=i;
        for (j=0;j<jend;j++)
        {
            index[1]=j;
            for (k=0;k<kend;k++)
            {
                index[2]=k;
                if(checkImage->GetPixel(index))
                {
                    value =  m0Image->GetPixel(index);
                    contm0++;
                    //valuem2 =  m2Image->GetPixel(index);    //excloem els que tenen també una m2 negativa
                    if(value > 0.000001 )
                    {
                        indexint = i + j*iend + k*jend*iend;
                        sortedMoment0 << qMakePair( value, indexint);
                    }
                }
            }
        }
    }
    qSort( sortedMoment0 );    // sort in ascending order
    DEBUG_LOG(QString("sortedMoment 0 size: %1").arg(sortedMoment0.size()));
    DEBUG_LOG(QString("A sorted moments0 hi ha %1 voxels").arg(contm0));

    for (i=sortedMoment0.size()-firstSelection;i<sortedMoment0.size();i++)
    {
        index[0] = sortedMoment0[i].second % iend;
        index[1] = (int)(sortedMoment0[i].second/iend) % jend;
        index[2] = (int)(sortedMoment0[i].second/(iend*jend));
        value =  m1Image->GetPixel(index);
        if(value > 2.000001)
        {
            sortedMoment1 << qMakePair( value, sortedMoment0[i].second);
        }
    }
    ///TODO: search if it's possible to sort descending
    qSort( sortedMoment1 );    // sort in ascending order
    DEBUG_LOG(QString("sortedMoment 1 size: %1").arg(sortedMoment1.size()));

/*    for (i=0;i<sortedMoment1.size();i++)
    {
        std::cout<<sortedMoment1[i].first<<" "<<sortedMoment1[i].second<<" // "<<std::endl;
    }
    std::cout<<std::endl;*/
    //std::cout<<"MAXIMUM MOMENT 0: "<<sortedMoment1[secondSelection].first<<" "<<sortedMoment1[ secondSelection].second<<index<<std::endl;

    //Inicialize the minimum
    index[0] = sortedMoment1[0].second % iend;
    index[1] = (int)(sortedMoment1[0].second/iend) % jend;
    index[2] = (int)(sortedMoment1[0].second/(iend*jend));

    double minm2 =  m2Image->GetPixel(index);
    int minposm2 = sortedMoment0[0].second;
    //If minm2 is 0 then we put a huge value in it
    if(minm2 < 1e-6)
    {
        minm2 = 1e6;
    }

    for (i = 1; i <  secondSelection;i++)
    {
        index[0] = sortedMoment1[i].second % iend;
        index[1] = (int)(sortedMoment1[i].second/iend) % jend;
        index[2] = (int)(sortedMoment1[i].second/(iend*jend));
        value =  m2Image->GetPixel(index);
        if(value < minm2 && value > 1e-6)
        {
            minm2 = value;
            minposm2 = sortedMoment1[i].second;
        }
    }
    index[0] = minposm2 % iend;
    index[1] = (int)(minposm2/iend) % jend;
    index[2] = (int)(minposm2/(iend*jend));

    m_m0aif = m0Image->GetPixel(index);

    DoubleTemporalImageType::IndexType indexTemp;
    indexTemp[1] = index[0];
    indexTemp[2] = index[1];
    indexTemp[3] = index[2];
    int t, tend = m_DSCVolume->getNumberOfPhases();
    DEBUG_LOG(QString("Tend AIF: %1").arg(tend));
    for (t=0;t<tend;t++)
    {
        indexTemp[0] = t;
        m_aif[t] = deltaRImage->GetPixel(indexTemp);
    }

    this->fftAIF();
    this->getOmega();
}

void PerfusionMapCalculatorMainThread::computePerfusion( )
{
    QTime time;
    int time1 = 0;
    int time2 = 0;
    time.restart();
    Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size = m_DSCVolume->getItkData()->GetBufferedRegion().GetSize();
    size[2]=m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    region.SetSize(size);
    region.SetIndex(start);
    cbfImage = DoubleImageType::New();
    cbfImage->SetRegions( region );
    cbfImage->Allocate();

    cbvImage = DoubleImageType::New();
    cbvImage->SetRegions( region );
    cbvImage->Allocate();

    mttImage = DoubleImageType::New();
    mttImage->SetRegions( region );
    mttImage->Allocate();

    Volume::ItkImageType::Pointer map0Image = Volume::ItkImageType::New();
    map0Image->SetRegions( region );
    map0Image->Allocate();

    Volume::ItkImageType::Pointer map1Image = Volume::ItkImageType::New();
    map1Image->SetRegions( region );
    map1Image->Allocate();

    Volume::ItkImageType::Pointer map2Image = Volume::ItkImageType::New();
    map2Image->SetRegions( region );
    map2Image->Allocate();

    //With threads!!!
/*
    int numberOfThreads = vtkMultiThreader::GetGlobalDefaultNumberOfThreads();  /// \todo QThread::idealThreadCount() amb Qt >= 4.3
    //numberOfThreads = 1;
    std::cout<<"Number of threads = "<<numberOfThreads<<std::endl;
    QVector<PerfusionMapCalculatorThread *> threads(numberOfThreads);
    int iend = m_DSCVolume->getDimensions()[0];
    int jend = m_DSCVolume->getDimensions()[1];
    int kend = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    int tend = m_DSCVolume->getNumberOfPhases();

    for ( int i = 0; i < numberOfThreads; i++ )
    {
        PerfusionMapCalculatorThread * thread = new PerfusionMapCalculatorThread( i, numberOfThreads );
        thread->setVolumeSize(iend, jend, kend, tend);
        thread->setCheckImage( checkImage );
        thread->setDeltaRImage( deltaRImage );
        thread->setm0Aif(m_m0aif);
        thread->setFFTAif(fftaifreal,fftaifimag);
        thread->setOmega(omega);
        thread->setm0Image(m0Image);
        thread->setCBVImage(cbvImage);
        thread->setCBFImage(cbfImage);
        thread->setMTTImage(mttImage);
        thread->setCBVMapImage(map0Image);
        thread->setCBFMapImage(map2Image);
        thread->setMTTMapImage(map1Image);
        thread->setPerfusionImageMode();
        threads[i] = thread;
    }

    // iniciem els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        PerfusionMapCalculatorThread * thread = threads[j];
        thread->start();
        //thread->runPerfusionImage();
    }

    // esperem que acabin els threads
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        threads[j]->wait();
    }
    for ( int j = 0; j < numberOfThreads; j++ )
    {
        delete threads[j];
    }

*/
  
    //Init com estava abans!!!!!!!!!!
    int i,j,k,t;
    int iend = m_DSCVolume->getDimensions()[0];
    int jend = m_DSCVolume->getDimensions()[1];
    int kend = m_DSCVolume->getSeries()->getNumberOfSlicesPerPhase();
    int tend = m_DSCVolume->getNumberOfPhases();
    Volume::ItkImageType::IndexType index;
    double max;
    double valueCbv, valueCbf, valueMtt;

    QVector<  double > timeseries(m_aif.size());
    QVector<  double > residueFunction(m_aif.size());

    typedef itk::ImageRegionIterator<DoubleTemporalImageType> DoubleTempIterator;
    DoubleTempIterator imIter( deltaRImage, deltaRImage->GetBufferedRegion() );
    imIter.GoToBegin();

    for (k=0;k<kend;k++)
    {
        index[2]=k;
        for (j=0;j<jend;j++)
        {
            index[1]=j;
            for (i=0;i<iend;i++)
            {
                index[0]=i;
                if(checkImage->GetPixel(index))
                {
                    for (t=0;t<tend;t++)
                    {
                        timeseries[t] = imIter.Get();
                        ++imIter;
                    }
                    this->deconvolve(timeseries, residueFunction);
                    max=residueFunction[0];
                    for (t=1;t<tend;t++)
                    {
                        if(residueFunction[t] > max)
                        {
                            max = residueFunction[t];
                        }
                    }
                    valueCbv = 100*0.7*m0Image->GetPixel(index)/m_m0aif; //in ml/100g --> Peter dixit!!
                    valueCbf = max*100*60*0.7/TR; //ml/100g*min --> Peter dixit!!
                    valueMtt = (60*valueCbv)/valueCbf; // TR (in sec.)
                    cbvImage->SetPixel(index, 10.0*valueCbv);   //JUST FOR A GOOD VISUALIZATION!!!!!!
                    map0Image->SetPixel(index, (int)(10*valueCbv));
                    cbfImage->SetPixel(index, valueCbf);
                    map1Image->SetPixel(index, (int)(valueCbf));
                    mttImage->SetPixel(index, 10.0*valueMtt);   //JUST FOR A GOOD VISUALIZATION!!!!!!
                    map2Image->SetPixel(index, (int)(10*valueMtt));
                }
                else
                {
                    cbvImage->SetPixel(index, 0.0);
                    cbfImage->SetPixel(index, 0.0);
                    mttImage->SetPixel(index, 0.0);
                    map0Image->SetPixel(index, 0);
                    map1Image->SetPixel(index, 0);
                    map2Image->SetPixel(index, 0);
                    for (t=0;t<tend;t++)
                    {
                       ++imIter;
                    }
                }
            }
        }
    }

    time1 += time.elapsed();
    time.restart();

    //CBV
    m_map0Volume = new Volume();
    m_map0Volume->setImages( m_DSCVolume->getPhaseImages(0) );
    //m_map0Volume->setImages( m_DSCVolume->getImages() );
    m_map0Volume->setData(map0Image);

    //MTT
    m_map1Volume = new Volume();
    m_map1Volume->setImages( m_DSCVolume->getPhaseImages(0) );
    m_map1Volume->setData(map2Image);

    //CBF
    m_map2Volume = new Volume();
    m_map2Volume->setImages( m_DSCVolume->getPhaseImages(0) );
    m_map2Volume->setData(map1Image);

    time2 += time.elapsed();
    DEBUG_LOG(QString("Done!!"));

    DEBUG_LOG(QString( "-- TEMPS COMPUTANT Perfusion : %1ms " ).arg( time1 ) );
    DEBUG_LOG(QString( "-- TEMPS PINTANT Perfusion : %1ms " ).arg( time2 ) );
}

void PerfusionMapCalculatorMainThread::fftAIF( )
{
    fftaifreal = QVector<double>(m_aif.size());
    fftaifimag = QVector<double>(m_aif.size());
//     //Usant les fftw
//     std::cout<<"Usant fftw"<<std::endl;
//     std::cout<<"Size: "<<std::endl;
//     std::cout<<aif.size()<<std::endl;
//
//     fftw_complex* in;
//     fftw_complex* out;
//
//     in = new fftw_complex[aif.size()];
//     out = new fftw_complex[aif.size()];
//
//     fftw_plan pf;
//
//     pf = fftw_plan_dft_1d(aif.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
//
//     int i;
//     for(i=0;i<aif.size();i++)
//     {
//         in[i][0]=aif[i];
//         in[i][1]=0.0;
//     }
//     fftw_execute(pf);
//     for(i=0;i<aif.size();i++)
//     {
//         fftaifreal[i]=out[i][0];
//         fftaifimag[i]=out[i][1];
//     }
//
//     fftw_destroy_plan(pf);
//     free(in);
//     free(out);


    //Usant itk's
    //std::cout<<"Usant itk's"<<std::endl;
    typedef itk::Image< double, 1 > VectorImageType;
    VectorImageType::RegionType region;
    VectorImageType::IndexType start;
    start[0]=0;
    VectorImageType::SizeType size;
    size[0] = m_aif.size();  //les mostres temporals
    region.SetSize(size);
    region.SetIndex(start);

    VectorImageType::Pointer aifImage = VectorImageType::New();
    aifImage->SetRegions( region );
    aifImage->Allocate();

    typedef itk::ImageRegionIterator<VectorImageType> VectorIterator;
    VectorIterator aifIter( aifImage, aifImage->GetBufferedRegion() );

    typedef itk::VnlFFTRealToComplexConjugateImageFilter< double, 1 >  FFTFilterType;
    FFTFilterType::Pointer fftFilter = FFTFilterType::New();

    int i;
    aifIter.GoToBegin();
    for(i=0;i<m_aif.size();i++)
    {
        aifIter.Set(m_aif[i]);
        ++aifIter;
    }

    fftFilter->SetInput( aifImage );

    try
        {
        fftFilter->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        //std::cerr << "Error: " << std::endl;
        //std::cerr << excp << std::endl;
        return;
        }

    typedef itk::ImageRegionIterator<FFTFilterType::OutputImageType> ComplexIterator;
    ComplexIterator fftaifIter( fftFilter->GetOutput(), fftFilter->GetOutput()->GetBufferedRegion() );
    fftaifIter.GoToBegin();

    for(i=0;i<m_aif.size();i++)
    {
        fftaifreal[i]=fftaifIter.Get().real();
        fftaifimag[i]=fftaifIter.Get().imag();
        ++fftaifIter;
    }
}

void PerfusionMapCalculatorMainThread::deconvolve( QVector<double> tissue, QVector<double>& residuefunc)
{
    typedef std::complex<double> complexd;
    complexd num1, num2, num3;

//     //Usant fftw
//     fftw_complex* in;
//     fftw_complex* out;
//
//     in = new fftw_complex[tissue.size()];
//     out = new fftw_complex[tissue.size()];
//
//     fftw_plan pf, pb;
//
//     pf = fftw_plan_dft_1d(tissue.size(), in, out, FFTW_FORWARD, FFTW_ESTIMATE);
//     pb = fftw_plan_dft_1d(tissue.size(), in, out, FFTW_BACKWARD, FFTW_ESTIMATE);
//
//     int i;
//     for(i=0;i<tissue.size();i++)
//     {
//         in[i][0]=tissue[i];
//         in[i][1]=0.0;
//     }
//
//     fftw_execute(pf);
//
//     for(i=0;i<tissue.size();i++)
//     {
//         num1=complexd(fftaifreal[i],fftaifimag[i]);
//         num2=complexd(out[i][0],out[i][1]);
//
//         if((reg_fact > 1e-6) || ((fabs(num1.real()) + fabs(num1.imag()))> 1e-6))
//         {
//             num3 = num2* (conj(num1) / (num1*conj(num1) + reg_fact*pow(-1,reg_exp)*pow(omega[i],2*reg_exp)));
//             in[i][0] = num3.real();
//             in[i][1] = num3.imag();
//         }
//         else
//         {
//             in[i][0] = 0.0;
//             in[i][1] = 0.0;
//         }
//     }
//     fftw_execute(pb);
//     for(i=0;i<tissue.size();i++)
//     {
//         residuefunc[i]=out[i][0]/tissue.size();
//     }
//     fftw_destroy_plan(pf);
//     fftw_destroy_plan(pb);
//     free(in);
//     free(out);

    //Usant itk's
    //std::cout<<"Usant itk's"<<std::endl;
    typedef itk::Image< double, 1 > VectorImageType;
    VectorImageType::RegionType region;
    VectorImageType::IndexType start;
    start[0]=0;
    VectorImageType::SizeType size;
    size[0] = m_aif.size();  //les mostres temporals
    region.SetSize(size);
    region.SetIndex(start);

    VectorImageType::Pointer tissueImage = VectorImageType::New();
    tissueImage->SetRegions( region );
    tissueImage->Allocate();

    typedef itk::ImageRegionIterator<VectorImageType> VectorIteratorType;
    VectorIteratorType tissueIter( tissueImage, tissueImage->GetBufferedRegion() );

    typedef itk::VnlFFTRealToComplexConjugateImageFilter< double, 1 >  FFTFilterType;
    FFTFilterType::Pointer fftFilter = FFTFilterType::New();

    int i;
    tissueIter.GoToBegin();

    for(i=0;i<tissue.size();i++)
    {
        tissueIter.Set(tissue[i]);
        ++tissueIter;
    }
    fftFilter->SetInput( tissueImage );

    try
        {
        fftFilter->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        //std::cerr << "Error: " << std::endl;
        //std::cerr << excp << std::endl;
        return;
        }

    typedef FFTFilterType::OutputImageType ComplexImageType;
    ComplexImageType::Pointer residualFFTImage = ComplexImageType::New();
    residualFFTImage->SetRegions( region );
    residualFFTImage->Allocate();

    typedef itk::ImageRegionIterator<ComplexImageType> ComplexIteratorType;
    ComplexIteratorType fftTissueIter( fftFilter->GetOutput(), fftFilter->GetOutput()->GetBufferedRegion() );
    fftTissueIter.GoToBegin();

    ComplexIteratorType fftResidualIter( residualFFTImage, residualFFTImage->GetBufferedRegion() );
    fftResidualIter.GoToBegin();

    for(i=0;i<tissue.size();i++)
    {
        num1=complexd(fftaifreal[i],fftaifimag[i]);
        num2=complexd(fftTissueIter.Get().real(),fftTissueIter.Get().imag());

        if((reg_fact > 1e-6) || ((fabs(num1.real()) + fabs(num1.imag()))> 1e-6))
        {
            num3 = num2* (conj(num1) / (num1*conj(num1) + reg_fact*pow(-1,reg_exp)*pow(omega[i],2*reg_exp)));
            fftResidualIter.Set(num3);
        }
        else
        {
            num3 = complexd( 0.0, 0.0 );
            fftResidualIter.Set(num3);
        }
        ++fftTissueIter;
        ++fftResidualIter;
    }

    typedef itk::VnlFFTComplexConjugateToRealImageFilter< double, 1 >  IFFTFilterType;
    IFFTFilterType::Pointer fftInverseFilter = IFFTFilterType::New();
    fftInverseFilter->SetInput( residualFFTImage );

    try
        {
        fftInverseFilter->Update();
        }
    catch( itk::ExceptionObject & excp )
        {
        //std::cerr << "Error: " << std::endl;
        //std::cerr << excp << std::endl;
        return;
        }

    VectorIteratorType residualIter( fftInverseFilter->GetOutput(), fftInverseFilter->GetOutput()->GetBufferedRegion() );
    residualIter.GoToBegin();

    for(i=0;i<tissue.size();i++)
    {
        //if(residuefunc[i]!=residualIter.Get()) std::cout<<"Resultat residuefunc diferent: "<<residuefunc[i]<<" ," <<residualIter.Get()/tissue.size()<<std::endl;

        residuefunc[i]=residualIter.Get();
        ++residualIter;
    }

}

void PerfusionMapCalculatorMainThread::computeMomentsVoxel( QVector<double> v, double &m0, double &m1, double &m2)
{
    int i;
    m0=0.0;
    m1=0.0;
    m2=0.0;
    //std::cout<<"compute moments size "<<v.size()<<std::endl;
    for(i=0;i<v.size();i++)
    {
        m0+=v[i];
    }

    for(i=0;i<v.size();i++)
    {
        m1+=i*v[i];
    }
    if(m1>0 && m1<m0*(double)v.size())
    {
        m1= m1/m0;
    }
    else
    {
        m1=0;
    }

    for(i=0;i<v.size();i++)
    {
        m2+=(i-m1)*(i-m1)*v[i];
    }
    if(m2>0)
    {
        m2= sqrt(m2/m0);
    }
    else
    {
        m2=0;
    }
    if(m2>(double)v.size()/2.0)
    {
        m2=0;
    }

}

void PerfusionMapCalculatorMainThread::getOmega()
{
    //! returns omega axis for fft for dt=1
    omega = QVector<double>(m_aif.size());

    int i, index;
    double  omega_max;

    omega_max = MathTools::PI; // divide omega by Delta t for scaling according to sampling

    index = static_cast<int>( ceil(m_aif.size()/2.0) ) + 1;

    for(i=0; i<index; i++)
    {
        omega[i] = (double)i/(double)(index-1)*omega_max;
    }
    for(i=index; i<m_aif.size(); i++)
    {
        omega[i] = -(double)(m_aif.size()-i)/(double)(index-1)*omega_max;
    }

    return;
}


}
