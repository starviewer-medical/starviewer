#include "viewpointintensityinformationchannel.h"

#include "camera.h"
#include "cudaviewpointintensityinformationchannel.h"
#include "experimental3dvolume.h"
#include "informationtheory.h"
#include "logging.h"
#include "matrix4.h"
#include "qexperimental3dviewer.h"

#include <QCoreApplication>
#include <QSet>
#include <QThread>

namespace udg {

ViewpointIntensityInformationChannel::ViewpointIntensityInformationChannel(const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume,
                                                                           QExperimental3DViewer *viewer, const TransferFunction &transferFunction)
    : QObject(), m_viewpointGenerator(viewpointGenerator), m_volume(volume), m_viewer(viewer), m_transferFunction(transferFunction)
{
    m_backgroundColor = m_viewer->getBackgroundColor();
    m_viewpoints = m_viewpointGenerator.viewpoints();
}

void ViewpointIntensityInformationChannel::setViewpoints(const QVector<Vector3> &viewpoints)
{
    m_viewpoints = viewpoints;
}

void ViewpointIntensityInformationChannel::setIntensityClusteringNumberOfClusters(int numberOfClusters)
{
    m_numberOfIntensityClusters = numberOfClusters;
}

void ViewpointIntensityInformationChannel::filterViewpoints(const QVector<bool> &filter)
{
    if (m_viewpoints.size() != filter.size())
    {
        DEBUG_LOG(QString("Error: mides dels vectors diferents al filtre de punts de vista: %1 punts de vista, %2 filtres").arg(m_viewpoints.size())
                                                                                                                           .arg(filter.size()));
        return;
    }

    m_viewpoints.clear();

    for (int i = 0; i < filter.size(); i++)
    {
        if (filter.at(i))
        {
            m_viewpoints << m_viewpointGenerator.viewpoint(i);
        }
    }
}

void ViewpointIntensityInformationChannel::compute(bool &intensityProbabilitiesGivenView, bool &viewProbabilities, bool &intensityProbabilities, bool &HI,
                                                   bool &HIv, bool &HIV, bool &jointEntropy, bool &vmii, bool &mii, bool &viewpointUnstabilities, bool &imi,
                                                   bool &intensityClustering, bool display)
{
    // Si no hi ha res a calcular marxem
    if (!intensityProbabilitiesGivenView && !viewProbabilities && !intensityProbabilities && !HI && !HIv && !HIV && !jointEntropy && !vmii && !mii
        && !viewpointUnstabilities && !imi && !intensityClustering)
    {
        return;
    }

    // Dependències
    if (HI)
    {
        intensityProbabilities = true;
    }
    if (HIV)
    {
        viewProbabilities = true;
    }
    if (HIV)
    {
        HIv = true;
    }
    if (jointEntropy)
    {
        viewProbabilities = true;
    }
    if (mii)
    {
        vmii = true;
    }
    if (vmii)
    {
        intensityProbabilities = true;
    }
    if (viewpointUnstabilities)
    {
        viewProbabilities = true;
    }
    if (imi)
    {
        intensityProbabilities = true;
    }
    if (intensityClustering)
    {
        intensityProbabilities = true;
    }
    if (intensityProbabilities)
    {
        viewProbabilities = true;
    }

    computeCuda(intensityProbabilitiesGivenView, viewProbabilities, intensityProbabilities, HI, HIv, HIV, jointEntropy, vmii, mii, viewpointUnstabilities, imi,
                intensityClustering, display);
}

bool ViewpointIntensityInformationChannel::hasViewedVolume() const
{
    return !m_viewedVolume.isEmpty();
}

const QVector<float>& ViewpointIntensityInformationChannel::viewedVolume() const
{
    return m_viewedVolume;
}

double ViewpointIntensityInformationChannel::totalViewedVolume() const
{
    return m_totalViewedVolume;
}

const QVector< QVector<float> >& ViewpointIntensityInformationChannel::intensityProbabilitiesGivenView() const
{
    return m_intensityProbabilitiesGivenView;
}

const QVector<float>& ViewpointIntensityInformationChannel::viewProbabilities() const
{
    return m_viewProbabilities;
}

const QVector<float>& ViewpointIntensityInformationChannel::intensityProbabilities() const
{
    return m_intensityProbabilities;
}

float ViewpointIntensityInformationChannel::HI() const
{
    return m_HI;
}

const QVector<float>& ViewpointIntensityInformationChannel::HIv() const
{
    return m_HIv;
}

float ViewpointIntensityInformationChannel::HIV() const
{
    return m_HIV;
}

float ViewpointIntensityInformationChannel::jointEntropy() const
{
    return m_jointEntropy;
}

const QVector<float>& ViewpointIntensityInformationChannel::vmii() const
{
    return m_vmii;
}

float ViewpointIntensityInformationChannel::mii() const
{
    return m_mii;
}

const QVector<float>& ViewpointIntensityInformationChannel::viewpointUnstabilities() const
{
    return m_viewpointUnstabilities;
}

const QVector<float>& ViewpointIntensityInformationChannel::imi() const
{
    return m_imi;
}

float ViewpointIntensityInformationChannel::maximumImi() const
{
    return m_maximumImi;
}

QList< QList<int> > ViewpointIntensityInformationChannel::intensityClusters() const
{
    return m_intensityClusters;
}

QVector<float> ViewpointIntensityInformationChannel::intensityProbabilitiesInView(int i)
{
    return intensityProbabilitiesInViewCuda(i);
}

Matrix4 ViewpointIntensityInformationChannel::viewMatrix(const Vector3 &viewpoint)
{
    Camera camera;
    camera.lookAt(viewpoint, Vector3(), ViewpointGenerator::up(viewpoint));
    return camera.getViewMatrix();
}

void ViewpointIntensityInformationChannel::computeCuda(bool computeIntensityProbabilitiesGivenView, bool computeViewProbabilities,
                                                       bool computeIntensityProbabilities, bool computeHI, bool computeHIv, bool computeHIV,
                                                       bool computeJointEntropy, bool computeVmii, bool computeMii, bool computeViewpointUnstabilities,
                                                       bool computeImi, bool computeIntensityClustering, bool display)
{
    DEBUG_LOG("computeCuda");

    // Inicialitzar progrés
    int nSteps = 0;
    // p(I|V) + p(V)
    if (computeIntensityProbabilitiesGivenView || computeViewProbabilities)
    {
        nSteps++;
    }
    // p(I) + H(I)
    if (computeIntensityProbabilities || computeHI)
    {
        nSteps++;
    }
    // IMI
    if (computeImi)
    {
        nSteps++;
    }
    // H(I|v) + H(I|V) + H(V,I) + VMIi + MIi + viewpoint unstabilities
    if (computeHIv || computeHIV || computeJointEntropy || computeVmii || computeMii || computeViewpointUnstabilities)
    {
        nSteps++;
    }
    // intensity clustering
    if (computeIntensityClustering)
    {
        nSteps++;
    }

    emit totalProgressMaximum(nSteps);
    int step = 0;
    emit totalProgress(step);

    // Inicialització de CUDA
    cviicSetupRayCast(m_volume->getImage(), m_transferFunction, 1024, 720, m_backgroundColor, display);
    if (computeIntensityProbabilities)
    {
        cviicSetupIntensityProbabilities();
    }

    // p(I|V) + p(V)
    if (computeIntensityProbabilitiesGivenView || computeViewProbabilities)
    {
        computeViewProbabilitiesCuda(computeIntensityProbabilitiesGivenView);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // p(I) + H(I)
    if (computeIntensityProbabilities || computeHI)
    {
        computeIntensityProbabilitiesAndEntropyCuda(computeHI);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // IMI
    if (computeImi)
    {
        computeImiCuda();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // H(I|v) + H(I|V) + H(V,I) + VMIi + MIi + viewpoint unstabilities
    if (computeHIv || computeHIV || computeJointEntropy || computeVmii || computeMii || computeViewpointUnstabilities)
    {
        computeViewMeasuresCuda(computeHIv, computeHIV, computeJointEntropy, computeVmii, computeMii, computeViewpointUnstabilities);
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // intensity clustering
    if (computeIntensityClustering)
    {
        computeIntensityClusteringCuda();
        emit totalProgress(++step);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    // Finalització de CUDA
    if (computeIntensityProbabilities)
    {
        cviicCleanupIntensityProbabilities();
    }
    cviicCleanupRayCast();
}

QVector<float> ViewpointIntensityInformationChannel::intensityProbabilitiesInViewCuda(int i)
{
    if (!m_intensityProbabilitiesGivenView.isEmpty())
    {
        return m_intensityProbabilitiesGivenView.at(i);
    }
    else
    {
        QVector<float> pIv = cviicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i)));   // p(I|v) * viewedVolume
        double viewedVolume = 0.0;
        int nIntensities = m_volume->getRangeMax() + 1;
        for (int j = 0; j < nIntensities; j++)
        {
            viewedVolume += pIv.at(j);
        }
        Q_ASSERT(!MathTools::isNaN(viewedVolume));
        for (int j = 0; j < nIntensities; j++)
        {
            pIv[j] /= viewedVolume;  // p(I|v)
        }
        return pIv;
    }
}

void ViewpointIntensityInformationChannel::computeViewProbabilitiesCuda(bool computeIntensityProbabilitiesGivenView)
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    if (computeIntensityProbabilitiesGivenView)
    {
        m_intensityProbabilitiesGivenView.fill(QVector<float>(nIntensities), nViewpoints);
    }

    m_viewProbabilities.resize(nViewpoints);
    double totalViewedVolume = 0.0;

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> histogram = cviicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i))); // p(I|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nIntensities; j++)
        {
            viewedVolume += histogram.at(j);
        }
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        if (computeIntensityProbabilitiesGivenView)
        {
            for (int j = 0; j < nIntensities; j++)
            {
                m_intensityProbabilitiesGivenView[i][j] = histogram.at(j) / viewedVolume;    // p(I|v)
            }
        }

        m_viewProbabilities[i] = viewedVolume;
        totalViewedVolume += viewedVolume;

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_viewedVolume = m_viewProbabilities;
    m_totalViewedVolume = totalViewedVolume;

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
}

