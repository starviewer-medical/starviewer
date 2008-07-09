#ifndef UDGVOXELSHADER_H
#define UDGVOXELSHADER_H


#include <QColor>


namespace udg {


/**
 * Aquesta classe implementa els mètodes per retornar el color d'un vòxel. El mètode shade ha de ser implementat per les classes filles.
 */
class VoxelShader {

public:

    VoxelShader();
    virtual ~VoxelShader();

    /// Assigna el volum de dades.
    void setData( const unsigned char *data );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset, const QColor &baseColor = Qt::white ) const = 0;

protected:

    const unsigned char *m_data;

};


}


#endif
