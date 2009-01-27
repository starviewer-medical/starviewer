/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#include "perfusionmapcalculatorthread.h"

#include "series.h"

#include <itkImageRegionIteratorWithIndex.h>
#include <itkVnlFFTRealToComplexConjugateImageFilter.h>
#include <itkVnlFFTComplexConjugateToRealImageFilter.h>


namespace udg {

const double PerfusionMapCalculatorThread::TE = 25.0;
const double PerfusionMapCalculatorThread::TR = 1.5;


PerfusionMapCalculatorThread::PerfusionMapCalculatorThread( int id, int numberOfThreads, QObject * parent )
    : QThread(parent),
      m_id( id ), m_numberOfThreads( numberOfThreads ), reg_fact(1.0), reg_exp(2.0)
{
}


PerfusionMapCalculatorThread::~PerfusionMapCalculatorThread()
{
}

void PerfusionMapCalculatorThread::setCheckImageMode()
{
    m_mode = PerfusionMapCalculatorThread::CheckImage;
}

void PerfusionMapCalculatorThread::setDeltaRImageMode()
{
    m_mode = PerfusionMapCalculatorThread::DeltaRImage;
}

void PerfusionMapCalculatorThread::setPerfusionImageMode()
{
    m_mode = PerfusionMapCalculatorThread::PerfusionImage;
}

void PerfusionMapCalculatorThread::run()
{
    switch(m_mode)
    {
        case PerfusionMapCalculatorThread::CheckImage:
            this->runCheckImage();
            break;
        case PerfusionMapCalculatorThread::DeltaRImage:
            this->runDeltaRImage();
            break;
        case PerfusionMapCalculatorThread::PerfusionImage:
            this->runPerfusionImage();
            break;
        default:
            break;
    }
}

void PerfusionMapCalculatorThread::runCheckImage()
{
    static const int Nbaselinestart = 1;
    static const int Nbaselineend = 10; //9 + 1;

    int i,j,k,t;
    //prenem la i perquè sol ser el valor més gran
    //TODO:Compte quan un no és múltiple de l'altre!!!
    int iinit = (m_id * m_sizex)/m_numberOfThreads;
    int iend = ((m_id+1)* m_sizex)/m_numberOfThreads;
    int jend = m_sizey;
    int kend = m_sizez;
    int tend = m_sizet;

    /*Volume::ItkImageType::RegionType region;
    Volume::ItkImageType::IndexType start;
    start[0]=0;
    start[1]=0;
    start[2]=0;
    Volume::ItkImageType::SizeType size;
    //només agafem el nombre de llesques (i no les phases)
    size[0]= m_sizex;
    size[1]= m_sizey;
    size[2]= m_sizez;
    region.SetSize(size);
    region.SetIndex(start);

    m_checkImage = BoolImageType::New();
    m_checkImage->SetRegions( region );
    m_checkImage->Allocate();
    */

    Volume::ItkImageType::IndexType index;
    Volume::ItkImageType::IndexType indexVoxel;
    int slice;
    QVector<signed int> timeseries(tend);
    double meanbl, stdbl;
    double min;
    bool valid;
    //std::ofstream fout("rcbv.dat", ios::out);
    double value;

    typedef itk::ImageRegionIteratorWithIndex<BoolImageType> BoolIterator;
    BoolIterator boolIter( m_checkImage, m_checkImage->GetBufferedRegion() );
    boolIter.GoToBegin();

    for (k=0;k<kend;k++)
    {
        indexVoxel[2]=k;
        slice=k*tend;
        for (j=0;j<jend;j++)
        {
            indexVoxel[1]=j;
            indexVoxel[0]=iinit;
            boolIter.SetIndex(indexVoxel);
            index[1]=j;
            for (i=iinit;i<iend;i++)
            {
                index[0]=i;
                min=10e6;
                for (t=0;t<tend;t++)
                {
                    index[2]=slice+t;
                    timeseries[t] = m_DSCImage->GetPixel(index);
                    //timeseries[t] = inputImage->GetPixel(index);
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
                //m_checkImage->SetPixel(indexVoxel,valid);
                boolIter.Set(valid);
                ++boolIter;
            }
        }
    }
}

void PerfusionMapCalculatorThread::runDeltaRImage()
{
    static const int Nbaselinestart = 1;
    static const int Nbaselineend = 10; //9 + 1;

    int i,j,k,t;
    //prenem la i perquè sol ser el valor més gran
    //TODO:Compte quan un no és múltiple de l'altre!!!
    int iinit = (m_id * m_sizex)/m_numberOfThreads;
    int iend = ((m_id+1)* m_sizex)/m_numberOfThreads;
    int jend = m_sizey;
    int kend = m_sizez;
    int tend = m_sizet;
    Volume::ItkImageType::IndexType index;
    DoubleTemporalImageType::IndexType indexTemp;
    Volume::ItkImageType::IndexType indexVoxel;
    int slice;
    QVector<signed int> timeseries(tend);
    double meanbl, stdbl;
    double min;
    bool valid;
    //std::ofstream fout("rcbv.dat", ios::out);
    double value;

    typedef itk::ImageRegionIteratorWithIndex<BoolImageType> BoolIterator;
    BoolIterator boolIter( m_checkImage, m_checkImage->GetBufferedRegion() );

    typedef itk::ImageRegionIteratorWithIndex<DoubleTemporalImageType> DoubleTempIterator;
    DoubleTempIterator imIter( m_deltaRImage, m_deltaRImage->GetBufferedRegion() );

    boolIter.GoToBegin();
    imIter.GoToBegin();

    for (k=0;k<kend;k++)
    {
        slice=k*tend;
        indexVoxel[2]=k;
        indexTemp[3]=k;
        for (j=0;j<jend;j++)
        {
            index[1]=j;
            indexVoxel[1]=j;
            indexVoxel[0]=iinit;
            boolIter.SetIndex(indexVoxel);
            indexTemp[2]=j;
            indexTemp[1]=iinit;
            indexTemp[0]=0;
            imIter.SetIndex(indexTemp);
            for (i=iinit;i<iend;i++)
            {
                index[0]=i;
                min=10e6;
                //if(m_checkImage->GetPixel(indexVoxel))
                if( boolIter.Get() )
                {
                    for (t=0;t<tend;t++)
                    {
                        index[2]=slice+t;
                        timeseries[t] = m_DSCImage->GetPixel(index);
                        //timeseries[t] = inputImage->GetPixel(index);
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
                        //indexTemp[0]=t;
                        //m_deltaRImage->SetPixel(indexTemp, value);
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
                        //indexTemp[0]=t;
                        //m_deltaRImage->SetPixel(indexTemp, 0.0);
                        //Amb iteradors
                        imIter.Set(0.0);
                        ++imIter;
                    }
                }
                ++boolIter;
            }
        }
    }
}

void PerfusionMapCalculatorThread::runPerfusionImage()
{
    DEBUG_LOG(QString("Fill %1 inicia perfusionImage").arg(m_id));
    int i,j,k,t;
    //prenem la i perquè sol ser el valor més gran
    //TODO:Compte quan un no és múltiple de l'altre!!!
    int iinit = (m_id * m_sizex)/m_numberOfThreads;
    int iend = ((m_id+1)* m_sizex)/m_numberOfThreads;
    int jend = m_sizey;
    int kend = m_sizez;
    int tend = m_sizet;
    Volume::ItkImageType::IndexType index;
    DoubleTemporalImageType::IndexType indexTemp;
    Volume::ItkImageType::IndexType indexVoxel;
    int slice;
    double meanbl, stdbl;
    double min;
    bool valid;
    //std::ofstream fout("rcbv.dat", ios::out);
    double value;
    double max;
    double valueCbv, valueCbf, valueMtt;

    QVector< double > timeseries(m_sizet);
    QVector< double > residueFunction(m_sizet);

    typedef itk::ImageRegionIteratorWithIndex<BoolImageType> BoolIterator;
    BoolIterator boolIter( m_checkImage, m_checkImage->GetBufferedRegion() );

    typedef itk::ImageRegionIteratorWithIndex<DoubleTemporalImageType> DoubleTempIterator;
    DoubleTempIterator imIter( m_deltaRImage, m_deltaRImage->GetBufferedRegion() );

    typedef itk::ImageRegionIteratorWithIndex<DoubleImageType> DoubleIterator;
    DoubleIterator cbvIter( m_cbvImage, m_cbvImage->GetBufferedRegion() );
    DoubleIterator cbfIter( m_cbfImage, m_cbfImage->GetBufferedRegion() );
    DoubleIterator mttIter( m_mttImage, m_mttImage->GetBufferedRegion() );
    DoubleIterator m0Iter( m_m0Image, m_m0Image->GetBufferedRegion() );

    typedef itk::ImageRegionIteratorWithIndex<ImageType> MapIterator;
    MapIterator cbvMapIter( m_cbvMapImage, m_cbvMapImage->GetBufferedRegion() );
    MapIterator cbfMapIter( m_cbfMapImage, m_cbfMapImage->GetBufferedRegion() );
    MapIterator mttMapIter( m_mttMapImage, m_mttMapImage->GetBufferedRegion() );

/*    std::cout<<"Fill "<<m_id<<" inicia recorregut"<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge deltaR: "<<m_deltaRImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge check: "<<m_checkImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge cbv: "<<m_cbvImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge cbf: "<<m_cbfImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge mtt: "<<m_mttImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge m0: "<<m_m0Image->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge cbv map: "<<m_cbvMapImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge cbf map: "<<m_cbfMapImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<" mida imatge mtt map: "<<m_mttMapImage->GetBufferedRegion().GetSize()<<std::endl;
    std::cout<<"Fill "<<m_id<<": iinit = "<<iinit<<", iend = "<<iend<<", m0aif = "<<m_m0aif<<std::endl;
    std::cout<<"Fill "<<m_id<<": jend = "<<jend<<", kend = "<<kend<<", tend = "<<tend<<std::endl;*/
    for (k=0;k<kend;k++)
    {
        //std::cout<<"********************* Fill "<<m_id<<" inicia llesca *********************************"<<k<<std::endl;
        index[2]=k;
        indexTemp[3]=k;
        for (j=0;j<jend;j++)
        {
            //std::cout<<"Fill "<<m_id<<" inicia columna "<<j<<std::endl;
            index[1]=j;
            index[0]=iinit;
            boolIter.SetIndex(index);
            cbvIter.SetIndex(index);
            cbfIter.SetIndex(index);
            mttIter.SetIndex(index);
            m0Iter.SetIndex(index);
            cbvMapIter.SetIndex(index);
            cbfMapIter.SetIndex(index);
            mttMapIter.SetIndex(index);
            indexTemp[2]=j;
            indexTemp[1]=iinit;
            indexTemp[0]=0;
            imIter.SetIndex(indexTemp);
            for (i=iinit;i<iend;i++)
            {
                indexTemp[1]=i;
                index[0]=i;
                //std::cout<<"Fill "<<m_id<<" inicia fila "<<i<<std::endl;
                if(m_checkImage->GetPixel(index))
//                if(boolIter.Get())
                {
                    for (t=0;t<tend;t++)
                    {
                        indexTemp[0]=t;
                        timeseries[t] = m_deltaRImage->GetPixel(indexTemp);
                        //timeseries[t] = imIter.Get();
                        ++imIter;
                    }
                    //std::cout<<","<<std::endl;
                    residueFunction = this->deconvolve(timeseries);
                    //std::cout<<"-"<<std::endl;
                    max=residueFunction[0];
                    for (t=1;t<tend;t++)
                    {
                        if(residueFunction[t] > max)
                        {
                            max = residueFunction[t];
                        }
                    }
                    //valueCbv = 100*0.7*m0Image->GetPixel(index)/m_m0aif; //in ml/100g --> Peter dixit!!
                    valueCbv = 100*0.7*m0Iter.Get()/m_m0aif; //in ml/100g --> Peter dixit!!
                    valueCbf = max*100*60*0.7/TR; //ml/100g*min --> Peter dixit!!
                    valueMtt = (60*valueCbv)/valueCbf; // TR (in sec.)
                    /*cbvIter.Set(10.0*valueCbv);  //JUST FOR A GOOD VISUALIZATION!!!!!!
                    cbfIter.Set(valueCbf);
                    mttIter.Set(10.0*valueMtt);  //JUST FOR A GOOD VISUALIZATION!!!!!!
                    cbvMapIter.Set(10*(int)valueCbv);  //JUST FOR A GOOD VISUALIZATION!!!!!!
                    cbfMapIter.Set((int)valueCbf);
                    mttMapIter.Set(10*(int)valueMtt);  //JUST FOR A GOOD VISUALIZATION!!!!!!
*/
                    m_cbvImage->SetPixel(index, 10.0*valueCbv);   //JUST FOR A GOOD VISUALIZATION!!!!!!
                    m_cbvMapImage->SetPixel(index, (int)(10*valueCbv));
                    m_cbfImage->SetPixel(index, valueCbf);
                    m_cbfMapImage->SetPixel(index, (int)(valueCbf));
                    m_mttImage->SetPixel(index, 10.0*valueMtt);   //JUST FOR A GOOD VISUALIZATION!!!!!!
                    m_mttMapImage->SetPixel(index, (int)(10*valueMtt));
                }
                else
                {
/*                    cbvIter.Set(0.0);
                    cbfIter.Set(0.0);
                    mttIter.Set(0.0);
                    cbvMapIter.Set(0);
                    cbfMapIter.Set(0);
                    mttMapIter.Set(0);
*/
                    m_cbvImage->SetPixel(index, 0.0);
                    m_cbfImage->SetPixel(index, 0.0);
                    m_mttImage->SetPixel(index, 0.0);
                    m_cbvMapImage->SetPixel(index, 0);
                    m_cbfMapImage->SetPixel(index, 0);
                    m_mttMapImage->SetPixel(index, 0);
                    
                    //Movem imIter fins al proper pixel
                    indexTemp[1]=i+1;
                    indexTemp[0]=0;
                    imIter.SetIndex(indexTemp);
                    //std::cout<<";"<<std::endl;
                }
                ++m0Iter;
                ++cbvIter;
                ++cbfIter;
                ++mttIter;
                ++cbvMapIter;
                ++cbfMapIter;
                ++mttMapIter;
                ++boolIter;
            }
        }
    }
    //std::cout<<"Fill "<<m_id<<" acaba perfusionImage"<<std::endl;

}

QVector<double> PerfusionMapCalculatorThread::deconvolve( QVector<double> tissue )
{
    QVector<double> residuefunc(tissue.size());
    int i;
    //std::cout<<"?"<<std::endl;
    for(i=0;i<tissue.size();i++)
    {
        std::cout<<tissue[i]<<" "<<std::flush;
    }
    //std::cout<<"?"<<std::endl;
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
    size[0] = m_sizet;  //les mostres temporals
    region.SetSize(size);
    region.SetIndex(start);
    //std::cout<<"&"<<std::endl;

    VectorImageType::Pointer tissueImage = VectorImageType::New();
    tissueImage->SetRegions( region );
    try
        {
        tissueImage->Allocate();
        }
    catch( itk::ExceptionObject & excp )
        {
        std::cerr << "Error: " << std::endl;
        std::cerr << excp << std::endl;
        return residuefunc;
        }
    //std::cout<<"$"<<std::endl;

    typedef itk::ImageRegionIterator<VectorImageType> VectorIteratorType;
    VectorIteratorType tissueIter( tissueImage, tissueImage->GetLargestPossibleRegion() );
    //std::cout<<"@"<<tissueImage->GetLargestPossibleRegion().GetSize()[0]<<std::endl;

    typedef itk::VnlFFTRealToComplexConjugateImageFilter< double, 1 >  FFTFilterType;
    FFTFilterType::Pointer fftFilter = FFTFilterType::New();
    //std::cout<<"#"<<std::endl;

    tissueIter.GoToBegin();

    //std::cout<<"%"<<std::endl;
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
        std::cerr << "Error: " << std::endl;
        std::cerr << excp << std::endl;
        return residuefunc;
        }

    typedef FFTFilterType::OutputImageType ComplexImageType;
    ComplexImageType::Pointer residualFFTImage = ComplexImageType::New();
    residualFFTImage->SetRegions( region );
    residualFFTImage->Allocate();

    typedef itk::ImageRegionIterator<ComplexImageType> ComplexIteratorType;
    ComplexIteratorType fftTissueIter( fftFilter->GetOutput(), fftFilter->GetOutput()->GetLargestPossibleRegion() );
    fftTissueIter.GoToBegin();

    ComplexIteratorType fftResidualIter( residualFFTImage, residualFFTImage->GetLargestPossibleRegion() );
    fftResidualIter.GoToBegin();

    //std::cout<<"!"<<fftFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]<<std::endl;
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
        std::cerr << "Error: " << std::endl;
        std::cerr << excp << std::endl;
        return residuefunc;
        }

    //std::cout<<"*"<<fftInverseFilter->GetOutput()->GetLargestPossibleRegion().GetSize()[0]<<std::endl;
    VectorIteratorType residualIter( fftInverseFilter->GetOutput(), fftInverseFilter->GetOutput()->GetLargestPossibleRegion() );
    residualIter.GoToBegin();

    for(i=0;i<residuefunc.size();i++)
    {
        //if(residuefunc[i]!=residualIter.Get()) std::cout<<"Resultat residuefunc diferent: "<<residuefunc[i]<<" ," <<residualIter.Get()/tissue.size()<<std::endl;

        residuefunc[i]=residualIter.Get();
        ++residualIter;
    }
    for(i=0;i<residuefunc.size();i++)
    {
        //std::cout<<residuefunc[i]<<" "<<std::flush;
    }
    //std::cout<<"?"<<std::endl;
    return residuefunc;
}

}
