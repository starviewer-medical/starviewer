#ifndef VIEWPOINTINFORMATIONCHANNEL_H
#define VIEWPOINTINFORMATIONCHANNEL_H


#include <QObject>

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

    /**
     * Filtra el conjunt de punts de vista que es faran servir.
     * \a filter Vector que conté un booleà per cada punt de vista original. Es faran servir els que estiguin a cert.
     */
    void filterViewpoints( const QVector<bool> &filter );
    /// Calcula les mesures demanades. Si en calcula més per dependències actualitza els paràmetres corresponents.
    void compute( bool &viewpointEntropy, bool &entropy, bool &vmi, bool &mi, bool &vomi );
    const QVector<float>& viewpointEntropy() const;
    float entropy() const;
    const QVector<float>& vmi() const;
    float mi() const;
    const QVector<float>& vomi() const;
    float maximumVomi() const;

signals:

    void totalProgressMaximum( int maximum );
    void totalProgress( int progress );
    void partialProgress( int progress );

private:

#ifndef CUDA_AVAILABLE
    void computeCpu( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeVomi );
    void createVoxelProbabilitiesPerViewFiles();
    void readVoxelProbabilitiesInView( int i, QVector<float> &voxelProbabilitiesInView );
    void deleteVoxelProbabilitiesPerViewFiles();
    float rayCastingCpu( bool computeViewProbabilities );
    void computeViewProbabilitiesCpu( float totalViewedVolume );
    void computeVoxelProbabilitiesCpu();
    void computeViewMeasuresCpu( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi );
    void computeVomiCpu();
#else // CUDA_AVAILABLE
    static Matrix4 viewMatrix( const Vector3 &viewpoint );
    void computeCuda( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi, bool computeVomi );
    void computeViewProbabilitiesCuda();
    void computeVoxelProbabilitiesCuda();
    void computeViewMeasuresCuda( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi, bool computeMi );
    void computeVomiCuda();
#endif // CUDA_AVAILABLE

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
    QVector<float> m_vomi;
    float m_maximumVomi;

};


} // namespace udg


#endif // VIEWPOINTINFORMATIONCHANNEL_H
