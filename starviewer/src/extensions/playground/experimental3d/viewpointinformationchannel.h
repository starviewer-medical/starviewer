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

    ViewpointInformationChannel( const ViewpointGenerator &viewpointGenerator, Experimental3DVolume *volume, QExperimental3DViewer *viewer, const TransferFunction &transferFunction );

    void setColorVomiPalette( const QVector<Vector3Float> &colorVomiPalette );
    void setBestViewsParameters( bool fixedNumber, int n, float threshold );
    void setExploratoryTourThreshold( float threshold );
    /**
     * Filtra el conjunt de punts de vista que es faran servir.
     * \a filter Vector que conté un booleà per cada punt de vista original. Es faran servir els que estiguin a cert.
     */
    void filterViewpoints( const QVector<bool> &filter );
    /// Calcula les mesures demanades. Si en calcula més per dependències actualitza els paràmetres corresponents.
    void compute( bool &viewpointEntropy, bool &entropy, bool &vmi, bool &mi, bool &viewpointUnstabilities, bool &vomi, bool &viewpointVomi, bool &colorVomi, bool &evmiOpacity, bool &evmiVomi, bool &bestViews,
                  bool &guidedTour, bool &exploratoryTour, bool display = false );
    const QVector<float>& viewpointEntropy() const;
    float entropy() const;
    const QVector<float>& vmi() const;
    float mi() const;
    const QVector<float>& viewpointUnstabilities() const;
    const QVector<float>& vomi() const;
    float maximumVomi() const;
    const QVector<float>& viewpointVomi() const;
    const QVector<Vector3Float>& colorVomi() const;
    float maximumColorVomi() const;
    const QVector<float>& evmiOpacity() const;
    const QVector<float>& evmiVomi() const;
    const QList< QPair<int, Vector3> >& bestViews() const;
    const QList< QPair<int, Vector3> >& guidedTour() const;
    const QList< QPair<int, Vector3> >& exploratoryTour() const;

signals:

    void totalProgressMaximum( int maximum );
    void totalProgress( int progress );
    void partialProgress( int progress );

private:

    QVector<float> voxelProbabilitiesInView( int i );

#ifndef CUDA_AVAILABLE
    void computeCpu( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeViewpointUnstabilities,
                     bool computeVomi, bool computeViewpointVomi, bool computeColorVomi, bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour, bool computeExploratoryTour );
    void createVoxelProbabilitiesPerViewFiles();
    QVector<float> voxelProbabilitiesInViewCpu( int i );
    void deleteVoxelProbabilitiesPerViewFiles();
    float rayCastingCpu( bool computeViewProbabilities );
    void computeViewProbabilitiesCpu( float totalViewedVolume );
    void computeVoxelProbabilitiesCpu();
    void computeViewMeasuresCpu( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeViewpointUnstabilities, bool computeViewpointVomi, bool computeEvmiOpacity,
                                 bool computeEvmiVomi );
    void computeVomiCpu( bool computeVomi, bool computeColorVomi );
#else // CUDA_AVAILABLE
    static Matrix4 viewMatrix( const Vector3 &viewpoint );
    void computeCuda( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeViewpointUnstabilities,
                      bool computeVomi, bool computeViewpointVomi, bool computeColorVomi, bool computeEvmiOpacity, bool computeEvmiVomi, bool computeBestViews, bool computeGuidedTour, bool computeExploratoryTour,
                      bool display );
    QVector<float> voxelProbabilitiesInViewCuda( int i );
    void computeViewProbabilitiesCuda();
    void computeVoxelProbabilitiesCuda();
    void computeViewMeasuresCuda( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeViewpointUnstabilities, bool computeViewpointVomi, bool computeEvmiOpacity,
                                  bool computeEvmiVomi );
    void computeVomiCuda( bool computeVomi, bool computeColorVomi );
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

    QVector<float> m_viewProbabilities;     // p(V)
    QVector<float> m_voxelProbabilities;    // p(Z)

    QVector<float> m_viewpointEntropy;
    float m_entropy;
    QVector<float> m_vmi;
    float m_mi;
    QVector<float> m_viewpointUnstabilities;
    QVector<float> m_vomi;
    float m_maximumVomi;
    QVector<float> m_viewpointVomi;
    QVector<Vector3Float> m_viewpointColors;
    QVector<Vector3Float> m_colorVomi;
    float m_maximumColorVomi;
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
