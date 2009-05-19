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
    void compute( bool viewpointEntropy );

signals:

    void totalProgressMaximum( int maximum );
    void totalProgress( int progress );
    void partialProgress( int progress );

private:

    static Matrix4 viewMatrix( const Vector3 &viewpoint );

#ifndef CUDA_AVAILABLE
    void computeCpu( bool viewpointEntropy );
#else // CUDA_AVAILABLE
    void computeCuda( bool viewpointEntropy );
    void computeViewProbabilitiesCuda();
    void computeVoxelProbabilitiesCuda();
#endif // CUDA_AVAILABLE

private:

    ViewpointGenerator m_viewpointGenerator;
    const Experimental3DVolume *m_volume;
    TransferFunction m_transferFunction;
    QColor m_backgroundColor;

    QVector<Vector3> m_viewpoints;
    QVector<float> m_viewProbabilities;
    QVector<float> m_voxelProbabilities;

};


} // namespace udg


#endif // VIEWPOINTINFORMATIONCHANNEL_H
