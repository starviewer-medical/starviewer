#ifndef UDGOBSCURANCEMAINTHREAD_H
#define UDGOBSCURANCEMAINTHREAD_H


#include <QThread>

#include <QVector>

#include "transferfunction.h"


class vtkVolume;


namespace udg {


class Vector3;


/**
 * Thread principal per al càlcul d'obscurances. Controla els altres threads.
 */
class ObscuranceMainThread : public QThread {

    Q_OBJECT

public:

    /// Estructura que guarda les coordenades d'un vòxel.
    struct Voxel { int x, y, z; };
    /// Funcions d'obscurances.
    enum Function { Constant0, Distance, SquareRoot, Exponential, ExponentialNorm, CubeRoot };
    /// Variants de les obscurances.
    enum Variant { Density, DensitySmooth, Opacity, OpacitySmooth, OpacitySaliency, OpacitySmoothSaliency, OpacityColorBleeding, OpacitySmoothColorBleeding };

    ObscuranceMainThread( int numberOfDirections, double maximumDistance, Function function, Variant variant, QObject *parent = 0 );
    virtual ~ObscuranceMainThread();

    bool hasColor() const;
    void setVolume( vtkVolume *volume );
    void setTransferFunction( const TransferFunction &transferFunction );
    void setObscurance( double *obscurance, Vector3 *colorBleeding );
    void setSaliency( const double *saliency, double fxSaliencyA, double fxSaliencyB, double fxSaliencyLow, double fxSaliencyHigh );

protected:

    virtual void run();

private:

    static void getLineStarts( QVector<Vector3> &lineStarts, int dimX, int dimY, int dimZ, const Vector3 &forward );

    int m_numberOfDirections;
    double m_maximumDistance;
    Function m_function;
    Variant m_variant;
    vtkVolume *m_volume;
    TransferFunction m_transferFunction;
    double *m_obscurance;
    Vector3 *m_colorBleeding;
    const double *m_saliency;
    double m_fxSaliencyA, m_fxSaliencyB;
    double m_fxSaliencyLow, m_fxSaliencyHigh;

};


}


#endif
