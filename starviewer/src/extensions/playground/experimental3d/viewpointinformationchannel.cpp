#include "viewpointinformationchannel.h"

#include <QCoreApplication>
#include <QSet>
#include <QThread>

#include "qexperimental3dviewer.h"
#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"

#include <limits>

#ifndef CUDA_AVAILABLE
#include <QTemporaryFile>
#include "mathtools.h"
#else // CUDA_AVAILABLE
#include "../gputesting/camera.h"
#include "cudaviewpointinformationchannel.h"
#include "../gputesting/matrix4.h"
#endif


namespace udg {


ViewpointInformationChannel::ViewpointInformationChannel( const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer, const TransferFunction &transferFunction )
        : QObject(), m_viewpointGenerator( viewpointGenerator ), m_volume( volume ), m_viewer( viewer ), m_transferFunction( transferFunction )
{
    m_backgroundColor = m_viewer->getBackgroundColor();
    m_viewpoints = m_viewpointGenerator.viewpoints();
}


void ViewpointInformationChannel::setColorVomiPalette( const QVector<Vector3Float> &colorVomiPalette )
{
    QMap< QPair<int, int>, QVector<int> > viewpointSubSets; // (nPunts,nColors) -> punts-millor-distribuïts
    {
        viewpointSubSets[qMakePair(42, 6)] = ( QVector<int>() << 13 << 17 << 23 << 27 << 33 << 39 );
        viewpointSubSets[qMakePair(162, 6)] = ( QVector<int>() << 16 << 34 << 62 << 82 << 110 << 143 );
    }

    int nViewpoints = m_viewpoints.size();
    m_viewpointColors.resize( nViewpoints );
    m_viewpointColors.fill( Vector3Float() );
    int nColors = colorVomiPalette.size();

    if ( nColors == 1 ) // tots el mateix
    {
        DEBUG_LOG( "tots els colors iguals" );
        for ( int i = 0; i < nViewpoints; i++ ) m_viewpointColors[i] = colorVomiPalette.at( 0 );
    }
    else if ( nViewpoints <= nColors )  // agafem els primers colors
    {
        DEBUG_LOG( "primers colors" );
        for ( int i = 0; i < nViewpoints; i++ ) m_viewpointColors[i] = colorVomiPalette.at( i );
    }
    else    // trobar els més separats -> que la distància mínima entre ells sigui màxima entre totes les mínimes
    {
        QVector<int> bestIndices;
        QPair<int, int> mapIndex( nViewpoints, nColors );

        if ( viewpointSubSets.contains( mapIndex ) )
        {
            DEBUG_LOG( "índexs precalculats" );
            bestIndices = viewpointSubSets.value( mapIndex );
        }
        else
        {
            DEBUG_LOG( "calculem els índexs" );
            double maxMinDistance = 0.0;

            class Combination
            {
                private:
                    int m_n;
                    int m_k;
                    QVector<int> m_data;
                public:
                    Combination( int n, int k )
                    {
                        Q_ASSERT( n >= 0 && k >= 0 );
                        m_n = n;
                        m_k = k;
                        m_data.resize( k );
                        for ( int i = 0; i < k; ++i ) m_data[i] = i;
                    }   // Combination(n,k)
                    QVector<int> data() const
                    {
                        return m_data;
                    }
                    QString toString() const
                    {
                        QString s = "{ ";
                        for ( int i = 0; i < m_k; ++i ) s += QString::number( m_data.at( i ) ) + " ";
                        s += "}";
                        return s;
                    }   // toString()
                    Combination* successor() const
                    {
                        if ( m_data[0] == m_n - m_k) return 0;
                        Combination *ans = new Combination( m_n, m_k );
                        int i;
                        for ( i = 0; i < m_k; ++i ) ans->m_data[i] = m_data.at( i );
                        for ( i = m_k - 1; i > 0 && ans->m_data.at( i ) == m_n - m_k + i; --i );
                        ++ans->m_data[i];
                        for ( int j = i; j < m_k - 1; ++j ) ans->m_data[j+1] = ans->m_data[j] + 1;
                        return ans;
                    }   // successor()
                    static quint64 choose( int n, int k )
                    {
                        if ( n < k ) return 0;  // special case
                        if ( n == k ) return 1;
                        int delta, iMax;
                        if ( k < n - k )    // ex: choose(100,3)
                        {
                            delta = n - k;
                            iMax = k;
                        }
                        else                // ex: choose(100,97)
                        {
                            delta = k;
                            iMax = n - k;
                        }
                        quint64 ans = delta + 1;
                        for ( int i = 2; i <= iMax; ++i ) ans = ( ans * ( delta + i ) ) / i;
                        return ans;
                    }   // choose()
            };  // Combination class

            quint64 nCombinations = Combination::choose( nViewpoints, nColors );
            Combination *c = new Combination( nViewpoints, nColors );
            for ( quint64 i = 0; i < nCombinations; i++ )
            {
                //DEBUG_LOG( QString( "%1: %2" ).arg( i ).arg( c->toString() ) );
                QVector<int> indices = c->data();
                double minDistance = ( m_viewpoints.at( indices.at( 0 ) ) - m_viewpoints.at( indices.at( 1 ) ) ).length();
                bool mayBeBetter = minDistance > maxMinDistance;

                for ( int j = 0; j < nColors - 1 && mayBeBetter; j++ )
                {
                    const Vector3 &v1 = m_viewpoints.at( indices.at( j ) );

                    for ( int k = j + 1; k < nColors && mayBeBetter; k++ )
                    {
                        double distance = ( v1 - m_viewpoints.at( indices.at( k ) ) ).length();

                        if ( distance < minDistance )
                        {
                            minDistance = distance;
                            mayBeBetter = minDistance > maxMinDistance;
                        }
                    }
                }

                if ( minDistance > maxMinDistance )
                {
                    maxMinDistance = minDistance;
                    bestIndices = indices;
                }

                Combination *t = c;
                c = c->successor();
                delete t;
            }
        }

#ifndef QT_NO_DEBUG
        DEBUG_LOG( "best indices:" );
        for ( int i = 0; i < nColors; i++ ) DEBUG_LOG( QString::number( bestIndices.at( i ) ) );
#endif

        for ( int i = 0; i < nColors; i++ ) m_viewpointColors[bestIndices.at(i)] = colorVomiPalette.at( i );

        float maxChange;

        do
        {
            maxChange = 0.0f;

            for ( int i = 0; i < nViewpoints; i++ )
            {
                if ( bestIndices.contains( i ) ) continue;

                Vector3 viewpoint = m_viewpoints.at( i );
                QVector<int> neighbours = m_viewpointGenerator.neighbours( i );
                Vector3Float color;
                double totalWeight = 0.0;
                int nNeighbours = neighbours.size();

                for ( int j = 0; j < nNeighbours; j++ )
                {
                    int neighbourIndex = neighbours.at( j );
                    double distance = ( viewpoint - m_viewpoints.at( neighbourIndex ) ).length();
                    double weight = 1.0 / distance;
                    color += weight * m_viewpointColors.at( neighbourIndex );
                    totalWeight += weight;
                }

                color /= totalWeight;
                float change = ( color - m_viewpointColors.at( i ) ).length();
                m_viewpointColors[i] = color;

                if ( change > maxChange ) maxChange = change;
            }
        } while ( maxChange > 0.1f );
    }

#ifndef QT_NO_DEBUG
    DEBUG_LOG( "colors: " );
    for ( int i = 0; i < nViewpoints; i++ )
    {
        DEBUG_LOG( QString( "v%1: %2 -> %3" ).arg( i+1 ).arg( m_viewpoints.at( i ).toString() ).arg( m_viewpointColors.at( i ).toString() ) );
    }
#endif
}


void ViewpointInformationChannel::setEvmiOpacityTransferFunction( const TransferFunction &evmiOpacityTransferFunction )
{
    m_evmiOpacityTransferFunction = evmiOpacityTransferFunction;
}


void ViewpointInformationChannel::setBestViewsParameters( bool fixedNumber, int n, float threshold )
{
    m_fixedNumberOfBestViews = fixedNumber;
    m_numberOfBestViews = n;
    m_bestViewsThreshold = threshold;
}


void ViewpointInformationChannel::setExploratoryTourThreshold( float threshold )
{
    m_exploratoryTourThreshold = threshold;
}


void ViewpointInformationChannel::filterViewpoints( const QVector<bool> &filter )
{
    if ( m_viewpoints.size() != filter.size() )
    {
        DEBUG_LOG( QString( "Error: mides dels vectors diferents al filtre de punts de vista: %1 punts de vista, %2 filtres" ).arg( m_viewpoints.size() ).arg( filter.size() ) );
        return;
    }

    m_viewpoints.clear();
    QVector<Vector3Float> viewpointColors;

    for ( int i = 0; i < filter.size(); i++ )
    {
        if ( filter.at( i ) )
        {
            m_viewpoints << m_viewpointGenerator.viewpoint( i );
            if ( !m_viewpointColors.isEmpty() ) viewpointColors << m_viewpointColors.at( i );
        }
    }

    m_viewpointColors = viewpointColors;
}


void ViewpointInformationChannel::compute(bool &HV, bool &HVz, bool &HZ, bool &HZv, bool &HZV, bool &vmi, bool &vmi2, bool &vmi3, bool &mi, bool &viewpointUnstabilities, bool &vomi, bool &vomi2, bool &vomi3,
                                          bool &viewpointVomi, bool &viewpointVomi2, bool &colorVomi, bool &evmiOpacity, bool &evmiVomi, bool &bestViews, bool &guidedTour, bool &exploratoryTour, bool display)
{
    // Si no hi ha res a calcular marxem
    if (!HV && !HVz && !HZ && !HZv && !HZV && !vmi && !vmi2 && !vmi3 && !mi && !viewpointUnstabilities && !vomi && !vomi2 && !vomi3 && !viewpointVomi && !viewpointVomi2 && !colorVomi && !evmiOpacity && !evmiVomi
        && !bestViews && !guidedTour && !exploratoryTour)
        return;

    bool viewProbabilities = false;
    bool voxelProbabilities = false;

    // Dependències
    if (vmi3) vomi2 = true;
    if (viewpointVomi2) vomi2 = true;
    if (vomi2) HV = true;
    if (vomi2) HVz = true;
    if (HV) viewProbabilities = true;
    if (HVz) viewProbabilities = true;
    if (HVz) voxelProbabilities = true;
    if (vomi3) vmi2 = true;
    if (vmi2) HZ = true;
    if (vmi2) HZv = true;
    if (HZ) voxelProbabilities = true;
    if (HZV) viewProbabilities = true;
    if (HZV) HZv = true;
    if (guidedTour) bestViews = true;
    if (bestViews) mi = true;
    if (exploratoryTour) mi = true;
    if (mi) vmi = true;
    if (vmi) voxelProbabilities = true;
    if (viewpointUnstabilities) viewProbabilities = true;
    if (viewpointVomi) vomi = true;
    if (evmiOpacity) voxelProbabilities = true;
    if (evmiVomi) vomi = true;
    if (vomi) voxelProbabilities = true;
    if (colorVomi) voxelProbabilities = true;
    if (voxelProbabilities) viewProbabilities = true;

#ifndef CUDA_AVAILABLE
    Q_UNUSED(display);
    computeCpu(viewProbabilities, voxelProbabilities, HV, HVz, HZ, HZv, HZV, vmi, vmi2, vmi3, mi, viewpointUnstabilities, vomi, vomi2, vomi3, viewpointVomi, viewpointVomi2, colorVomi, evmiOpacity, evmiVomi, bestViews,
               guidedTour, exploratoryTour);
#else // CUDA_AVAILABLE
    computeCuda(viewProbabilities, voxelProbabilities, HV, HVz, HZ, HZv, HZV, vmi, vmi2, vmi3, mi, viewpointUnstabilities, vomi, vomi2, vomi3, viewpointVomi, viewpointVomi2, colorVomi, evmiOpacity, evmiVomi,
                bestViews, guidedTour, exploratoryTour, display);
#endif // CUDA_AVAILABLE
}


bool ViewpointInformationChannel::hasViewedVolume() const
{
    return !m_viewedVolume.isEmpty();
}


const QVector<float>& ViewpointInformationChannel::viewedVolume() const
{
    return m_viewedVolume;
}


float ViewpointInformationChannel::HV() const
{
    return m_HV;
}


const QVector<float>& ViewpointInformationChannel::HVz() const
{
    return m_HVz;
}


float ViewpointInformationChannel::HZ() const
{
    return m_HZ;
}


const QVector<float>& ViewpointInformationChannel::HZv() const
{
    return m_HZv;
}


float ViewpointInformationChannel::HZV() const
{
    return m_HZV;
}


const QVector<float>& ViewpointInformationChannel::vmi() const
{
    return m_vmi;
}


const QVector<float>& ViewpointInformationChannel::vmi2() const
{
    return m_vmi2;
}


const QVector<float>& ViewpointInformationChannel::vmi3() const
{
    return m_vmi3;
}


float ViewpointInformationChannel::mi() const
{
    return m_mi;
}


const QVector<float>& ViewpointInformationChannel::viewpointUnstabilities() const
{
    return m_viewpointUnstabilities;
}


const QVector<float>& ViewpointInformationChannel::vomi() const
{
    return m_vomi;
}


float ViewpointInformationChannel::minimumVomi() const
{
    return m_minimumVomi;
}


float ViewpointInformationChannel::maximumVomi() const
{
    return m_maximumVomi;
}


const QVector<float>& ViewpointInformationChannel::vomi2() const
{
    return m_vomi2;
}


float ViewpointInformationChannel::minimumVomi2() const
{
    return m_minimumVomi2;
}


float ViewpointInformationChannel::maximumVomi2() const
{
    return m_maximumVomi2;
}


const QVector<float>& ViewpointInformationChannel::vomi3() const
{
    return m_vomi3;
}


float ViewpointInformationChannel::minimumVomi3() const
{
    return m_minimumVomi3;
}


float ViewpointInformationChannel::maximumVomi3() const
{
    return m_maximumVomi3;
}


const QVector<float>& ViewpointInformationChannel::viewpointVomi() const
{
    return m_viewpointVomi;
}


const QVector<float>& ViewpointInformationChannel::viewpointVomi2() const
{
    return m_viewpointVomi2;
}


const QVector<Vector3Float>& ViewpointInformationChannel::colorVomi() const
{
    return m_colorVomi;
}


float ViewpointInformationChannel::maximumColorVomi() const
{
    return m_maximumColorVomi;
}


const QVector<float>& ViewpointInformationChannel::evmiOpacity() const
{
    return m_evmiOpacity;
}


const QVector<float>& ViewpointInformationChannel::evmiVomi() const
{
    return m_evmiVomi;
}


const QList< QPair<int, Vector3> >& ViewpointInformationChannel::bestViews() const
{
    return m_bestViews;
}


const QList< QPair<int, Vector3> >& ViewpointInformationChannel::guidedTour() const
{
    return m_guidedTour;
}


const QList< QPair<int, Vector3> >& ViewpointInformationChannel::exploratoryTour() const
{
    return m_exploratoryTour;
}


QVector<float> ViewpointInformationChannel::voxelProbabilitiesInView( int i )
{
#ifndef CUDA_AVAILABLE
    return voxelProbabilitiesInViewCpu( i );
#else // CUDA_AVAILABLE
    return voxelProbabilitiesInViewCuda( i );
#endif // CUDA_AVAILABLE
}


#ifndef CUDA_AVAILABLE


void ViewpointInformationChannel::computeCpu(bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeHV, bool computeHVz, bool computeHZ, bool computeHZv, bool computeHZV, bool computeVmi,
                                             bool computeVmi2, bool computeVmi3, bool computeMi, bool computeViewpointUnstabilities, bool computeVomi, bool computeVomi2, bool computeVomi3, bool computeViewpointVomi,
                                             bool computeViewpointVomi2, bool computeColorVomi, bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour,bool computeExploratoryTour)
{
    DEBUG_LOG("computeCpu");

    // Inicialitzar progrés
    int nSteps = 1; // ray casting (p(Z|V))
    if (computeViewProbabilities || computeHV) nSteps++;    // p(V) + H(V)
    if (computeVoxelProbabilities || computeHZ) nSteps++;   // p(Z) + H(Z)
    if (computeHVz || computeVomi || computeColorVomi) nSteps++;    // H(V|z) + VoMI + color VoMI
    if (computeVomi2) nSteps++; // VoMI2
    // H(Z|v) + H(Z|V) + VMI + VMI2 + MI + viewpoint unstabilities + viewpoint VoMI (INF) + viewpoint VoMI2 (INF2) + EVMI with opacity + EVMI with VoMI
    if (computeHZv || computeHZV || computeVmi || computeVmi2 || computeMi || computeViewpointUnstabilities || computeViewpointVomi || computeViewpointVomi2 || computeEvmiOpacity || computeEvmiVomi) nSteps++;
    if (computeVmi3) nSteps++;  // VMI3
    if (computeVomi3) nSteps++; // VoMI3
    if (computeBestViews) nSteps++; // best views
    if (computeGuidedTour) nSteps++;    // guided tour
    if (computeExploratoryTour) nSteps++;   // exploratory tour

    emit totalProgressMaximum(nSteps);
    int step = 0;
    emit totalProgress(step);

    createVoxelProbabilitiesPerViewFiles();
    if (m_voxelProbabilitiesPerViewFiles.isEmpty()) return; // caldria llançar alguna excepció o retornar error

    float totalViewedVolume;

    // p(Z|V) (i acumulació de p(V))
    {
        totalViewedVolume = rayCastingCpu(computeViewProbabilities);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(V) + H(V)
    if (computeViewProbabilities || computeHV)
    {
        computeViewProbabilitiesAndEntropyCpu(totalViewedVolume, computeHV);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(Z) + H(Z)
    if (computeVoxelProbabilities || computeHZ)
    {
        computeVoxelProbabilitiesAndEntropyCpu(computeHZ);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // H(V|z) + VoMI + color VoMI
    if (computeHVz || computeVomi || computeColorVomi)
    {
        computeVomiCpu(computeHVz, computeVomi, computeColorVomi);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VoMI2
    if (computeVomi2)
    {
        computeVomi2Cpu();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // H(Z|v) + H(Z|V) + VMI + VMI2 + MI + viewpoint unstabilities + viewpoint VoMI (INF) + viewpoint VoMI2 (INF2) + EVMI with opacity + EVMI with VoMI
    if (computeHZv || computeHZV || computeVmi || computeVmi2 || computeMi || computeViewpointUnstabilities || computeViewpointVomi || computeViewpointVomi2 || computeEvmiOpacity || computeEvmiVomi)
    {
        computeViewMeasuresCpu(computeHZv, computeHZV, computeVmi, computeVmi2, computeMi, computeViewpointUnstabilities, computeViewpointVomi, computeViewpointVomi2, computeEvmiOpacity, computeEvmiVomi);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VMI3
    if (computeVmi3)
    {
        computeVmi3Cpu();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VoMI3
    if (computeVomi3)
    {
        computeVomi3Cpu();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // best views
    if (computeBestViews)
    {
        this->computeBestViews();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // guided tour
    if (computeGuidedTour)
    {
        this->computeGuidedTour();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // exploratory tour
    if (computeExploratoryTour)
    {
        this->computeExploratoryTour();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    deleteVoxelProbabilitiesPerViewFiles();
}


void ViewpointInformationChannel::createVoxelProbabilitiesPerViewFiles()
{
    DEBUG_LOG( "Creem p(Z|V)" );

    int nViewpoints = m_viewpoints.size();
    m_voxelProbabilitiesPerViewFiles.resize( nViewpoints );

    for ( int i = 0; i < nViewpoints; i++ )
    {
        m_voxelProbabilitiesPerViewFiles[i] = new QTemporaryFile( "pZvXXXXXX.tmp" );    // els fitxers temporals es creen al directori de treball

        if ( !m_voxelProbabilitiesPerViewFiles.at( i )->open() )
        {
            DEBUG_LOG( QString( "No s'ha pogut obrir el fitxer: error %1" ).arg( m_voxelProbabilitiesPerViewFiles.at( i )->errorString() ) );
            for ( int j = 0; j < i; j++ ) m_voxelProbabilitiesPerViewFiles.at( j )->close();
            m_voxelProbabilitiesPerViewFiles.clear();   // el deixarem buit si hi ha hagut problemes
            break;
        }
    }
}


QVector<float> ViewpointInformationChannel::voxelProbabilitiesInViewCpu( int i )
{
    QVector<float> pZv( m_volume->getSize() );
    m_voxelProbabilitiesPerViewFiles.at( i )->reset();  // reset per tornar al principi
    m_voxelProbabilitiesPerViewFiles.at( i )->read( reinterpret_cast<char*>( pZv.data() ), pZv.size() * sizeof(float) ); // llegim...
    m_voxelProbabilitiesPerViewFiles.at( i )->reset();  // ... i després fem un reset per tornar al principi i buidar el buffer (amb un peek queda el buffer ple, i es gasta molta memòria)
    return pZv;
}


void ViewpointInformationChannel::deleteVoxelProbabilitiesPerViewFiles()
{
    DEBUG_LOG( "Destruïm p(Z|V)" );

    for ( int i = 0; i < m_voxelProbabilitiesPerViewFiles.size(); i++ )
    {
        m_voxelProbabilitiesPerViewFiles.at( i )->close();
        delete m_voxelProbabilitiesPerViewFiles.at( i );
    }

    m_voxelProbabilitiesPerViewFiles.clear();
}


float ViewpointInformationChannel::rayCastingCpu( bool computeViewProbabilities )
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();
    double totalViewedVolume = 0.0;

    if ( computeViewProbabilities ) m_viewProbabilities.resize( nViewpoints );

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_volume->startVmiMode();

    for ( int i = 0; i < nViewpoints; i++ )
    {
        m_volume->startVmiSecondPass();
        m_viewer->setCamera( m_viewpoints.at( i ), Vector3(), m_viewpointGenerator.up( m_viewpoints.at( i ) ) );    // render

        // p(Z|V)
        QVector<float> voxelProbabilitiesInView = m_volume->finishVmiSecondPass();  // p(Z|v)
        m_voxelProbabilitiesPerViewFiles.at( i )->write( reinterpret_cast<const char*>( voxelProbabilitiesInView.data() ), nVoxels * sizeof(float) );

        // p(V)
        if ( computeViewProbabilities )
        {
            float viewedVolume = m_volume->viewedVolumeInVmiSecondPass();
            m_viewProbabilities[i] = viewedVolume;
            totalViewedVolume += viewedVolume;
        }

        emit partialProgress( 100 * ( i + 1 ) / nViewpoints );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    return totalViewedVolume;
}


void ViewpointInformationChannel::computeViewProbabilitiesAndEntropyCpu(float totalViewedVolume, bool computeHV)
{
    int nViewpoints = m_viewpoints.size();

    m_viewedVolume = m_viewProbabilities;

    if (totalViewedVolume > 0.0f)
    {
        emit partialProgress(0);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

        for (int i = 0; i < nViewpoints; i++)
        {
            DEBUG_LOG(QString("volume(v%1) = %2").arg(i + 1).arg(m_viewProbabilities.at(i)));
            m_viewProbabilities[i] /= totalViewedVolume;
            Q_ASSERT(!MathTools::isNaN(m_viewProbabilities.at(i)));
            DEBUG_LOG(QString("p(v%1) = %2").arg(i + 1).arg(m_viewProbabilities.at(i)));

            emit partialProgress(100 * (i + 1) / nViewpoints);
            QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
        }
    }

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        Q_ASSERT(!MathTools::isNaN(pv));
        Q_ASSERT(pv >= 0.0f && pv <= 1.0f);
        sum += pv;
    }
    DEBUG_LOG(QString("sum p(v) = %1").arg(sum));
#endif

    if (computeHV)
    {
        m_HV = InformationTheory::entropy(m_viewProbabilities);
        DEBUG_LOG(QString("H(V) = %1").arg(m_HV));
    }
}


void ViewpointInformationChannel::computeVoxelProbabilitiesAndEntropyCpu(bool computeHZ)
{
    class PZThread : public QThread {
        public:
            PZThread(QVector<float> &voxelProbabilities, int start, int end)
                : m_viewProbability(0.0f), m_voxelProbabilities(voxelProbabilities), m_start(start), m_end(end)
            {
            }
            void setViewData(float viewProbability, const QVector<float> &voxelProbabilitiesInView)
            {
                m_viewProbability = viewProbability;
                m_voxelProbabilitiesInView = voxelProbabilitiesInView;
            }
        protected:
            virtual void run()
            {
                for (int i = m_start; i < m_end; i++) m_voxelProbabilities[i] += m_viewProbability * m_voxelProbabilitiesInView.at(i);
            }
        private:
            float m_viewProbability;
            QVector<float> &m_voxelProbabilities;
            QVector<float> m_voxelProbabilitiesInView;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    m_voxelProbabilities.resize(nVoxels);
    m_voxelProbabilities.fill(0.0f);

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    PZThread **pzThreads = new PZThread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for (int k = 0; k < nThreads; k++)
    {
        pzThreads[k] = new PZThread(m_voxelProbabilities, start, end);
        start += nVoxelsPerThread;
        end += nVoxelsPerThread;
        if (end > nVoxels) end = nVoxels;
    }

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);    // p(Z|v)

        for (int k = 0; k < nThreads; k++)
        {
            pzThreads[k]->setViewData(m_viewProbabilities.at(i), voxelProbabilitiesInView);
            pzThreads[k]->start();
        }

        for (int k = 0; k < nThreads; k++) pzThreads[k]->wait();

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for (int k = 0; k < nThreads; k++) delete pzThreads[k];
    delete[] pzThreads;

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for (int j = 0; j < nVoxels; j++)
    {
        float pz = m_voxelProbabilities.at(j);
        Q_ASSERT(!MathTools::isNaN(pz));
        Q_ASSERT(pz >= 0.0f && pz <= 1.0f);
        sum += pz;
    }
    DEBUG_LOG(QString("sum p(z) = %1").arg(sum));
#endif

    if (computeHZ)
    {
        m_HZ = InformationTheory::entropy(m_voxelProbabilities);
        DEBUG_LOG(QString("H(Z) = %1").arg(m_HZ));
    }
}


void ViewpointInformationChannel::computeViewMeasuresCpu(bool computeHZv, bool computeHZV, bool computeVmi, bool computeVmi2, bool computeMi, bool computeViewpointUnstabilities, bool computeViewpointVomi,
                                                         bool computeViewpointVomi2, bool computeEvmiOpacity, bool computeEvmiVomi)
{
    class ViewpointVomiThread : public QThread {
        public:
            ViewpointVomiThread(const QVector<float> &voxelProbabilities, bool computeViewpointVomi, const QVector<float> &vomi, bool computeViewpointVomi2, const QVector<float> &vomi2, int start, int end)
                : m_viewProbability(0.0f), m_voxelProbabilities(voxelProbabilities), m_computeViewpointVomi(computeViewpointVomi), m_vomi(vomi), m_viewpointVomi(0.0f), m_computeViewpointVomi2(computeViewpointVomi2),
                  m_vomi2(vomi2), m_viewpointVomi2(0.0f), m_start(start), m_end(end)
            {
            }
            void setViewData(float viewProbability, const QVector<float> &voxelProbabilitiesInView)
            {
                m_viewProbability = viewProbability;
                m_voxelProbabilitiesInView = voxelProbabilitiesInView;
            }
            float viewpointVomi() const
            {
                return m_viewpointVomi;
            }
            float viewpointVomi2() const
            {
                return m_viewpointVomi2;
            }
        protected:
            virtual void run()
            {
                if (m_computeViewpointVomi)
                {
                    double viewpointVomi = 0.0;
                    for (int i = m_start; i < m_end; i++)
                    {
                        float pz = m_voxelProbabilities.at(i);
                        float pzv = m_voxelProbabilitiesInView.at(i);
                        float pvz = m_viewProbability * pzv / pz;
                        if (pvz > 0.0f) viewpointVomi += pvz * m_vomi.at(i);
                    }
                    m_viewpointVomi = viewpointVomi;
                }
                if (m_computeViewpointVomi2)
                {
                    double viewpointVomi2 = 0.0;
                    for (int i = m_start; i < m_end; i++)
                    {
                        float pz = m_voxelProbabilities.at(i);
                        float pzv = m_voxelProbabilitiesInView.at(i);
                        float pvz = m_viewProbability * pzv / pz;
                        if (pvz > 0.0f) viewpointVomi2 += pvz * m_vomi2.at(i);
                    }
                    m_viewpointVomi2 = viewpointVomi2;
                }
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            QVector<float> m_voxelProbabilitiesInView;
            bool m_computeViewpointVomi;
            const QVector<float> &m_vomi;
            float m_viewpointVomi;
            bool m_computeViewpointVomi2;
            const QVector<float> &m_vomi2;
            float m_viewpointVomi2;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    int nThreads = QThread::idealThreadCount();
    ViewpointVomiThread **viewpointVomiThreads;

    if (computeHZv) m_HZv.resize(nViewpoints);
    if (computeHZV) m_HZV = 0.0f;
    if (computeVmi) m_vmi.resize(nViewpoints);
    if (computeVmi2) m_vmi2.resize(nViewpoints);
    if (computeMi) m_mi = 0.0f;
    if (computeViewpointUnstabilities) m_viewpointUnstabilities.resize(nViewpoints);
    if (computeViewpointVomi || computeViewpointVomi2)
    {
        if (computeViewpointVomi)
        {
            m_viewpointVomi.resize(nViewpoints);
            m_viewpointVomi.fill(0.0f);
        }
        if (computeViewpointVomi2)
        {
            m_viewpointVomi2.resize(nViewpoints);
            m_viewpointVomi2.fill(0.0f);
        }
        viewpointVomiThreads = new ViewpointVomiThread*[nThreads];
        int nVoxelsPerThread = nVoxels / nThreads + 1;
        int start = 0, end = nVoxelsPerThread;
        for (int k = 0; k < nThreads; k++)
        {
            viewpointVomiThreads[k] = new ViewpointVomiThread(m_voxelProbabilities, computeViewpointVomi, m_vomi, computeViewpointVomi2, m_vomi2, start, end);
            start += nVoxelsPerThread;
            end += nVoxelsPerThread;
            if (end > nVoxels) end = nVoxels;
        }
    }
    QVector<float> ppZOpacity;  // p'(Z) (opacitat)
    if (computeEvmiOpacity)
    {
        m_evmiOpacity.resize(nViewpoints);
        ppZOpacity.resize(nVoxels);
        double total = 0.0;
        unsigned short *voxels = reinterpret_cast<unsigned short*>(m_volume->getImage()->GetScalarPointer());
        for (int j = 0; j < nVoxels; j++)
        {
            ppZOpacity[j] = m_voxelProbabilities.at(j) * m_evmiOpacityTransferFunction.getOpacity(voxels[j]);
            total += ppZOpacity.at(j);
        }
        for (int j = 0; j < nVoxels; j++) ppZOpacity[j] /= total;
    }
    QVector<float> ppZVomi; // p'(Z) (VoMI)
    if (computeEvmiVomi)
    {
        m_evmiVomi.resize(nViewpoints);
        ppZVomi.resize(nVoxels);
        double total = 0.0;
        for (int j = 0; j < nVoxels; j++)
        {
            ppZVomi[j] = m_voxelProbabilities.at(j) * m_vomi.at(j);
            total += ppZVomi.at(j);
        }
        for (int j = 0; j < nVoxels; j++) ppZVomi[j] /= total;
    }

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);

        if (computeHZv)
        {
            float HZv = InformationTheory::entropy(voxelProbabilitiesInView);
            Q_ASSERT(!MathTools::isNaN(HZv));
            m_HZv[i] = HZv;
            DEBUG_LOG(QString("H(Z|v%1) = %2").arg(i + 1).arg(HZv));
        }

        if (computeHZV)
        {
            m_HZV += m_viewProbabilities.at(i) * m_HZv.at(i);
        }

        if (computeVmi)
        {
            float vmi = InformationTheory::kullbackLeiblerDivergence(voxelProbabilitiesInView, m_voxelProbabilities);
            Q_ASSERT(!MathTools::isNaN(vmi));
            m_vmi[i] = vmi;
            DEBUG_LOG(QString("VMI(v%1) = %2").arg(i + 1).arg(vmi));
        }

        if (computeVmi2)
        {
            float vmi2 = m_HZ - m_HZv.at(i);
            Q_ASSERT(!MathTools::isNaN(vmi2));
            m_vmi2[i] = vmi2;
            DEBUG_LOG(QString("VMI2(v%1) = %2").arg(i + 1).arg(vmi2));
        }

        if (computeMi)
        {
            m_mi += m_viewProbabilities.at(i) * m_vmi.at(i);
        }

        if (computeViewpointUnstabilities)
        {
            float pvi = m_viewProbabilities.at(i);  // p(vi)

            QVector<int> neighbours = m_viewpointGenerator.neighbours(i);
            int nNeighbours = neighbours.size();
            double viewpointUnstability = 0.0;

            QVector<float> voxelProbabilitiesInNeighbour(nVoxels);  // p(Z|vj)

            for (int ij = 0; ij < nNeighbours; ij++)
            {
                int neighbour = neighbours.at(ij);
                float pvj = m_viewProbabilities.at(neighbour);  // p(vj)
                float pvij = pvi + pvj; // p(v̂)

                if (pvij == 0.0f) continue;

                QVector<float> voxelProbabilitiesInNeighbour = this->voxelProbabilitiesInView(neighbour);

                float viewpointDissimilarity = InformationTheory::jensenShannonDivergence(pvi / pvij, pvj / pvij, voxelProbabilitiesInView, voxelProbabilitiesInNeighbour);
                viewpointUnstability += viewpointDissimilarity;
            }

            viewpointUnstability /= nNeighbours;
            m_viewpointUnstabilities[i] = viewpointUnstability;
            DEBUG_LOG(QString("U(v%1) = %2").arg(i + 1).arg(viewpointUnstability));
        }

        if (computeViewpointVomi || computeViewpointVomi2)
        {
            float pv = m_viewProbabilities.at(i);

            if (pv > 0.0f)
            {
                for (int k = 0; k < nThreads; k++)
                {
                    viewpointVomiThreads[k]->setViewData(pv, voxelProbabilitiesInView);
                    viewpointVomiThreads[k]->start();
                }

                double viewpointVomi = 0.0;
                double viewpointVomi2 = 0.0;

                for (int k = 0; k < nThreads; k++)
                {
                    viewpointVomiThreads[k]->wait();
                    if (computeViewpointVomi) viewpointVomi += viewpointVomiThreads[k]->viewpointVomi();
                    if (computeViewpointVomi2) viewpointVomi2 += viewpointVomiThreads[k]->viewpointVomi2();
                }

                if (computeViewpointVomi)
                {
                    Q_ASSERT(!MathTools::isNaN(viewpointVomi));
                    m_viewpointVomi[i] = viewpointVomi;
                    DEBUG_LOG(QString("VVoMI(v%1) = %2").arg(i + 1).arg(viewpointVomi));
                }

                if (computeViewpointVomi2)
                {
                    Q_ASSERT(!MathTools::isNaN(viewpointVomi2));
                    m_viewpointVomi2[i] = viewpointVomi2;
                    DEBUG_LOG(QString("VVoMI2(v%1) = %2").arg(i + 1).arg(viewpointVomi2));
                }
            }
        }

        if (computeEvmiOpacity)
        {
            float evmiOpacity = InformationTheory::kullbackLeiblerDivergence(voxelProbabilitiesInView, ppZOpacity, true);
            Q_ASSERT(!MathTools::isNaN(evmiOpacity));
            m_evmiOpacity[i] = evmiOpacity;
            DEBUG_LOG(QString("EVMI_O(v%1) = %2").arg(i + 1).arg(evmiOpacity));
        }

        if (computeEvmiVomi)
        {
            float evmiVomi = InformationTheory::kullbackLeiblerDivergence(voxelProbabilitiesInView, ppZVomi, true);
            Q_ASSERT(!MathTools::isNaN(evmiVomi));
            m_evmiVomi[i] = evmiVomi;
            DEBUG_LOG(QString("EVMI_V(v%1) = %2").arg(i + 1).arg(evmiVomi));
        }

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if (computeHZV)
    {
        DEBUG_LOG(QString("H(Z) = %1").arg(m_HZV));
    }

    if (computeMi)
    {
        DEBUG_LOG(QString("I(V;Z) = %1").arg(m_mi));
    }

    if (computeViewpointVomi || computeViewpointVomi2)
    {
        for (int k = 0; k < nThreads; k++) delete viewpointVomiThreads[k];
        delete[] viewpointVomiThreads;
    }

#ifndef QT_NO_DEBUG
    if (computeVmi || computeVmi2)
    {
        double sumVmi1 = 0.0, sumVmi2 = 0.0;
        for (int i = 0; i < nViewpoints; i++)
        {
            float pv = m_viewProbabilities.at(i);
            if (computeVmi) sumVmi1 += pv * m_vmi.at(i);
            if (computeVmi2) sumVmi2 += pv * m_vmi2.at(i);
        }
        if (computeVmi)
        {
            DEBUG_LOG(QString("sum VMI1 = %1").arg(sumVmi1));
        }
        if (computeVmi2)
        {
            DEBUG_LOG(QString("sum VMI2 = %1").arg(sumVmi2));
        }
    }
#endif
}


void ViewpointInformationChannel::computeVmi3Cpu()
{
    class Vmi3Thread : public QThread {
        public:
            Vmi3Thread(const QVector<float> &voxelProbabilitiesInView, const QVector<float> &vomi2, int start, int end)
                : m_sum(0.0), m_voxelProbabilitiesInView(voxelProbabilitiesInView), m_vomi2(vomi2), m_start(start), m_end(end)
            {
            }
            float sum() const
            {
                return m_sum;
            }
        protected:
            virtual void run()
            {
                for (int i = m_start; i < m_end; i++)
                {
                    float pzv = m_voxelProbabilitiesInView.at(i);
                    float vomi2 = m_vomi2.at(i);
                    m_sum += pzv * vomi2;
                }
            }
        private:
            double m_sum;
            const QVector<float> &m_voxelProbabilitiesInView;
            const QVector<float> &m_vomi2;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    int nThreads = QThread::idealThreadCount();
    Vmi3Thread **vmi3Threads = new Vmi3Thread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;

    m_vmi3.resize(nViewpoints);

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);
        int start = 0, end = nVoxelsPerThread;

        for (int k = 0; k < nThreads; k++)
        {
            vmi3Threads[k] = new Vmi3Thread(voxelProbabilitiesInView, m_vomi2, start, end);
            vmi3Threads[k]->start();
            start += nVoxelsPerThread;
            end += nVoxelsPerThread;
            if (end > nVoxels) end = nVoxels;
        }

        float vmi3 = 0.0f;

        for (int k = 0; k < nThreads; k++)
        {
            vmi3Threads[k]->wait();
            vmi3 += vmi3Threads[k]->sum();
            delete vmi3Threads[k];
        }

        Q_ASSERT(!MathTools::isNaN(vmi3));
        m_vmi3[i] = vmi3;
        DEBUG_LOG(QString("VMI3(v%1) = %2").arg(i + 1).arg(vmi3));

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    delete[] vmi3Threads;

#ifndef QT_NO_DEBUG
    double sumVmi3 = 0.0;
    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        sumVmi3 += pv * m_vmi3.at(i);
    }
    DEBUG_LOG(QString("sum VMI3 = %1").arg(sumVmi3));
#endif
}


void ViewpointInformationChannel::computeVomiCpu(bool computeHVz, bool computeVomi, bool computeColorVomi)
{
    class VomiThread : public QThread {
        public:
            VomiThread(const QVector<float> &voxelProbabilities, bool computeHVz, QVector<float> &HVz, bool computeVomi, QVector<float> &vomi, bool computeColorVomi, QVector<Vector3Float> &colorVomi, int start,
                       int end)
                : m_viewProbability(0.0f), m_voxelProbabilities(voxelProbabilities), m_computeHVz(computeHVz), m_HVz(HVz), m_computeVomi(computeVomi), m_vomi(vomi), m_computeColorVomi(computeColorVomi),
                  m_viewColor(1.0f, 1.0f, 1.0f), m_colorVomi(colorVomi), m_start(start), m_end(end)
            {
            }
            void setViewData(float viewProbability, const QVector<float> &voxelProbabilitiesInView)
            {
                m_viewProbability = viewProbability;
                m_voxelProbabilitiesInView = voxelProbabilitiesInView;
            }
            void setViewColor(const Vector3Float &viewColor)
            {
                m_viewColor = viewColor;
            }
        protected:
            virtual void run()
            {
                Vector3Float color = Vector3Float(1.0f, 1.0f, 1.0f) - m_viewColor;
                for (int i = m_start; i < m_end; i++)
                {
                    float pz = m_voxelProbabilities.at(i);
                    float pzv = m_voxelProbabilitiesInView.at(i);
                    float pvz = m_viewProbability * pzv / pz;
                    if (pvz > 0.0f)
                    {
                        if (m_computeHVz) m_HVz[i] -= pvz * MathTools::logTwo(pvz);
                        if (m_computeVomi) m_vomi[i] += pvz * MathTools::logTwo(pvz / m_viewProbability);
                        if (m_computeColorVomi) m_colorVomi[i] += pvz * MathTools::logTwo(pvz / m_viewProbability) * color;
                    }
                }
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            QVector<float> m_voxelProbabilitiesInView;
            bool m_computeHVz;
            QVector<float> &m_HVz;
            bool m_computeVomi;
            QVector<float> &m_vomi;
            bool m_computeColorVomi;
            Vector3Float m_viewColor;
            QVector<Vector3Float> &m_colorVomi;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    if (computeHVz)
    {
        m_HVz.resize(nVoxels);
        m_HVz.fill(0.0f);
    }

    if (computeVomi)
    {
        m_vomi.resize(nVoxels);
        m_vomi.fill(0.0f);
    }

    if (computeColorVomi)
    {
        m_colorVomi.resize(nVoxels);
        m_colorVomi.fill(Vector3Float());
    }

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    VomiThread **vomiThreads = new VomiThread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for (int k = 0; k < nThreads; k++)
    {
        vomiThreads[k] = new VomiThread(m_voxelProbabilities, computeHVz, m_HVz, computeVomi, m_vomi, computeColorVomi, m_colorVomi, start, end);
        start += nVoxelsPerThread;
        end += nVoxelsPerThread;
        if (end > nVoxels) end = nVoxels;
    }

    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        if (pv == 0.0f) continue;

        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);

        for (int k = 0; k < nThreads; k++)
        {
            vomiThreads[k]->setViewData(m_viewProbabilities.at(i), voxelProbabilitiesInView);
            if (computeColorVomi) vomiThreads[k]->setViewColor(m_viewpointColors.at(i));
            vomiThreads[k]->start();
        }

        for (int k = 0; k < nThreads; k++) vomiThreads[k]->wait();

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for (int k = 0; k < nThreads; k++) delete vomiThreads[k];
    delete[] vomiThreads;

    if (computeVomi)
    {
        m_minimumVomi = std::numeric_limits<float>::infinity();
        m_maximumVomi = -std::numeric_limits<float>::infinity();
    }
    if (computeColorVomi) m_maximumColorVomi = 0.0f;

    for (int j = 0; j < nVoxels; j++)
    {
        if (computeHVz)
        {
            Q_ASSERT(!MathTools::isNaN(m_HVz.at(j)));
        }

        if (computeVomi)
        {
            float vomi = m_vomi.at(j);
            Q_ASSERT(!MathTools::isNaN(vomi));
            Q_ASSERT(vomi >= 0.0f);
            if (vomi < m_minimumVomi) m_minimumVomi = vomi;
            if (vomi > m_maximumVomi) m_maximumVomi = vomi;
        }

        if (computeColorVomi)
        {
            Vector3Float colorVomi = m_colorVomi.at(j);
            Q_ASSERT(!MathTools::isNaN(colorVomi.x) && !MathTools::isNaN(colorVomi.y) && !MathTools::isNaN(colorVomi.z));
            /// \todo pot ser < 0???
            //Q_ASSERT(colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f);
            //Q_ASSERT_X(colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f, "comprovació color vomi", qPrintable(colorVomi.toString()));
            if (colorVomi.x > m_maximumColorVomi) m_maximumColorVomi = colorVomi.x;
            if (colorVomi.y > m_maximumColorVomi) m_maximumColorVomi = colorVomi.y;
            if (colorVomi.z > m_maximumColorVomi) m_maximumColorVomi = colorVomi.z;
        }
    }

#ifndef QT_NO_DEBUG
    if (computeVomi)
    {
        double sumVomi1 = 0.0;
        for (int j = 0; j < nVoxels; j++)
        {
            float pz = m_voxelProbabilities.at(j);
            sumVomi1 += pz * m_vomi.at(j);
        }
        DEBUG_LOG(QString("sum VoMI1 = %1").arg(sumVomi1));
    }
#endif
}


void ViewpointInformationChannel::computeVomi2Cpu()
{
    class Vomi2Thread : public QThread {
        public:
            Vomi2Thread(float HV, const QVector<float> &HVz, QVector<float> &vomi2, int start, int end)
                : m_HV(HV), m_HVz(HVz), m_vomi2(vomi2), m_minimumVomi2(std::numeric_limits<float>::infinity()), m_maximumVomi2(-std::numeric_limits<float>::infinity()), m_start(start), m_end(end)
            {
            }
            float minimumVomi2() const
            {
                return m_minimumVomi2;
            }
            float maximumVomi2() const
            {
                return m_maximumVomi2;
            }
        protected:
            virtual void run()
            {
                for (int i = m_start; i < m_end; i++)
                {
                    m_vomi2[i] = m_HV - m_HVz.at(i);
                    Q_ASSERT(!MathTools::isNaN(m_vomi2.at(i)));
                    //Q_ASSERT(vomi2 >= 0.0f);  // la VoMI2 sí que pot ser negativa
                    if (m_vomi2.at(i) < m_minimumVomi2) m_minimumVomi2 = m_vomi2.at(i);
                    if (m_vomi2.at(i) > m_maximumVomi2) m_maximumVomi2 = m_vomi2.at(i);
                }
            }
        private:
            float m_HV;
            const QVector<float> &m_HVz;
            QVector<float> &m_vomi2;
            float m_minimumVomi2;
            float m_maximumVomi2;
            int m_start, m_end;
    };

    int nVoxels = m_volume->getSize();

    m_vomi2.resize(nVoxels);
    m_minimumVomi2 = std::numeric_limits<float>::infinity();
    m_maximumVomi2 = -std::numeric_limits<float>::infinity();

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    Vomi2Thread **vomi2Threads = new Vomi2Thread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for (int k = 0; k < nThreads; k++)
    {
        vomi2Threads[k] = new Vomi2Thread(m_HV, m_HVz, m_vomi2, start, end);
        vomi2Threads[k]->start();
        start += nVoxelsPerThread;
        end += nVoxelsPerThread;
        if (end > nVoxels) end = nVoxels;
    }

    for (int k = 0; k < nThreads; k++)
    {
        vomi2Threads[k]->wait();
        if (vomi2Threads[k]->minimumVomi2() < m_minimumVomi2) m_minimumVomi2 = vomi2Threads[k]->minimumVomi2();
        if (vomi2Threads[k]->maximumVomi2() > m_maximumVomi2) m_maximumVomi2 = vomi2Threads[k]->maximumVomi2();
        delete vomi2Threads[k];
        emit partialProgress(100 * (k + 1) / nThreads);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    delete[] vomi2Threads;

#ifndef QT_NO_DEBUG
    double sumVomi2 = 0.0;
    for (int j = 0; j < nVoxels; j++)
    {
        float pz = m_voxelProbabilities.at(j);
        sumVomi2 += pz * m_vomi2.at(j);
    }
    DEBUG_LOG(QString("sum VoMI2 = %1").arg(sumVomi2));
#endif
}


void ViewpointInformationChannel::computeVomi3Cpu()
{
    class Vomi3Thread : public QThread {
        public:
            Vomi3Thread(const QVector<float> &voxelProbabilities, QVector<float> &vomi3, int start, int end)
                : m_viewProbability(0.0f), m_voxelProbabilities(voxelProbabilities), m_vmi2(0.0f), m_vomi3(vomi3), m_start(start), m_end(end)
            {
            }
            void setViewData(float viewProbability, const QVector<float> &voxelProbabilitiesInView, float vmi2)
            {
                m_viewProbability = viewProbability;
                m_voxelProbabilitiesInView = voxelProbabilitiesInView;
                m_vmi2 = vmi2;
            }
        protected:
            virtual void run()
            {
                for (int i = m_start; i < m_end; i++)
                {
                    float pz = m_voxelProbabilities.at(i);
                    float pzv = m_voxelProbabilitiesInView.at(i);
                    float pvz = m_viewProbability * pzv / pz;
                    if (pvz > 0.0f) m_vomi3[i] += pvz * m_vmi2;
                }
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            QVector<float> m_voxelProbabilitiesInView;
            float m_vmi2;
            QVector<float> &m_vomi3;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    m_vomi3.resize(nVoxels);
    m_vomi3.fill(0.0f);

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    int nThreads = QThread::idealThreadCount();
    Vomi3Thread **vomi3Threads = new Vomi3Thread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;
    int start = 0, end = nVoxelsPerThread;

    for (int k = 0; k < nThreads; k++)
    {
        vomi3Threads[k] = new Vomi3Thread(m_voxelProbabilities, m_vomi3, start, end);
        start += nVoxelsPerThread;
        end += nVoxelsPerThread;
        if (end > nVoxels) end = nVoxels;
    }

    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        if (pv == 0.0f) continue;

        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);

        for (int k = 0; k < nThreads; k++)
        {
            vomi3Threads[k]->setViewData(m_viewProbabilities.at(i), voxelProbabilitiesInView, m_vmi2.at(i));
            vomi3Threads[k]->start();
        }

        for (int k = 0; k < nThreads; k++) vomi3Threads[k]->wait();

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    for (int k = 0; k < nThreads; k++) delete vomi3Threads[k];
    delete[] vomi3Threads;

    m_minimumVomi3 = std::numeric_limits<float>::infinity();
    m_maximumVomi3 = -std::numeric_limits<float>::infinity();

    for (int j = 0; j < nVoxels; j++)
    {
        float vomi3 = m_vomi3.at(j);
        Q_ASSERT(!MathTools::isNaN(vomi3));
        //Q_ASSERT(vomi3 >= 0.0f);  // la VoMI3 sí que pot ser negativa
        if (vomi3 < m_minimumVomi3) m_minimumVomi3 = vomi3;
        if (vomi3 > m_maximumVomi3) m_maximumVomi3 = vomi3;
    }

#ifndef QT_NO_DEBUG
    double sumVomi3 = 0.0;
    for (int j = 0; j < nVoxels; j++)
    {
        float pz = m_voxelProbabilities.at(j);
        sumVomi3 += pz * m_vomi3.at(j);
    }
    DEBUG_LOG(QString("sum VoMI3 = %1").arg(sumVomi3));
#endif
}


#else // CUDA_AVAILABLE


Matrix4 ViewpointInformationChannel::viewMatrix( const Vector3 &viewpoint )
{
    Camera camera;
    camera.lookAt( viewpoint, Vector3(), ViewpointGenerator::up( viewpoint ) );
    return camera.getViewMatrix();
}


void ViewpointInformationChannel::computeCuda(bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeHV, bool computeHVz, bool computeHZ, bool computeHZv, bool computeHZV, bool computeVmi,
                                              bool computeVmi2, bool computeVmi3, bool computeMi, bool computeViewpointUnstabilities, bool computeVomi, bool computeVomi2, bool computeVomi3, bool computeViewpointVomi,
                                              bool computeViewpointVomi2, bool computeColorVomi, bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour,
                                              bool computeExploratoryTour, bool display)
{
    DEBUG_LOG("computeCuda");

    // Inicialitzar progrés
    int nSteps = 0;
    if (computeViewProbabilities || computeHV) nSteps++;    // p(V) + H(V)
    if (computeVoxelProbabilities || computeHZ) nSteps++;   // p(Z) + H(Z)
    if (computeHVz || computeVomi || computeColorVomi) nSteps++;    // H(V|z) + VoMI + color VoMI
    if (computeVomi2) nSteps++; // VoMI2
    // H(Z|v) + H(Z|V) + VMI + VMI2 + MI + viewpoint unstabilities + viewpoint VoMI (INF) + viewpoint VoMI2 (INF2) + EVMI with opacity + EVMI with VoMI
    if (computeHZv || computeHZV || computeVmi || computeVmi2 || computeMi || computeViewpointUnstabilities || computeViewpointVomi || computeViewpointVomi2 || computeEvmiOpacity || computeEvmiVomi) nSteps++;
    if (computeVmi3) nSteps++;  // VMI3
    if (computeVomi3) nSteps++; // VoMI3
    if (computeBestViews) nSteps++; // best views
    if (computeGuidedTour) nSteps++;    // guided tour
    if (computeExploratoryTour) nSteps++;   // exploratory tour

    emit totalProgressMaximum(nSteps);
    int step = 0;
    emit totalProgress(step);

    // Inicialització de CUDA
    cvicSetupRayCast(m_volume->getImage(), m_transferFunction, 1024, 720, m_backgroundColor, display);
    if (computeVoxelProbabilities) cvicSetupVoxelProbabilities();

    // p(V) + H(V)
    if (computeViewProbabilities || computeHV)
    {
        computeViewProbabilitiesAndEntropyCuda(computeHV);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(Z) + H(Z)
    if (computeVoxelProbabilities || computeHZ)
    {
        computeVoxelProbabilitiesAndEntropyCuda(computeHZ);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // H(V|z) + VoMI + color VoMI
    if (computeHVz || computeVomi || computeColorVomi)
    {
        computeVomiCuda(computeHVz, computeVomi, computeColorVomi);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VoMI2
    if (computeVomi2)
    {
        computeVomi2Cuda();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // H(Z|v) + H(Z|V) + VMI + VMI2 + MI + viewpoint unstabilities + viewpoint VoMI (INF) + viewpoint VoMI2 (INF2) + EVMI with opacity + EVMI with VoMI
    if (computeHZv || computeHZV || computeVmi || computeMi || computeViewpointUnstabilities || computeViewpointVomi || computeViewpointVomi2 || computeEvmiOpacity || computeEvmiVomi)
    {
        computeViewMeasuresCuda(computeHZv, computeHZV, computeVmi, computeVmi2, computeMi, computeViewpointUnstabilities, computeViewpointVomi, computeViewpointVomi2, computeEvmiOpacity, computeEvmiVomi);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VMI3
    if (computeVmi3)
    {
        computeVmi3Cuda();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // VoMI3
    if (computeVomi3)
    {
        computeVomi3Cuda();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // best views
    if (computeBestViews)
    {
        this->computeBestViews();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // guided tour
    if (computeGuidedTour)
    {
        this->computeGuidedTour();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // exploratory tour
    if (computeExploratoryTour)
    {
        this->computeExploratoryTour();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // Finalització de CUDA
    if (computeVoxelProbabilities) cvicCleanupVoxelProbabilities();
    cvicCleanupRayCast();
}


QVector<float> ViewpointInformationChannel::voxelProbabilitiesInViewCuda( int i )
{
    QVector<float> pZv = cvicRayCastAndGetHistogram( m_viewpoints.at( i ), viewMatrix( m_viewpoints.at( i ) ) );    // p(Z|v) * viewedVolume
    double viewedVolume = 0.0;
    int nVoxels = m_volume->getSize();
    for ( int j = 0; j < nVoxels; j++ ) viewedVolume += pZv.at( j );
    Q_ASSERT( viewedVolume == viewedVolume );
    for ( int j = 0; j < nVoxels; j++ ) pZv[j] /= viewedVolume; // p(Z|v)
    return pZv;
}


void ViewpointInformationChannel::computeViewProbabilitiesAndEntropyCuda(bool computeHV)
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    m_viewProbabilities.resize(nViewpoints);
    double totalViewedVolume = 0.0;

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> histogram = cvicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i)));  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nVoxels; j++) viewedVolume += histogram.at(j);
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        m_viewProbabilities[i] = viewedVolume;
        totalViewedVolume += viewedVolume;

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_viewedVolume = m_viewProbabilities;

    for (int i = 0; i < nViewpoints; i++)
    {
        DEBUG_LOG(QString("volume(v%1) = %2").arg(i + 1).arg(m_viewProbabilities.at(i)));
        m_viewProbabilities[i] /= totalViewedVolume;
        DEBUG_LOG(QString("p(v%1) = %2").arg(i + 1).arg(m_viewProbabilities.at(i)));
    }

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        Q_ASSERT(!MathTools::isNaN(pv));
        Q_ASSERT(pv >= 0.0f && pv <= 1.0f);
        sum += pv;
    }
    DEBUG_LOG(QString("sum p(v) = %1").arg(sum));
#endif

    if (computeHV)
    {
        m_HV = InformationTheory::entropy(m_viewProbabilities);
        DEBUG_LOG(QString("H(V) = %1").arg(m_HV));
    }
}


void ViewpointInformationChannel::computeVoxelProbabilitiesAndEntropyCuda(bool computeHZ)
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> histogram = cvicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i)));  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nVoxels; j++) viewedVolume += histogram.at(j);
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        cvicAccumulateVoxelProbabilities(m_viewProbabilities.at(i), viewedVolume);

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_voxelProbabilities = cvicGetVoxelProbabilities();

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for (int j = 0; j < nVoxels; j++)
    {
        float pz = m_voxelProbabilities.at(j);
        Q_ASSERT(!MathTools::isNaN(pz));
        Q_ASSERT(pz >= 0.0f && pz <= 1.0f);
        sum += pz;
    }
    DEBUG_LOG(QString("sum p(z) = %1").arg(sum));
#endif

    if (computeHZ)
    {
        m_HZ = InformationTheory::entropy(m_voxelProbabilities);
        DEBUG_LOG(QString("H(Z) = %1").arg(m_HZ));
    }
}


void ViewpointInformationChannel::computeViewMeasuresCuda(bool computeHZv, bool computeHZV, bool computeVmi, bool computeVmi2, bool computeMi, bool computeViewpointUnstabilities, bool computeViewpointVomi,
                                                          bool computeViewpointVomi2, bool computeEvmiOpacity, bool computeEvmiVomi)
{
    class ViewpointVomiThread : public QThread {
        public:
            ViewpointVomiThread(const QVector<float> &voxelProbabilities, bool computeViewpointVomi, const QVector<float> &vomi, bool computeViewpointVomi2, const QVector<float> &vomi2, int start, int end)
                : m_viewProbability(0.0f), m_voxelProbabilities(voxelProbabilities), m_computeViewpointVomi(computeViewpointVomi), m_vomi(vomi), m_viewpointVomi(0.0f), m_computeViewpointVomi2(computeViewpointVomi2),
                  m_vomi2(vomi2), m_viewpointVomi2(0.0f), m_start(start), m_end(end)
            {
            }
            void setViewData(float viewProbability, const QVector<float> &voxelProbabilitiesInView)
            {
                m_viewProbability = viewProbability;
                m_voxelProbabilitiesInView = voxelProbabilitiesInView;
            }
            float viewpointVomi() const
            {
                return m_viewpointVomi;
            }
            float viewpointVomi2() const
            {
                return m_viewpointVomi2;
            }
        protected:
            virtual void run()
            {
                if (m_computeViewpointVomi)
                {
                    double viewpointVomi = 0.0;
                    for (int i = m_start; i < m_end; i++)
                    {
                        float pz = m_voxelProbabilities.at(i);
                        float pzv = m_voxelProbabilitiesInView.at(i);
                        float pvz = m_viewProbability * pzv / pz;
                        if (pvz > 0.0f) viewpointVomi += pvz * m_vomi.at(i);
                    }
                    m_viewpointVomi = viewpointVomi;
                }
                if (m_computeViewpointVomi2)
                {
                    double viewpointVomi2 = 0.0;
                    for (int i = m_start; i < m_end; i++)
                    {
                        float pz = m_voxelProbabilities.at(i);
                        float pzv = m_voxelProbabilitiesInView.at(i);
                        float pvz = m_viewProbability * pzv / pz;
                        if (pvz > 0.0f) viewpointVomi2 += pvz * m_vomi2.at(i);
                    }
                    m_viewpointVomi2 = viewpointVomi2;
                }
            }
        private:
            float m_viewProbability;
            const QVector<float> &m_voxelProbabilities;
            QVector<float> m_voxelProbabilitiesInView;
            bool m_computeViewpointVomi;
            const QVector<float> &m_vomi;
            float m_viewpointVomi;
            bool m_computeViewpointVomi2;
            const QVector<float> &m_vomi2;
            float m_viewpointVomi2;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    int nThreads = QThread::idealThreadCount();
    ViewpointVomiThread **viewpointVomiThreads;

    if (computeHZv) m_HZv.resize(nViewpoints);
    if (computeHZV) m_HZV = 0.0f;
    if (computeVmi) m_vmi.resize(nViewpoints);
    if (computeVmi2) m_vmi2.resize(nViewpoints);
    if (computeMi) m_mi = 0.0f;
    if (computeViewpointUnstabilities) m_viewpointUnstabilities.resize(nViewpoints);
    if (computeViewpointVomi || computeViewpointVomi2)
    {
        if (computeViewpointVomi)
        {
            m_viewpointVomi.resize(nViewpoints);
            m_viewpointVomi.fill(0.0f);
        }
        if (computeViewpointVomi2)
        {
            m_viewpointVomi2.resize(nViewpoints);
            m_viewpointVomi2.fill(0.0f);
        }
        viewpointVomiThreads = new ViewpointVomiThread*[nThreads];
        int nVoxelsPerThread = nVoxels / nThreads + 1;
        int start = 0, end = nVoxelsPerThread;
        for (int k = 0; k < nThreads; k++)
        {
            viewpointVomiThreads[k] = new ViewpointVomiThread(m_voxelProbabilities, computeViewpointVomi, m_vomi, computeViewpointVomi2, m_vomi2, start, end);
            start += nVoxelsPerThread;
            end += nVoxelsPerThread;
            if (end > nVoxels) end = nVoxels;
        }
    }
    QVector<float> ppZOpacity;  // p'(Z) (opacitat)
    if (computeEvmiOpacity)
    {
        m_evmiOpacity.resize(nViewpoints);
        ppZOpacity.resize(nVoxels);
        double total = 0.0;
        unsigned short *voxels = reinterpret_cast<unsigned short*>(m_volume->getImage()->GetScalarPointer());
        for (int j = 0; j < nVoxels; j++)
        {
            ppZOpacity[j] = m_voxelProbabilities.at(j) * m_evmiOpacityTransferFunction.getOpacity(voxels[j]);
            total += ppZOpacity.at(j);
        }
        for (int j = 0; j < nVoxels; j++) ppZOpacity[j] /= total;
    }
    QVector<float> ppZVomi; // p'(Z) (VoMI)
    if (computeEvmiVomi)
    {
        m_evmiVomi.resize(nViewpoints);
        ppZVomi.resize(nVoxels);
        double total = 0.0;
        for (int j = 0; j < nVoxels; j++)
        {
            ppZVomi[j] = m_voxelProbabilities.at(j) * m_vomi.at(j);
            total += ppZVomi.at(j);
        }
        for (int j = 0; j < nVoxels; j++) ppZVomi[j] /= total;
    }

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);    // p(Z|v)

        if (computeHZv)
        {
            float HZv = InformationTheory::entropy(voxelProbabilitiesInView);
            Q_ASSERT(!MathTools::isNaN(HZv));
            m_HZv[i] = HZv;
            DEBUG_LOG(QString("H(Z|v%1) = %2").arg(i + 1).arg(HZv));
        }

        if (computeHZV)
        {
            m_HZV += m_viewProbabilities.at(i) * m_HZv.at(i);
        }

        if (computeVmi)
        {
            float vmi = InformationTheory::kullbackLeiblerDivergence(voxelProbabilitiesInView, m_voxelProbabilities);
            Q_ASSERT(!MathTools::isNaN(vmi));
            m_vmi[i] = vmi;
            DEBUG_LOG(QString("VMI(v%1) = %2").arg(i + 1).arg(vmi));
        }

        if (computeVmi2)
        {
            float vmi2 = m_HZ - m_HZv.at(i);
            Q_ASSERT(!MathTools::isNaN(vmi2));
            m_vmi2[i] = vmi2;
            DEBUG_LOG(QString("VMI2(v%1) = %2").arg(i + 1).arg(vmi2));
        }

        if (computeMi)
        {
            m_mi += m_viewProbabilities.at(i) * m_vmi.at(i);
        }

        if (computeViewpointUnstabilities)
        {
            float pvi = m_viewProbabilities.at(i);  // p(vi)

            QVector<int> neighbours = m_viewpointGenerator.neighbours(i);
            int nNeighbours = neighbours.size();
            double viewpointUnstability = 0.0;

            for (int ij = 0; ij < nNeighbours; ij++)
            {
                int neighbour = neighbours.at(ij);
                float pvj = m_viewProbabilities.at(neighbour);  // p(vj)
                float pvij = pvi + pvj; // p(v')

                if (pvij == 0.0f) continue;

                QVector<float> voxelProbabilitiesInNeighbour = this->voxelProbabilitiesInView(neighbour);   // p(Z|vj)

                float viewpointDissimilarity = InformationTheory::jensenShannonDivergence(pvi / pvij, pvj / pvij, voxelProbabilitiesInView, voxelProbabilitiesInNeighbour);
                viewpointUnstability += viewpointDissimilarity;
            }

            viewpointUnstability /= nNeighbours;
            m_viewpointUnstabilities[i] = viewpointUnstability;
            DEBUG_LOG(QString("U(v%1) = %2").arg(i + 1).arg(viewpointUnstability));
        }

        if (computeViewpointVomi || computeViewpointVomi2)
        {
            float pv = m_viewProbabilities.at(i);

            if (pv > 0.0f)
            {
                for (int k = 0; k < nThreads; k++)
                {
                    viewpointVomiThreads[k]->setViewData(pv, voxelProbabilitiesInView);
                    viewpointVomiThreads[k]->start();
                }

                double viewpointVomi = 0.0;
                double viewpointVomi2 = 0.0;

                for (int k = 0; k < nThreads; k++)
                {
                    viewpointVomiThreads[k]->wait();
                    if (computeViewpointVomi) viewpointVomi += viewpointVomiThreads[k]->viewpointVomi();
                    if (computeViewpointVomi2) viewpointVomi2 += viewpointVomiThreads[k]->viewpointVomi2();
                }

                if (computeViewpointVomi)
                {
                    Q_ASSERT(!MathTools::isNaN(viewpointVomi));
                    m_viewpointVomi[i] = viewpointVomi;
                    DEBUG_LOG(QString("VVoMI(v%1) = %2").arg(i + 1).arg(viewpointVomi));
                }

                if (computeViewpointVomi2)
                {
                    Q_ASSERT(!MathTools::isNaN(viewpointVomi2));
                    m_viewpointVomi2[i] = viewpointVomi2;
                    DEBUG_LOG(QString("VVoMI2(v%1) = %2").arg(i + 1).arg(viewpointVomi2));
                }
            }
        }

        if (computeEvmiOpacity)
        {
            float evmiOpacity = InformationTheory::kullbackLeiblerDivergence(voxelProbabilitiesInView, ppZOpacity, true);
            Q_ASSERT(!MathTools::isNaN(evmiOpacity));
            m_evmiOpacity[i] = evmiOpacity;
            DEBUG_LOG(QString("EVMI_O(v%1) = %2").arg(i + 1).arg(evmiOpacity));
        }

        if (computeEvmiVomi)
        {
            float evmiVomi = InformationTheory::kullbackLeiblerDivergence(voxelProbabilitiesInView, ppZVomi, true);
            Q_ASSERT(!MathTools::isNaN(evmiVomi));
            m_evmiVomi[i] = evmiVomi;
            DEBUG_LOG(QString("EVMI_V(v%1) = %2").arg(i + 1).arg(evmiVomi)) ;
        }

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if (computeHZV)
    {
        DEBUG_LOG(QString("H(Z|V) = %1").arg(m_HZV));
    }

    if (computeMi)
    {
        DEBUG_LOG(QString("I(V;Z) = %1").arg(m_mi));
    }

    if (computeViewpointVomi || computeViewpointVomi2)
    {
        for (int k = 0; k < nThreads; k++) delete viewpointVomiThreads[k];
        delete[] viewpointVomiThreads;
    }

#ifndef QT_NO_DEBUG
    if (computeVmi || computeVmi2)
    {
        double sumVmi1 = 0.0, sumVmi2 = 0.0;
        for (int i = 0; i < nViewpoints; i++)
        {
            float pv = m_viewProbabilities.at(i);
            if (computeVmi) sumVmi1 += pv * m_vmi.at(i);
            if (computeVmi2) sumVmi2 += pv * m_vmi2.at(i);
        }
        if (computeVmi)
        {
            DEBUG_LOG(QString("sum VMI1 = %1").arg(sumVmi1));
        }
        if (computeVmi2)
        {
            DEBUG_LOG(QString("sum VMI2 = %1").arg(sumVmi2));
        }
    }
#endif
}


void ViewpointInformationChannel::computeVmi3Cuda()
{
    class Vmi3Thread : public QThread {
        public:
            Vmi3Thread(const QVector<float> &voxelProbabilitiesInView, const QVector<float> &vomi2, int start, int end)
                : m_sum(0.0), m_voxelProbabilitiesInView(voxelProbabilitiesInView), m_vomi2(vomi2), m_start(start), m_end(end)
            {
            }
            float sum() const
            {
                return m_sum;
            }
        protected:
            virtual void run()
            {
                for (int i = m_start; i < m_end; i++)
                {
                    float pzv = m_voxelProbabilitiesInView.at(i);
                    float vomi2 = m_vomi2.at(i);
                    m_sum += pzv * vomi2;
                }
            }
        private:
            double m_sum;
            const QVector<float> &m_voxelProbabilitiesInView;
            const QVector<float> &m_vomi2;
            int m_start, m_end;
    };

    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    int nThreads = QThread::idealThreadCount();
    Vmi3Thread **vmi3Threads = new Vmi3Thread*[nThreads];
    int nVoxelsPerThread = nVoxels / nThreads + 1;

    m_vmi3.resize(nViewpoints);

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> voxelProbabilitiesInView = this->voxelProbabilitiesInView(i);
        int start = 0, end = nVoxelsPerThread;

        for (int k = 0; k < nThreads; k++)
        {
            vmi3Threads[k] = new Vmi3Thread(voxelProbabilitiesInView, m_vomi2, start, end);
            vmi3Threads[k]->start();
            start += nVoxelsPerThread;
            end += nVoxelsPerThread;
            if (end > nVoxels) end = nVoxels;
        }

        float vmi3 = 0.0f;

        for (int k = 0; k < nThreads; k++)
        {
            vmi3Threads[k]->wait();
            vmi3 += vmi3Threads[k]->sum();
            delete vmi3Threads[k];
        }

        Q_ASSERT(!MathTools::isNaN(vmi3));
        m_vmi3[i] = vmi3;
        DEBUG_LOG(QString("VMI3(v%1) = %2").arg(i + 1).arg(vmi3));

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    delete[] vmi3Threads;

#ifndef QT_NO_DEBUG
    double sumVmi3 = 0.0;
    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        sumVmi3 += pv * m_vmi3.at(i);
    }
    DEBUG_LOG(QString("sum VMI3 = %1").arg(sumVmi3));
#endif
}


void ViewpointInformationChannel::computeVomiCuda(bool computeHVz, bool computeVomi, bool computeColorVomi)
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    cvicSetupVomi(computeHVz, computeVomi, computeColorVomi);

    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        if (pv == 0.0f) continue;

        QVector<float> histogram = cvicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i)));  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nVoxels; j++) viewedVolume += histogram.at(j);
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        if (computeHVz) cvicAccumulateHVz(pv, viewedVolume);
        if (computeVomi) cvicAccumulateVomi(pv, viewedVolume);
        if (computeColorVomi) cvicAccumulateColorVomi(pv, m_viewpointColors.at(i), viewedVolume);

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if (computeHVz)
    {
        m_HVz = cvicGetHVz();
    }

    if (computeVomi)
    {
        m_vomi = cvicGetVomi();
        m_minimumVomi = std::numeric_limits<float>::infinity();
        m_maximumVomi = -std::numeric_limits<float>::infinity();
    }

    if (computeColorVomi)
    {
        m_colorVomi = cvicGetColorVomi();
        m_maximumColorVomi = 0.0f;
    }

    cvicCleanupVomi();

    for (int j = 0; j < nVoxels; j++)
    {
        if (computeHVz)
        {
            Q_ASSERT(!MathTools::isNaN(m_HVz.at(j)));
        }

        if (computeVomi)
        {
            float vomi = m_vomi.at(j);
            Q_ASSERT(!MathTools::isNaN(vomi));
            Q_ASSERT(vomi >= 0.0f);
            if (vomi < m_minimumVomi) m_minimumVomi = vomi;
            if (vomi > m_maximumVomi) m_maximumVomi = vomi;
        }

        if (computeColorVomi)
        {
            Vector3Float colorVomi = m_colorVomi.at(j);
            Q_ASSERT(!MathTools::isNaN(colorVomi.x) && !MathTools::isNaN(colorVomi.y) && !MathTools::isNaN(colorVomi.z));
            /// \todo pot ser < 0???
            //Q_ASSERT(colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f);
            //Q_ASSERT_X(colorVomi.x >= 0.0f && colorVomi.y >= 0.0f && colorVomi.z >= 0.0f, "comprovació color vomi", qPrintable(colorVomi.toString()));
            if (colorVomi.x > m_maximumColorVomi) m_maximumColorVomi = colorVomi.x;
            if (colorVomi.y > m_maximumColorVomi) m_maximumColorVomi = colorVomi.y;
            if (colorVomi.z > m_maximumColorVomi) m_maximumColorVomi = colorVomi.z;
        }
    }

#ifndef QT_NO_DEBUG
    if (computeVomi)
    {
        double sumVomi1 = 0.0;
        for (int j = 0; j < nVoxels; j++)
        {
            float pz = m_voxelProbabilities.at(j);
            sumVomi1 += pz * m_vomi.at(j);
        }
        DEBUG_LOG(QString("sum VoMI1 = %1").arg(sumVomi1));
    }
#endif
}


void ViewpointInformationChannel::computeVomi2Cuda()
{
    int nVoxels = m_volume->getSize();

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_vomi2 = cvicComputeVomi2(m_HV, m_HVz);

    emit partialProgress(100);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_minimumVomi2 = std::numeric_limits<float>::infinity();
    m_maximumVomi2 = -std::numeric_limits<float>::infinity();

    for (int j = 0; j < nVoxels; j++)
    {
        float vomi2 = m_vomi2.at(j);
        Q_ASSERT(!MathTools::isNaN(vomi2));
        //Q_ASSERT(vomi2 >= 0.0f);  // la VoMI2 sí que pot ser negativa
        if (vomi2 < m_minimumVomi2) m_minimumVomi2 = vomi2;
        if (vomi2 > m_maximumVomi2) m_maximumVomi2 = vomi2;
    }

#ifndef QT_NO_DEBUG
    double sumVomi2 = 0.0;
    for (int j = 0; j < nVoxels; j++)
    {
        float pz = m_voxelProbabilities.at(j);
        sumVomi2 += pz * m_vomi2.at(j);
    }
    DEBUG_LOG(QString("sum VoMI2 = %1").arg(sumVomi2));
#endif
}


void ViewpointInformationChannel::computeVomi3Cuda()
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    cvicSetupVomi3();

    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        if (pv == 0.0f) continue;

        QVector<float> histogram = cvicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i)));  // p(Z|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nVoxels; j++) viewedVolume += histogram.at(j);
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        cvicAccumulateVomi3(pv, viewedVolume, m_vmi2.at(i));

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_vomi3 = cvicGetVomi3();
    m_minimumVomi3 = std::numeric_limits<float>::infinity();
    m_maximumVomi3 = -std::numeric_limits<float>::infinity();

    cvicCleanupVomi3();

    for (int j = 0; j < nVoxels; j++)
    {
        float vomi3 = m_vomi3.at(j);
        Q_ASSERT(!MathTools::isNaN(vomi3));
        //Q_ASSERT(vomi3 >= 0.0f);  // la VoMI3 sí que pot ser negativa
        if (vomi3 < m_minimumVomi3) m_minimumVomi3 = vomi3;
        if (vomi3 > m_maximumVomi3) m_maximumVomi3 = vomi3;
    }

#ifndef QT_NO_DEBUG
    double sumVomi3 = 0.0;
    for (int j = 0; j < nVoxels; j++)
    {
        float pz = m_voxelProbabilities.at(j);
        sumVomi3 += pz * m_vomi3.at(j);
    }
    DEBUG_LOG(QString("sum VoMI3 = %1").arg(sumVomi3));
#endif
}


#endif // CUDA_AVAILABLE


void ViewpointInformationChannel::computeBestViews()
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_bestViews.clear();

    // millor vista
    float minVmi = m_vmi.at( 0 );
    int minVmiIndex = 0;

    for ( int i = 1; i < nViewpoints; i++ )
    {
        float vmi = m_vmi.at( i );

        if ( vmi < minVmi )
        {
            minVmi = vmi;
            minVmiIndex = i;
        }
    }

    m_bestViews << qMakePair( minVmiIndex, m_viewpoints.at( minVmiIndex ) );

    QList<int> viewpointIndexList;
    for ( int i = 0; i < nViewpoints; i++ ) viewpointIndexList << i;
    viewpointIndexList.removeAt( minVmiIndex );

    float pvv = m_viewProbabilities.at( minVmiIndex );    // p(v̂)

    QVector<float> pZvv = this->voxelProbabilitiesInView( minVmiIndex );    // p(Z|v̂)

    // límits
    m_numberOfBestViews = qMin( m_numberOfBestViews, nViewpoints );
    float IvvZ = minVmi;    // I(v̂;Z)

    if ( m_fixedNumberOfBestViews )
    {
        DEBUG_LOG( QString( "límit %1 vistes" ).arg( m_numberOfBestViews ) );
    }
    else
    {
        DEBUG_LOG( QString( "límit llindar %1" ).arg( m_bestViewsThreshold ) );
    }

    DEBUG_LOG( QString( "I(V;Z) = %1" ).arg( m_mi ) );
    DEBUG_LOG( "Millors vistes:" );
    DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂;Z) = %4; I(v̂;Z)/I(V;Z) = %5" ).arg( 0 ).arg( minVmiIndex + 1 ).arg( m_viewpoints.at( minVmiIndex ).toString() ).arg( IvvZ ).arg( IvvZ / m_mi ) );

    if ( m_fixedNumberOfBestViews )
    {
        emit partialProgress( 100 / m_numberOfBestViews );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    while ( ( m_fixedNumberOfBestViews && m_bestViews.size() < m_numberOfBestViews ) || ( !m_fixedNumberOfBestViews && IvvZ / m_mi > m_bestViewsThreshold ) )
    {
        int nRemainingViews = viewpointIndexList.size();

        if ( nRemainingViews == 0 ) break;

        float pvvMin = 0.0f;
        QVector<float> pZvvMin;
        float IvvZMin = 0.0f;
        int iMin = 0;

        for ( int i = 0; i < nRemainingViews; i++ )
        {
            int viewIndex = viewpointIndexList.at( i );
            float pvi = m_viewProbabilities.at( viewIndex );
            QVector<float> pZvi = this->voxelProbabilitiesInView( viewIndex );  // p(Z|vi)

            float pvvi = pvv + pvi;         // p(v̂) afegint aquesta vista
            QVector<float> pZvvi( pZvv );   // vector p(Z|v̂) afegint aquesta vista
            for ( int j = 0; j < nVoxels; j++ ) pZvvi[j] = ( pvv * pZvv.at( j ) + pvi * pZvi.at( j ) ) / pvvi;
            float IvviZ = InformationTheory::kullbackLeiblerDivergence( pZvvi, m_voxelProbabilities );  // I(v̂,Z) afegint aquesta vista

            if ( i == 0 || IvviZ < IvvZMin )
            {
                pvvMin = pvvi;
                pZvvMin = pZvvi;
                IvvZMin = IvviZ;
                iMin = i;
            }
        }

        pvv = pvvMin;
        pZvv = pZvvMin;
        IvvZ = IvvZMin;
        int viewIndex = viewpointIndexList.takeAt( iMin );
        m_bestViews << qMakePair( viewIndex, m_viewpoints.at( viewIndex ) );
        DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂;Z) = %4; I(v̂;Z)/I(V;Z) = %5" ).arg( m_bestViews.size() - 1 ).arg( viewIndex + 1 ).arg( m_viewpoints.at( viewIndex ).toString() ).arg( IvvZ ).arg( IvvZ / m_mi ) );

        if ( m_fixedNumberOfBestViews )
        {
            emit partialProgress( 100 * m_bestViews.size() / m_fixedNumberOfBestViews );
            QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
        }
    }

    emit partialProgress( 100 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
}


void ViewpointInformationChannel::computeGuidedTour()
{
    int nViewpoints = m_viewpoints.size();

    DEBUG_LOG( "Guided tour:" );

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_guidedTour.clear();

    QList< QPair<int, Vector3> > bestViews = m_bestViews;   // còpia
    int nBestViews = bestViews.size();

    m_guidedTour << bestViews.takeAt( 0 );
    DEBUG_LOG( QString( "%1: (v%2) = %3" ).arg( 0 ).arg( m_guidedTour.last().first + 1 ).arg( m_guidedTour.last().second.toString() ) );

    emit partialProgress( 100 / nBestViews );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    QSet<int> viewpointIndices;
    for ( int i = 0; i < nViewpoints; i++ ) viewpointIndices << i;

    while ( !bestViews.isEmpty() )
    {
        QPair<int, Vector3> current = m_guidedTour.last();
        int i = current.first;
        float pvi = m_viewProbabilities.at( i );    // p(vi)

        QVector<float> pZvi = voxelProbabilitiesInView( i );    // p(Z|vi);

        int target, targetIndex;
        float minDissimilarity;
        int remainingViews = bestViews.size();

        // trobar el target
        for ( int k = 0; k < remainingViews; k++ )
        {
            int j = bestViews.at( k ).first;
            float pvj = m_viewProbabilities.at( j );    // p(vj)
            float pvij = pvi + pvj; // p(v̂)

            float dissimilarity;

            if ( pvij == 0.0f ) dissimilarity = 0.0f;
            else
            {
                QVector<float> pZvj = voxelProbabilitiesInView( j );    // p(Z|vj)

                dissimilarity = InformationTheory::jensenShannonDivergence( pvi / pvij, pvj / pvij, pZvi, pZvj );
            }

            if ( k == 0 || dissimilarity < minDissimilarity )
            {
                target = j;
                targetIndex = k;
                minDissimilarity = dissimilarity;
            }
        }

        // p(vi) i p(Z|vi) ara fan referència al target
        pvi = m_viewProbabilities.at( target ); // p(vi)

        pZvi = voxelProbabilitiesInView( target );

        QSet<int> indices( viewpointIndices );
        int currentIndex = i;

        // camí fins al target
        while ( currentIndex != target )
        {
            indices.remove( currentIndex );

            QVector<int> neighbours = m_viewpointGenerator.neighbours( currentIndex );
            int nNeighbours = neighbours.size();
            bool test = false;  // per comprovar que sempre tria un veí

            for ( int k = 0; k < nNeighbours; k++ )
            {
                int j = neighbours.at( k );

                if ( !indices.contains( j ) ) continue;

                float pvj = m_viewProbabilities.at( j );    // p(vj)
                float pvij = pvi + pvj; // p(v̂)

                float dissimilarity;

                if ( pvij == 0.0f ) dissimilarity = 0.0f;
                else
                {
                    QVector<float> pZvj = voxelProbabilitiesInView( j );

                    dissimilarity = InformationTheory::jensenShannonDivergence( pvi / pvij, pvj / pvij, pZvi, pZvj );
                }

                if ( k == 0 || dissimilarity < minDissimilarity )
                {
                    currentIndex = j;
                    minDissimilarity = dissimilarity;
                    test = true;
                }
            }

            Q_ASSERT( test );

            m_guidedTour << qMakePair( currentIndex, m_viewpoints.at( currentIndex ) );
            DEBUG_LOG( QString( "%1: (v%2) = %3; dissimilarity = %4" ).arg( m_guidedTour.size() - 1 ).arg( m_guidedTour.last().first + 1 ).arg( m_guidedTour.last().second.toString() ).arg( minDissimilarity ) );
        }

        bestViews.removeAt( targetIndex );  // esborrem el target de bestViews

        emit partialProgress( 100 * ( nBestViews - bestViews.size() ) / nBestViews );
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }
}


void ViewpointInformationChannel::computeExploratoryTour()
{
    int nViewpoints = m_viewpoints.size();
    int nVoxels = m_volume->getSize();

    emit partialProgress( 0 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    m_exploratoryTour.clear();

    // millor vista
    float minVmi = m_vmi.at( 0 );
    int minVmiIndex = 0;

    for ( int i = 1; i < nViewpoints; i++ )
    {
        float vmi = m_vmi.at( i );

        if ( vmi < minVmi )
        {
            minVmi = vmi;
            minVmiIndex = i;
        }
    }

    m_exploratoryTour << qMakePair( minVmiIndex, m_viewpoints.at( minVmiIndex ) );

    QList<int> viewpointIndexList;
    for ( int i = 0; i < nViewpoints; i++ ) viewpointIndexList << i;
    viewpointIndexList.removeAt( minVmiIndex );

    float pvv = m_viewProbabilities.at( minVmiIndex );    // p(v̂)

    QVector<float> pZvv = voxelProbabilitiesInView( minVmiIndex );  // p(Z|v̂)

    float IvvZ = minVmi;    // I(v̂;Z)
    float ratio = IvvZ / m_mi;  // I(v̂;Z) / I(V;Z)

    DEBUG_LOG( QString( "I(V;Z) = %1" ).arg( m_mi ) );
    DEBUG_LOG( "Exploratory tour:" );
    DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂;Z) = %4; I(v̂;Z)/I(V;Z) = %5" ).arg( 0 ).arg( minVmiIndex + 1 ).arg( m_viewpoints.at( minVmiIndex ).toString() ).arg( IvvZ ).arg( ratio ) );

    while ( ratio > m_exploratoryTourThreshold )
    {
        int nRemainingViews = viewpointIndexList.size();

        if ( nRemainingViews == 0 ) break;

        float pvvMin = 0.0f;
        QVector<float> pZvvMin;
        float IvvZMin = 0.0f;
        float ratioMin = 0.0f;
        int iMin = -1;

        int currentIndex = m_exploratoryTour.last().first;

        QVector<int> neighbours = m_viewpointGenerator.neighbours( currentIndex );
        int nNeighbours = neighbours.size();

        for ( int k = 0; k < nNeighbours; k++ )
        {
            int viewIndex = neighbours.at( k );

            if ( !viewpointIndexList.contains( viewIndex ) ) continue;

            float pvi = m_viewProbabilities.at( viewIndex );
            QVector<float> pZvi = voxelProbabilitiesInView( viewIndex );    // p(Z|vi)

            float pvvi = pvv + pvi;         // p(v̂) afegint aquesta vista
            QVector<float> pZvvi( pZvv );   // vector p(Z|v̂) afegint aquesta vista
            for ( int j = 0; j < nVoxels; j++ ) pZvvi[j] = ( pvv * pZvv.at( j ) + pvi * pZvi.at( j ) ) / pvvi;
            float IvviZ = InformationTheory::kullbackLeiblerDivergence( pZvvi, m_voxelProbabilities );  // I(v̂,Z) afegint aquesta vista
            float ratioi = IvviZ / m_mi;

            if ( iMin < 0 || ratioi < ratioMin )
            {
                pvvMin = pvvi;
                pZvvMin = pZvvi;
                IvvZMin = IvviZ;
                ratioMin = ratioi;
                iMin = viewpointIndexList.indexOf( viewIndex );
            }
        }

        if ( iMin < 0 ) break;  // no hi ha més veïns que puguem afegir, per tant pleguem

        pvv = pvvMin;
        pZvv = pZvvMin;
        IvvZ = IvvZMin;
        ratio = ratioMin;
        int viewIndex = viewpointIndexList.takeAt( iMin );
        m_exploratoryTour << qMakePair( viewIndex, m_viewpoints.at( viewIndex ) );
        DEBUG_LOG( QString( "%1: (v%2) = %3; I(v̂;Z) = %4; I(v̂;Z)/I(V;Z) = %5" ).arg( m_exploratoryTour.size() - 1 ).arg( viewIndex + 1 ).arg( m_viewpoints.at( viewIndex ).toString() ).arg( IvvZ ).arg( ratio ) );
    }

    emit partialProgress( 100 );
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
}


} // namespace udg
