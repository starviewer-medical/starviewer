#ifndef VIEWPOINTINFORMATIONCHANNEL_H
#define VIEWPOINTINFORMATIONCHANNEL_H


#include <QObject>

#include "transferfunction.h"
#include "viewpointgenerator.h"


namespace udg {


class Experimental3DVolume;
class Matrix4;


/**
 * Representa el canal d'informació entre un conjunt de punts de vista (V) i un conjunt de vòxels del volum (Z). Conté mètodes per calcular mesures relatives a aquest canal.
 */
class ViewpointInformationChannel : public QObject
{

    Q_OBJECT

public:

    ViewpointInformationChannel( const ViewpointGenerator &viewpointGenerator, const Experimental3DVolume *volume, const TransferFunction &transferFunction, const QColor &backgroundColor );

    /**
     * Filtra el conjunt de punts de vista que es faran servir.
     * \a filter Vector que conté un booleà per cada punt de vista original. Es faran servir els que estiguin a cert.
     */
    void filterViewpoints( const QVector<bool> &filter );
    /// Calcula les mesures demanades. Si en calcula més per dependències actualitza els paràmetres corresponents.
    void compute( bool &viewpointEntropy, bool &entropy, bool &vmi );
    const QVector<float>& viewpointEntropy() const;
    float entropy() const;
    const QVector<float>& vmi() const;

signals:

    void totalProgressMaximum( int maximum );
    void totalProgress( int progress );
    void partialProgress( int progress );

private:

    static Matrix4 viewMatrix( const Vector3 &viewpoint );

#ifndef CUDA_AVAILABLE
    void computeCpu( bool viewpointEntropy );
#else // CUDA_AVAILABLE
    void computeCuda( bool computeViewProbabilities, bool computeVoxelProbabilities, bool computeViewpointEntropy, bool computeEntropy, bool computeVmi );
    void computeViewProbabilitiesCuda();
    void computeVoxelProbabilitiesCuda();
    void computeViewMeasuresCuda( bool computeViewpointEntropy, bool computeEntropy, bool computeVmi );
#endif // CUDA_AVAILABLE

private:

    ViewpointGenerator m_viewpointGenerator;
    const Experimental3DVolume *m_volume;
    TransferFunction m_transferFunction;
    QColor m_backgroundColor;

    QVector<Vector3> m_viewpoints;

    QVector<float> m_viewProbabilities;
    QVector<float> m_voxelProbabilities;

    QVector<float> m_viewpointEntropy;
    float m_entropy;
    QVector<float> m_vmi;

};


} // namespace udg


#endif // VIEWPOINTINFORMATIONCHANNEL_H
