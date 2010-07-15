#ifndef VIEWPOINTINFORMATIONCHANNEL_H
#define VIEWPOINTINFORMATIONCHANNEL_H


#include <QObject>

#include <QPair>

#include "transferfunction.h"
#include "viewpointgenerator.h"


#ifndef CUDA_AVAILABLE
class QTemporaryFile;
#endif


namespace udg {


class Experimental3DVolume;
class QExperimental3DViewer;

#ifdef CUDA_AVAILABLE
class Matrix4;
#endif


/**
 * Representa el canal d'informació entre un conjunt de punts de vista (V) i un conjunt de vòxels del volum (Z). Conté mètodes per calcular mesures relatives a aquest canal.
 */
class ViewpointInformationChannel : public QObject
{

    Q_OBJECT

public:

    ViewpointInformationChannel(const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer, const TransferFunction &transferFunction);

    void setColorVomiPalette(const QVector<Vector3Float> &colorVomiPalette);
    void setEvmiOpacityTransferFunction(const TransferFunction &evmiOpacityTransferFunction);
    void setBestViewsParameters(bool fixedNumber, int n, float threshold);
    void setExploratoryTourThreshold(float threshold);

    /// Filtra el conjunt de punts de vista que es faran servir.
    /// \a filter Vector que conté un booleà per cada punt de vista original. Es faran servir els que estiguin a cert.
    void filterViewpoints(const QVector<bool> &filter);
    /// Calcula les mesures demanades. Si en calcula més per dependències actualitza els paràmetres corresponents.
    void compute(bool &HV, bool &HVz, bool &HZ, bool &HZv, bool &HZV, bool &vmi, bool &vmi2, bool &vmi3, bool &mi, bool &viewpointUnstabilities, bool &vomi, bool &vomi2, bool &vomi3, bool &viewpointVomi,
                 bool &colorVomi, bool &evmiOpacity, bool &evmiVomi, bool &bestViews, bool &guidedTour, bool &exploratoryTour, bool display = false);
    bool hasViewedVolume() const;
    const QVector<float>& viewedVolume() const;
    float HV() const;                   // H(V)
    const QVector<float>& HVz() const;  // H(V|z)
    float HZ() const;                   // H(Z)
    const QVector<float>& HZv() const;  // H(Z|v)
    float HZV() const;                  // H(Z|V)
    const QVector<float>& vmi() const;  // I₁(v;Z)
    const QVector<float>& vmi2() const; // I₂(v;Z)
    const QVector<float>& vmi3() const; // I₃(v;Z)
    float mi() const;
    const QVector<float>& viewpointUnstabilities() const;
    const QVector<float>& vomi() const;     // I₁(z;V)
    float minimumVomi() const;
    float maximumVomi() const;
    const QVector<float>& vomi2() const;    // I₂(z;V)
    float minimumVomi2() const;
    float maximumVomi2() const;
    const QVector<float>& vomi3() const;    // I₃(z;V)
    float minimumVomi3() const;
    float maximumVomi3() const;
    const QVector<float>& viewpointVomi() const;
    const QVector<Vector3Float>& colorVomi() const;
    float maximumColorVomi() const;
    const QVector<float>& evmiOpacity() const;
    const QVector<float>& evmiVomi() const;
    const QList< QPair<int, Vector3> >& bestViews() const;
    const QList< QPair<int, Vector3> >& guidedTour() const;
    const QList< QPair<int, Vector3> >& exploratoryTour() const;

signals:

    void totalProgressMaximum(int maximum);
    void totalProgress(int progress);
    void partialProgress(int progress);

private:

    QVector<float> voxelProbabilitiesInView(int i);

#ifndef CUDA_AVAILABLE
    void computeCpu(bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeHV, bool computeHVz, bool computeHZ, bool computeHZv, bool computeHZV, bool computeVmi, bool computeVmi2, bool computeVmi3,
                    bool computeMi, bool computeViewpointUnstabilities, bool computeVomi, bool computeVomi2, bool computeVomi3, bool computeViewpointVomi, bool computeColorVomi, bool computeEvmiOpacity,
                    bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour, bool computeExploratoryTour);
    void createVoxelProbabilitiesPerViewFiles();
    QVector<float> voxelProbabilitiesInViewCpu(int i);
    void deleteVoxelProbabilitiesPerViewFiles();
    float rayCastingCpu(bool computeViewProbabilities);
    void computeViewProbabilitiesAndEntropyCpu(float totalViewedVolume, bool computeHV);
    void computeVoxelProbabilitiesAndEntropyCpu(bool computeHZ);
    void computeViewMeasuresCpu(bool computeHZv, bool computeHZV, bool computeVmi, bool computeVmi2, bool computeMi, bool computeViewpointUnstabilities, bool computeViewpointVomi, bool computeEvmiOpacity,
                                bool computeEvmiVomi);
    void computeVmi3Cpu();
    void computeVomiCpu(bool computeHVz, bool computeVomi, bool computeColorVomi);
    void computeVomi2Cpu();
    void computeVomi3Cpu();
#else // CUDA_AVAILABLE
    static Matrix4 viewMatrix(const Vector3 &viewpoint);
    void computeCuda(bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeHV, bool computeHVz, bool computeHZ, bool computeHZv, bool computeHZV, bool computeVmi, bool computeVmi2,
                     bool computeVmi3, bool computeMi, bool computeViewpointUnstabilities, bool computeVomi, bool computeVomi2, bool computeVomi3, bool computeViewpointVomi, bool computeColorVomi,
                     bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour, bool computeExploratoryTour, bool display);
    QVector<float> voxelProbabilitiesInViewCuda(int i);
    void computeViewProbabilitiesAndEntropyCuda(bool computeHV);
    void computeVoxelProbabilitiesAndEntropyCuda(bool computeHZ);
    void computeViewMeasuresCuda(bool computeHZv, bool computeHZV, bool computeVmi, bool computeVmi2, bool computeMi, bool computeViewpointUnstabilities, bool computeViewpointVomi, bool computeEvmiOpacity,
                                 bool computeEvmiVomi);
    void computeVmi3Cuda();
    void computeVomiCuda(bool computeHVz, bool computeVomi, bool computeColorVomi);
    void computeVomi2Cuda();
    void computeVomi3Cuda();
#endif // CUDA_AVAILABLE

    void computeBestViews();
    void computeGuidedTour();
    void computeExploratoryTour();

private:

    ViewpointGenerator m_viewpointGenerator;
    Experimental3DVolume *m_volume;
    QExperimental3DViewer *m_viewer;
    TransferFunction m_transferFunction;

    QColor m_backgroundColor;
    QVector<Vector3> m_viewpoints;

#ifndef CUDA_AVAILABLE
    QVector<QTemporaryFile*> m_voxelProbabilitiesPerViewFiles;  // p(Z|V)
#endif

    QVector<float> m_viewedVolume;          // volum vist des de cada vista
    QVector<float> m_viewProbabilities;     // p(V)
    QVector<float> m_voxelProbabilities;    // p(Z)

    float m_HV;             // H(V)
    QVector<float> m_HVz;   // H(V|z)
    float m_HZ;             // H(Z)
    QVector<float> m_HZv;   // H(Z|v)
    float m_HZV;            // H(Z|V)
    QVector<float> m_vmi;   // I₁(v;Z)
    QVector<float> m_vmi2;  // I₂(v;Z)
    QVector<float> m_vmi3;  // I₃(v;Z)
    float m_mi;
    QVector<float> m_viewpointUnstabilities;
    QVector<float> m_vomi;  // I₁(z;V)
    float m_minimumVomi;
    float m_maximumVomi;
    QVector<float> m_vomi2; // I₂(z;V)
    float m_minimumVomi2;
    float m_maximumVomi2;
    QVector<float> m_vomi3; // I₃(z;V)
    float m_minimumVomi3;
    float m_maximumVomi3;
    QVector<float> m_viewpointVomi;
    QVector<Vector3Float> m_viewpointColors;
    QVector<Vector3Float> m_colorVomi;
    float m_maximumColorVomi;
    TransferFunction m_evmiOpacityTransferFunction;
    QVector<float> m_evmiOpacity;
    QVector<float> m_evmiVomi;
    bool m_fixedNumberOfBestViews;
    int m_numberOfBestViews;
    float m_bestViewsThreshold;
    QList< QPair<int, Vector3> > m_bestViews;
    QList< QPair<int, Vector3> > m_guidedTour;
    float m_exploratoryTourThreshold;
    QList< QPair<int, Vector3> > m_exploratoryTour;

};


} // namespace udg


#endif // VIEWPOINTINFORMATIONCHANNEL_H