void ViewpointIntensityInformationChannel::computeIntensityProbabilitiesAndEntropyCuda(bool computeHI)
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> histogram = cviicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i))); // p(I|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nIntensities; j++)
        {
            viewedVolume += histogram.at(j);
        }
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        cviicAccumulateIntensityProbabilities(m_viewProbabilities.at(i), viewedVolume);

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_intensityProbabilities = cviicGetIntensityProbabilities();

#ifndef QT_NO_DEBUG
    double sum = 0.0;
    for (int j = 0; j < nIntensities; j++)
    {
        float pi = m_intensityProbabilities.at(j);
        //DEBUG_LOG(QString("p(i%1) = %2").arg(j).arg(pi));
        Q_ASSERT(!MathTools::isNaN(pi));
        Q_ASSERT(pi >= 0.0f && pi <= 1.0f);
        sum += pi;
    }
    DEBUG_LOG(QString("sum p(i) = %1").arg(sum));
#endif

    if (computeHI)
    {
        m_HI = InformationTheory::entropy(m_intensityProbabilities);
        DEBUG_LOG(QString("H(I) = %1").arg(m_HI));
    }
}

void ViewpointIntensityInformationChannel::computeViewMeasuresCuda(bool computeHIv, bool computeHIV, bool computeJointEntropy, bool computeVmii,
                                                                   bool computeMii, bool computeViewpointUnstabilities)
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    if (computeHIv)
    {
        m_HIv.resize(nViewpoints);
    }
    if (computeHIV)
    {
        m_HIV = 0.0f;
    }
    if (computeJointEntropy)
    {
        m_jointEntropy = 0.0f;
    }
    if (computeVmii)
    {
        m_vmii.resize(nViewpoints);
    }
    if (computeMii)
    {
        m_mii = 0.0f;
    }
    if (computeViewpointUnstabilities)
    {
        m_viewpointUnstabilities.resize(nViewpoints);
    }

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    for (int i = 0; i < nViewpoints; i++)
    {
        QVector<float> intensityProbabilitiesInView = this->intensityProbabilitiesInView(i);    // p(I|v)

        if (computeHIv)
        {
            float HIv = InformationTheory::entropy(intensityProbabilitiesInView);
            Q_ASSERT(!MathTools::isNaN(HIv));
            m_HIv[i] = HIv;
            DEBUG_LOG(QString("H(I|v%1) = %2").arg(i + 1).arg(HIv));
        }

        if (computeHIV)
        {
            m_HIV += m_viewProbabilities.at(i) * m_HIv.at(i);
        }

        if (computeJointEntropy)
        {
            QVector<float> jpIv(intensityProbabilitiesInView);  // p(I,v) <- p(I|v)
            for (int j = 0; j < nIntensities; j++)
            {
                jpIv[j] *= m_viewProbabilities.at(i);    // p(I,v) = p(I|v) * p(v)
            }
            m_jointEntropy += InformationTheory::entropy(jpIv);
        }

        if (computeVmii)
        {
            float vmii = InformationTheory::kullbackLeiblerDivergence(intensityProbabilitiesInView, m_intensityProbabilities);
            Q_ASSERT(!MathTools::isNaN(vmii));
            m_vmii[i] = vmii;
            DEBUG_LOG(QString("VMIi(v%1) = %2").arg(i + 1).arg(vmii));
        }

        if (computeMii)
        {
            m_mii += m_viewProbabilities.at(i) * m_vmii.at(i);
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

                if (pvij == 0.0f)
                {
                    continue;
                }

                QVector<float> intensityProbabilitiesInNeighbour = this->intensityProbabilitiesInView(neighbour);   // p(I|vj)

                float viewpointDissimilarity = InformationTheory::jensenShannonDivergence(pvi / pvij, pvj / pvij, intensityProbabilitiesInView,
                                                                                          intensityProbabilitiesInNeighbour);
                viewpointUnstability += viewpointDissimilarity;
            }

            viewpointUnstability /= nNeighbours;
            m_viewpointUnstabilities[i] = viewpointUnstability;
            DEBUG_LOG(QString("U(v%1) = %2").arg(i + 1).arg(viewpointUnstability));
        }

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    if (computeHIV)
    {
        Q_ASSERT(!MathTools::isNaN(m_HIV));
        Q_ASSERT(m_HIV >= 0.0f);
        DEBUG_LOG(QString("H(I|V) = %1").arg(m_HIV));
    }

    if (computeJointEntropy)
    {
        Q_ASSERT(!MathTools::isNaN(m_jointEntropy));
        Q_ASSERT(m_jointEntropy >= 0.0f);
        DEBUG_LOG(QString("H(V,I) = %1").arg(m_jointEntropy));
    }

    if (computeMii)
    {
        Q_ASSERT(!MathTools::isNaN(m_mii));
        Q_ASSERT(m_mii >= 0.0f);
        DEBUG_LOG(QString("I(V;I) = %1").arg(m_mii));
    }
}

