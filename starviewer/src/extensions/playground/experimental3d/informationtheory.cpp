#ifndef UDGINFORMATIONTHEORY_CPP
#define UDGINFORMATIONTHEORY_CPP


#include "informationtheory.h"

#include <cmath>

#include <QThread>

//#include "histogram.h"


namespace udg {


// H(X) = -sum[0,n)( p(x) log p(x) )
template <class T>
double InformationTheory::entropy( const QVector<T> &probabilities )
{
    int size = probabilities.size();
    double entropy = 0.0;

    if ( size >= MIN_SIZE_TO_USE_THREADS )
    {
        int nThreads = QThread::idealThreadCount();
        EntropyThread<T> **threads = new EntropyThread<T>*[nThreads];
        int sizePerThread = size / nThreads + 1;
        int start = 0, end = sizePerThread;

        for ( int i = 0; i < nThreads; i++ )
        {
            threads[i] = new EntropyThread<T>( probabilities, start, end );
            threads[i]->start();
            start += sizePerThread;
            end += sizePerThread;
            if ( end > size ) end = size;
        }

        for ( int i = 0; i < nThreads; i++ )
        {
            threads[i]->wait();
            entropy += threads[i]->entropy();
            delete threads[i];
        }

        delete[] threads;
    }
    else
    {
        for ( int i = 0; i < size; i++ )
        {
            double p = probabilities.at( i );
            if ( p > 0.0 ) entropy -= p * log( p );
        }
    }

    entropy /= log( 2.0 );

    return entropy;
}


// H(X) = -sum[0,n)( p(x) log p(x) )
/*double InformationTheory::entropy( const Histogram &histogram )
{
    int size = histogram.size();
    double count = histogram.count();
    double entropy = 0.0;

    for ( int i = 0; i < size; i++ )
    {
        double p = histogram[i] / count;
        if ( p > 0.0 ) entropy -= p * log( p );
    }

    entropy /= log( 2.0 );

    return entropy;
}*/


// D_KL(P||Q) = sum[0,n)( P(i) log ( P(i) / Q(i) ) )
template <class T>
double InformationTheory::kullbackLeiblerDivergence( const QVector<T> &probabilitiesP, const QVector<T> &probabilitiesQ )
{
    int size = probabilitiesP.size();
    double kullbackLeiblerDivergence = 0.0;

    if ( size >= MIN_SIZE_TO_USE_THREADS )
    {
        int nThreads = QThread::idealThreadCount();
        KullbackLeiblerDivergenceThread<T> **threads = new KullbackLeiblerDivergenceThread<T>*[nThreads];
        int sizePerThread = size / nThreads + 1;
        int start = 0, end = sizePerThread;

        for ( int i = 0; i < nThreads; i++ )
        {
            threads[i] = new KullbackLeiblerDivergenceThread<T>( probabilitiesP, probabilitiesQ, start, end );
            threads[i]->start();
            start += sizePerThread;
            end += sizePerThread;
            if ( end > size ) end = size;
        }

        for ( int i = 0; i < nThreads; i++ )
        {
            threads[i]->wait();
            kullbackLeiblerDivergence += threads[i]->kullbackLeiblerDivergence();
            delete threads[i];
        }

        delete[] threads;
    }
    else
    {
        for ( int i = 0; i < size; i++ )
        {
            double p = probabilitiesP.at( i );

            if ( p > 0.0 )
            {
                double q = probabilitiesQ.at( i );
                kullbackLeiblerDivergence += p * log( p / q );
            }
        }
    }

    kullbackLeiblerDivergence /= log( 2.0 );

    return kullbackLeiblerDivergence;
}


// JSD(pi1,pi2; P1,P2) = H(pi1*P1 + pi2*P2) - ( pi1 * H(P1) + pi2 * H(P2) )
template <class T>
double InformationTheory::jensenShannonDivergence( double pi1, double pi2, const QVector<T> &probabilitiesP1, const QVector<T> &probabilitiesP2 )
{
    int size = probabilitiesP1.size();
    QVector<T> probabilitiesMix( size );

    if ( size >= MIN_SIZE_TO_USE_THREADS )
    {
        int nThreads = QThread::idealThreadCount();
        ProbabilitiesMixThread<T> **threads = new ProbabilitiesMixThread<T>*[nThreads];
        int sizePerThread = size / nThreads + 1;
        int start = 0, end = sizePerThread;

        for ( int i = 0; i < nThreads; i++ )
        {
            threads[i] = new ProbabilitiesMixThread<T>( pi1, pi2, probabilitiesP1, probabilitiesP2, probabilitiesMix, start, end );
            threads[i]->start();
            start += sizePerThread;
            end += sizePerThread;
            if ( end > size ) end = size;
        }

        for ( int i = 0; i < nThreads; i++ )
        {
            threads[i]->wait();
            delete threads[i];
        }

        delete[] threads;
    }
    else
    {
        for ( int i = 0; i < size; i++ ) probabilitiesMix[i] = pi1 * probabilitiesP1.at( i ) + pi2 * probabilitiesP2.at( i );
    }

    return entropy( probabilitiesMix ) - ( pi1 * entropy( probabilitiesP1 ) + pi2 * entropy( probabilitiesP2 ) );
}


template <class T>
class InformationTheory::EntropyThread : public QThread {
    public:
        EntropyThread( const QVector<T> &probabilities, int start, int end ) : m_probabilities( probabilities ), m_start( start ), m_end( end ) {}
        virtual void run()
        {
            m_entropy = 0.0;
            for ( int i = m_start; i < m_end; i++ )
            {
                double p = m_probabilities.at( i );
                if ( p > 0.0 ) m_entropy -= p * log( p );
            }
        }
        double entropy() const { return m_entropy; }
    private:
        const QVector<T> &m_probabilities;
        int m_start, m_end;
        double m_entropy;
};


template <class T>
class InformationTheory::KullbackLeiblerDivergenceThread : public QThread {
    public:
        KullbackLeiblerDivergenceThread( const QVector<T> &probabilitiesP, const QVector<T> &probabilitiesQ, int start, int end )
            : m_probabilitiesP( probabilitiesP ), m_probabilitiesQ( probabilitiesQ ), m_start( start ), m_end( end ) {}
        virtual void run()
        {
            m_kullbackLeiblerDivergence = 0.0;
            for ( int i = m_start; i < m_end; i++ )
            {
                double p = m_probabilitiesP.at( i );
                if ( p > 0.0 )
                {
                    double q = m_probabilitiesQ.at( i );
                    m_kullbackLeiblerDivergence += p * log( p / q );
                }
            }
        }
        double kullbackLeiblerDivergence() const { return m_kullbackLeiblerDivergence; }
    private:
        const QVector<T> &m_probabilitiesP, &m_probabilitiesQ;
        int m_start, m_end;
        double m_kullbackLeiblerDivergence;
};


template <class T>
class InformationTheory::ProbabilitiesMixThread : public QThread {
    public:
        ProbabilitiesMixThread( double pi1, double pi2, const QVector<T> &probabilitiesP1, const QVector<T> &probabilitiesP2, QVector<T> &probabilitiesMix, int start, int end )
            : m_pi1( pi1 ), m_pi2( pi2 ), m_probabilitiesP1( probabilitiesP1 ), m_probabilitiesP2( probabilitiesP2 ), m_probabilitiesMix( probabilitiesMix ), m_start( start ), m_end( end ) {}
        virtual void run() { for ( int i = m_start; i < m_end; i++ ) m_probabilitiesMix[i] = m_pi1 * m_probabilitiesP1.at( i ) + m_pi2 * m_probabilitiesP2.at( i ); }
    private:
        double m_pi1, m_pi2;
        const QVector<T> &m_probabilitiesP1, &m_probabilitiesP2;
        QVector<T> &m_probabilitiesMix;
        int m_start, m_end;
};


/*
InformationTheory::InformationTheory()
{
}


InformationTheory::~InformationTheory()
{
}
*/


}


#endif
