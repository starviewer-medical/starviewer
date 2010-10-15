#ifndef VIEWPOINTINTENSITYINFORMATIONCHANNEL_H
#define VIEWPOINTINTENSITYINFORMATIONCHANNEL_H


#ifdef CUDA_AVAILABLE


#include <QObject>

#include <QPair>

#include "transferfunction.h"
#include "viewpointgenerator.h"


namespace udg {


class Experimental3DVolume;
class QExperimental3DViewer;
class Matrix4;


/**
 * Representa el canal d'informació entre un conjunt de punts de vista (V) i un conjunt de valors d'intensitat del volum (I). Conté mètodes per calcular mesures relatives a aquest canal.
 */
class ViewpointIntensityInformationChannel : public QObject
{

    Q_OBJECT

public:

    ViewpointIntensityInformationChannel(const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer, const TransferFunction &transferFunction);

    void setIntensityClusteringNumberOfClusters(int numberOfClusters);

    /// Filtra el conjunt de punts de vista que es faran servir.
    /// \a filter Vector que conté un booleà per cada punt de vista original. Es faran servir els que estiguin a cert.
    void filterViewpoints(const QVector<bool> &filter);
    /// Calcula les mesures demanades. Si en calcula més per dependències actualitza els paràmetres corresponents.
    void compute(bool &pI, bool &HI, bool &HIv, bool &HIV, bool &jointEntropy, bool &vmii, bool &mii, bool &viewpointUnstabilities, bool &imi, bool &intensityClustering, bool display = false);
    bool hasViewedVolume() const;
    const QVector<float>& viewedVolume() const;
    const QVector<float>& intensityProbabilities() const;   // p(I)
    float HI() const;                                       // H(I)
    const QVector<float>& HIv() const;                      // H(I|v)
    float HIV() const;                                      // H(I|V)
    float jointEntropy() const;                             // H(V,I)
    const QVector<float>& vmii() const;
    float mii() const;
    const QVector<float>& viewpointUnstabilities() const;
    const QVector<float>& imi() const;
    float maximumImi() const;
    QList< QList<int> > intensityClusters() const;

signals:

    void totalProgressMaximum(int maximum);
    void totalProgress(int progress);
    void partialProgress(int progress);

private:

    QVector<float> intensityProbabilitiesInView( int i );

    static Matrix4 viewMatrix( const Vector3 &viewpoint );
    void computeCuda(bool computeViewProbabilities, bool computeIntensityProbabilities, bool computeHI, bool computeHIv, bool computeHIV, bool computeJointEntropy, bool computeVmii, bool computeMii,
                     bool computeViewpointUnstabilities, bool computeImi, bool computeIntensityClustering, bool display);
    QVector<float> intensityProbabilitiesInViewCuda(int i);
    void computeViewProbabilitiesCuda();
    void computeIntensityProbabilitiesAndEntropyCuda(bool computeHI);
    void computeViewMeasuresCuda(bool computeViewpointEntropy, bool computeEntropy, bool computeJHVI, bool computeVmii, bool computeMii, bool computeViewpointUnstabilities/*, bool computeViewpointVomi,
                                 bool computeEvmiOpacity, bool computeEvmiVomi*/);
    void computeImiCuda(/* bool computeImi, bool computeColorVomi */);
    void computeIntensityClusteringCuda();

private:

    ViewpointGenerator m_viewpointGenerator;
    Experimental3DVolume *m_volume;
    QExperimental3DViewer *m_viewer;
    TransferFunction m_transferFunction;

    QColor m_backgroundColor;
    QVector<Vector3> m_viewpoints;

    QVector<float> m_viewedVolume;              // volum vist des de cada vista
    QVector<float> m_viewProbabilities;         // p(V)
    QVector<float> m_intensityProbabilities;    // p(I)

    float m_HI;             // H(I)
    QVector<float> m_HIv;   // H(I|v)
    float m_HIV;            // H(I|V)
    float m_jointEntropy;   // H(V,I)
    QVector<float> m_vmii;
    float m_mii;
    QVector<float> m_viewpointUnstabilities;
    QVector<float> m_imi;
    float m_maximumImi;
    int m_numberOfIntensityClusters;
    QList< QList<int> > m_intensityClusters;

};


} // namespace udg


#endif // CUDA_AVAILABLE


#endif // VIEWPOINTINTENSITYINFORMATIONCHANNEL_H