void ViewpointIntensityInformationChannel::computeImiCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    emit partialProgress(0);
    QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid

    cviicSetupImi();

    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities.at(i);
        if (pv == 0.0f)
        {
            continue;
        }

        QVector<float> histogram = cviicRayCastAndGetHistogram(m_viewpoints.at(i), viewMatrix(m_viewpoints.at(i))); // p(I|v) * viewedVolume

        double viewedVolume = 0.0;
        for (int j = 0; j < nIntensities; j++)
        {
            viewedVolume += histogram.at(j);
        }
        Q_ASSERT(!MathTools::isNaN(viewedVolume));

        cviicAccumulateImi(pv, viewedVolume);

        emit partialProgress(100 * (i + 1) / nViewpoints);
        QCoreApplication::processEvents();  // necessari perquè el procés vagi fluid
    }

    m_imi = cviicGetImi();
    m_maximumImi = 0.0f;

    cviicCleanupImi();

    for (int j = 0; j < nIntensities; j++)
    {
        float imi = m_imi.at(j);
        Q_ASSERT(!MathTools::isNaN(imi));
        Q_ASSERT(imi >= 0.0f);
        if (imi > m_maximumImi)
        {
            m_maximumImi = imi;
        }
    }
}

void ViewpointIntensityInformationChannel::computeIntensityClusteringCuda()
{
    int nViewpoints = m_viewpoints.size();
    int nIntensities = m_volume->getRangeMax() + 1;

    // Construïm el vector de probabilitats dels clusters p(C): inicialment és igual a p(I)
    QList<float> clusterProbabilities = QList<float>::fromVector(m_intensityProbabilities);    // p(C)

    // Construïm la matriu p(V|C), on cada fila sigui una p(V|c): inicialment és p(V|I), on cada fila és una p(V|i)
    QList< QVector<float> > pVC;
    for (int j = 0; j < nIntensities; j++)
    {
        pVC << QVector<float>(nViewpoints);  // tota la matriu queda plena de zeros inicialment
    }
    for (int i = 0; i < nViewpoints; i++)
    {
        float pv = m_viewProbabilities[i];  // p(v)
        if (pv == 0.0f)
        {
            continue;
        }
        QVector<float> pIv = intensityProbabilitiesInView(i);   // p(I|v)
        for (int j = 0; j < nIntensities; j++)
        {
            float piv = pIv[j]; // p(i|v)
            if (piv == 0.0f)
            {
                continue;
            }
            float pi = m_intensityProbabilities[j]; // p(i)
            if (pi == 0.0f)
            {
                continue;
            }
            float pvi = pv * piv / pi;
            Q_ASSERT(!MathTools::isNaN(pvi));
            pVC[j][i] = pvi;
        }
    }

    // Construïm el clustering inicial: un cluster per cada intensitat.
    QList< QList<int> > clusters;   // cada element és un cluster (un cluster està representat per la llista d'intensitats que el formen)
    for (int j = 0; j < nIntensities; j++)
    {
        clusters << (QList<int>() << j); // clusters = [[0], [1], ..., [nIntensities-1]]
    }

    // Calculem la pèrdua d'informació mútua per cada possible clustering
    // cada element és la pèrdua d'informació mútua resultat de fusionar l'element de la mateixa posició de clusters amb el següent
    QList<double> mutualInformationDecrease;
    for (int j = 0; j < nIntensities - 1; j++)
    {
        float pci = clusterProbabilities[j];
        float pcj = clusterProbabilities[j+1];
        float pcij = pci + pcj;
        mutualInformationDecrease << (pcij == 0.0f ? 0.0f : pcij * InformationTheory::jensenShannonDivergence(pci / pcij, pcj / pcij, pVC[j], pVC[j+1]));
    }

    while (clusters.size() > m_numberOfIntensityClusters)
    {
        // Trobem la pèrdua mínima d'informació mútua
        int minDecreaseIndex = 0;
        double minDecrease = mutualInformationDecrease[0];
        for (int k = 1; k < mutualInformationDecrease.size(); k++)
        {
            double decrease = mutualInformationDecrease[k];
            if (decrease < minDecrease)
            {
                minDecreaseIndex = k;
                minDecrease = decrease;
            }
        }

        // Fusionem les dues intensitats amb pèrdua mínima d'informació mútua i actualitzem les dades
        clusters[minDecreaseIndex] << clusters[minDecreaseIndex+1]; // integrem el segon cluster c2 al primer c1
        clusters.removeAt(minDecreaseIndex+1);  // esborrem el segon cluster c2
        // integrem p(V|c2) a p(V|c1)
        {
            float pc1 = clusterProbabilities[minDecreaseIndex];     // p(c1)
            float pc2 = clusterProbabilities[minDecreaseIndex+1];   // p(c2)
            float pc12 = pc1 + pc2;                                 // p(c12)
            if (pc12 > 0.0f)    // si p(c12) = 0, llavors p(V|c1) i p(V|c2) estan plens de zeros, per tant no cal fer res
            {
                QVector<float> &pVc1 = pVC[minDecreaseIndex];           // p(V|c1)
                const QVector<float> &pVc2 = pVC[minDecreaseIndex+1];   // p(V|c2)
                for (int i = 0; i < nViewpoints; i++)
                {
                    float pvc1 = pVc1[i];   // p(v|c1)
                    float pvc2 = pVc2[i];   // p(v|c2)
                    pVc1[i] = (pc1 * pvc1 + pc2 * pvc2) / pc12;
                }
            }
        }
        pVC.removeAt(minDecreaseIndex+1);   // esborrem p(V|c2)
        clusterProbabilities[minDecreaseIndex] += clusterProbabilities[minDecreaseIndex+1]; // integrem p(c2) a p(c1)
        clusterProbabilities.removeAt(minDecreaseIndex+1);  // esborrem p(c2)
        mutualInformationDecrease.removeAt(minDecreaseIndex);   // esborrem la pèrdua d'informació mútua associada a la fusió
        // actualitzem les pèrdues d'informació pertinents
        {
            if (minDecreaseIndex > 0)   // anterior
            {
                int i = minDecreaseIndex - 1;
                int j = minDecreaseIndex;
                float pci = clusterProbabilities[i];
                float pcj = clusterProbabilities[j];
                float pcij = pci + pcj;
                mutualInformationDecrease[i] = (pcij == 0.0f ? 0.0f : pcij * InformationTheory::jensenShannonDivergence(pci / pcij, pcj / pcij, pVC[i],
                                                                                                                        pVC[j]));
            }
            if (minDecreaseIndex < mutualInformationDecrease.size())    // següent
            {
                int i = minDecreaseIndex;
                int j = minDecreaseIndex + 1;
                float pci = clusterProbabilities[i];
                float pcj = clusterProbabilities[j];
                float pcij = pci + pcj;
                mutualInformationDecrease[i] = (pcij == 0.0f ? 0.0f : pcij * InformationTheory::jensenShannonDivergence(pci / pcij, pcj / pcij, pVC[i],
                                                                                                                        pVC[j]));
            }
        }

#ifndef QT_NO_DEBUG
        if (clusters.size() % 10 == m_numberOfIntensityClusters % 10)
        {
            QString clustersString = "[";
            for (int i = 0; i < clusters.size(); i++)
            {
                if (i > 0)
                {
                    clustersString += ", ";
                }
                clustersString += "[";
                clustersString += QString::number(clusters[i].first());
                if (clusters[i].size() > 1)
                {
                    clustersString += "-" + QString::number(clusters[i].last());
                }
                clustersString += "]";
            }
            clustersString += "]";
            DEBUG_LOG(clustersString);
        }
#endif
    }

    m_intensityClusters = clusters;
}

} // namespace udg
