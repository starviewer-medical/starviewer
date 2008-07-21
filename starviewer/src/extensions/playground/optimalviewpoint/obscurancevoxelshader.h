#ifndef UDGOBSCURANCEVOXELSHADER_H
#define UDGOBSCURANCEVOXELSHADER_H


#include <voxelshader.h>


namespace udg {


/**
 * Voxel shader que aplica obscurances.
 */
class ObscuranceVoxelShader : public VoxelShader {

public:

    ObscuranceVoxelShader();
    virtual ~ObscuranceVoxelShader();

    /// Assigna l'array d'obscurances.
    void setObscurance( const double *obscurance );
    /// Assigna el factor pel qual es multipliquen les obscurances.
    void setFactor( double factor );
    /// Assigna els filtres d'obscurances: per sota de \a low es considera 0 i per sobre de \a high es considera 1.
    void setFilters( double low, double high );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset, const Vector3 &direction, const QColor &baseColor = Qt::white ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    const double *m_obscurance;
    double m_factor;
    double m_lowFilter, m_highFilter;

};


}


#endif
