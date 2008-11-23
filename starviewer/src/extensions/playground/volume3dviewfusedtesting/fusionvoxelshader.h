/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGFUSIONVOXELSHADER_H
#define UDGFUSIONVOXELSHADER_H

#include "ambientvoxelshader.h"

namespace udg {

/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class FusionVoxelShader: public AmbientVoxelShader{
public:
    FusionVoxelShader();

    virtual ~FusionVoxelShader();

	 /// Assigna la funció de transferència del segon model. La primera ja la té del pare
    void setTransferFunction2( const TransferFunction &transferFunction );
	/// Assigna les dades del segon model. Les del primer ja la té del pare
	void setData2( const unsigned char *data2 );
	///especifica quin tipus de metode fa servir (0->interp lineal/1->un de cada.
	void setMethod(int m);

/// Retorna el color corresponent al vòxel a la posició offset.
    virtual HdrColor shade( int offset, const Vector3 &direction, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna el color corresponent al vòxel a la posició position, fent servir valors interpolats.
    virtual HdrColor shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor = HdrColor() ) const;
    /// Retorna un string representatiu del voxel shader.
    virtual QString toString() const;

protected:

    /// Omple la taula de colors ambient.
    void precomputeAmbientColors2();

    static const unsigned int AMBIENT_COLORS_TABLE_SIZE2 = 256;

    TransferFunction m_transferFunction2;
    HdrColor m_ambientColors2[AMBIENT_COLORS_TABLE_SIZE2];
	///Data del segon model
	 const unsigned char *m_data2;
	int m_method;

};

}

#endif
