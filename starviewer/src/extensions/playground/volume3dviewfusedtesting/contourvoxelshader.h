#ifndef UDGCONTOURVOXELSHADER_H
#define UDGCONTOURVOXELSHADER_H


#include "voxelshader.h"


class vtkDirectionEncoder;
class vtkEncodedGradientEstimator;


namespace udg {


/**
 * És un voxel shader que pinta un contorn negre en funcio d'un paràmetre.
 */
class ContourVoxelShader : public VoxelShader {

public:

    ContourVoxelShader();
    virtual ~ContourVoxelShader();

    /// Assigna l'estimador del gradient.
    void setGradientEstimator( vtkEncodedGradientEstimator *gradientEstimator );
    /// Assigna el llindar a partir del qual s'aplica el contorn.
    void setThreshold( double threshold );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    unsigned short *m_encodedNormals;
    vtkDirectionEncoder *m_directionEncoder;
    double m_threshold;

};


}


#endif
