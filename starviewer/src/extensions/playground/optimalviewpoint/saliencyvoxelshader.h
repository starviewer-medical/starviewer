#ifndef UDGSALIENCYVOXELSHADER_H
#define UDGSALIENCYVOXELSHADER_H


#include <voxelshader.h>


namespace udg {


/**
 * Voxel shader que aplica efectes de saliency.
 */
class SaliencyVoxelShader : public VoxelShader {

public:

    SaliencyVoxelShader();
    virtual ~SaliencyVoxelShader();

    /// Assigna l'array de saliency.
    void setSaliency( const double *saliency );
    /// Assigna els paràmetres per escalar la saliency: el mínim que s'aplicarà serà 1 - a i el màxim 1 + b.
    void setScale( double a, double b );
    /// Assigna els filtres de saliency: per sota de \a low es considera el mínim i per sobre de \a high es considera el màxim.
    void setFilters( double low, double high );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset, const Vector3 &direction, const QColor &baseColor = Qt::white ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const double *m_saliency;
    double m_a;
    double m_lowFilter, m_highFilter;
    double m_minimum, m_maximum, m_ab;

};


}


#endif
