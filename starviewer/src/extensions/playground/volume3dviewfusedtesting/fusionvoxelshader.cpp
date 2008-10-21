/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#include "fusionvoxelshader.h"
#include "logging.h"

namespace udg {

FusionVoxelShader::FusionVoxelShader()
	:AmbientVoxelShader()
{
}


FusionVoxelShader::~FusionVoxelShader()
{
	m_method=0;
}

void FusionVoxelShader::setData2( const unsigned char *data2 )
{
    m_data2 = data2;
}
void FusionVoxelShader::setTransferFunction2( const TransferFunction &transferFunction )
{
    m_transferFunction2 = transferFunction;
    precomputeAmbientColors2();
	for (int i=0;i<AMBIENT_COLORS_TABLE_SIZE2;i++)
	{
		DEBUG_LOG(m_ambientColors2[i].toString());
	}
}
void FusionVoxelShader::setMethod(int m)
{
	m_method=m;

}

HdrColor FusionVoxelShader::shade( int offset, const Vector3 &direction, const HdrColor &baseColor ) const
{
 	//cout << "entrashade" << endl;

    Q_CHECK_PTR( m_data );
	Q_CHECK_PTR( m_data2 );
	Q_UNUSED( direction );
    Q_UNUSED( baseColor );
 //	cout << "entra 1" << endl;
    HdrColor color1 = AmbientVoxelShader::shade( offset, direction, baseColor );
 	

	HdrColor color2 = m_ambientColors2[m_data2[offset]];

	HdrColor color = (color1 + color2) * 0.5;
 	
	
    return color;
}

HdrColor FusionVoxelShader::shade( const Vector3 &position, const Vector3 &direction, const TrilinearInterpolator *interpolator, const HdrColor &baseColor ) const
{
// 	cout << "entrashade" << endl;

    Q_UNUSED( direction );
    Q_UNUSED( baseColor );

    Q_CHECK_PTR( interpolator );
    Q_CHECK_PTR( m_data );
	Q_CHECK_PTR( m_data2 );
	//cout << "entra" << endl;
    int offsets[8];
    double weights[8];
    interpolator->getOffsetsAndWeights( position, offsets, weights );
// 	cout << "calculo value1" << endl;
    double value = TrilinearInterpolator::interpolate<double>( m_data, offsets, weights );

    //double value2 = TrilinearInterpolator::interpolate<double>( m_data2, offsets, weights );

	HdrColor color1 = m_ambientColors[static_cast<int>(value)];
// 	cout << "ok,trobo el color 2" << endl;
	HdrColor color2 = m_ambientColors2[static_cast<int>(value)];
// 	cout << "ok2" << endl;

	HdrColor color = (color1 + color2) * 0.5;
 	//cout << "fusiono" << endl;

    //return m_transferFunction.get( value );
    return color;
}


void FusionVoxelShader::precomputeAmbientColors2()
{
    for ( unsigned int i = 0; i < AMBIENT_COLORS_TABLE_SIZE2; i++ ) m_ambientColors2[i] = m_transferFunction2.get( i );
}

QString FusionVoxelShader::toString() const
{
    return "FusionVoxelShader";
}
}
