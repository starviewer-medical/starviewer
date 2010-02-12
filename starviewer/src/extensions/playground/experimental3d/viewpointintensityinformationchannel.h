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

    ViewpointIntensityInformationChannel( const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer, const TransferFunction &transferFunction );

    /**
     * Filtra el conjunt de punts de vista que es faran servir.
     * \a filter Vector que conté un booleà per cada punt de vista original. Es faran servir els que estiguin a cert.
     */
    void filterViewpoints( const QVector<bool> &filter );
    /// Calcula les mesures demanades. Si en calcula més per dependències actualitza els paràmetres corresponents.
    void compute( bool &viewpointEntropy, bool &entropy, bool &vmii, bool &mii, bool &viewpointUnstabilities, bool &imi, /*bool &viewpointVomi, bool &colorVomi, bool &evmiOpacity, bool &evmiVomi, bool &bestViews,
                  bool &guidedTour, bool &exploratoryTour,*/ bool display = false );
    bool hasViewedVolume() const;
    const QVector<float>& viewedVolume() const;
    const QVector<float>& viewpointEntropy() const;
    float entropy() const;
    const QVector<float>& vmii() const;
    float mii() const;
    const QVector<float>& viewpointUnstabilities() const;
    const QVector<float>& imi() const;
    float maximumImi() const;

signals:

    void totalProgressMaximum( int maximum );
    void totalProgress( int progress );
    void partialProgress( int progress );

private:

    QVector<float> intensityProbabilitiesInView( int i );

    static Matrix4 viewMatrix( const Vector3 &viewpoint );
    void computeCuda( bool computeViewProbabilities, bool computeIntensityProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmii, bool computeMii, bool computeViewpointUnstabilities,
                      bool computeImi, /*bool computeViewpointVomi, bool computeColorVomi, bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour, bool computeExploratoryTour,*/
                      bool display );
    QVector<float> intensityProbabilitiesInViewCuda( int i );
    void computeViewProbabilitiesCuda();
    void computeIntensityProbabilitiesCuda();
    void computeViewMeasuresCuda( bool computeViewpointEntropy, bool computeEntropy, bool computeVmii, bool computeMii, bool computeViewpointUnstabilities/*, bool computeViewpointVomi, bool computeEvmiOpacity,
                                  bool computeEvmiVomi*/ );
    void computeImiCuda(/* bool computeImi, bool computeColorVomi */);

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

    QVector<float> m_viewedVolume;  // volum vist des de cada vista
    QVector<float> m_viewProbabilities;     // p(V)
    QVector<float> m_intensityProbabilities;    // p(I)

    QVector<float> m_viewpointEntropy;
    float m_entropy;
    QVector<float> m_vmii;
    float m_mii;
    QVector<float> m_viewpointUnstabilities;
    QVector<float> m_imi;
    float m_maximumImi;
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


#endif // CUDA_AVAILABLE


#endif // VIEWPOINTINTENSITYINFORMATIONCHANNEL_H
