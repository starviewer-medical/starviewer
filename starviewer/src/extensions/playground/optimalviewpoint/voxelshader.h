#ifndef UDGVOXELSHADER_H
#define UDGVOXELSHADER_H


class QColor;


namespace udg {


/**
 * Aquesta classe implementa els mètodes per retornar el color d'un vòxel. El mètode shade ha de ser implementat per les classes filles.
 */
class VoxelShader {

public:

    VoxelShader();
    virtual ~VoxelShader();

    /// Assigna el volum de dades que es farà servir.
    void setData( unsigned char * data );

    /// Retorna el color corresponent al vòxel a la posició offset.
    virtual QColor shade( int offset ) const = 0;

protected:

    unsigned char * m_data;

};


}


#endif
