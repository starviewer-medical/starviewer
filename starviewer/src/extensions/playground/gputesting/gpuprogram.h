#ifndef UDGGPUPROGRAM_H
#define UDGGPUPROGRAM_H

#include "glew.h" //Ha d'estar abans que includes d'opengl
#include <GL/gl.h>

#include <QHash>
#include <QSet>


namespace udg {


/**
 * Representa un programa amb GLSL (els shaders que s'executen a la GPU).
 *
 * Un programa està format per 1 o més vertex shaders i 1 o més fragment shaders. Tenir més d'un shader d'un tipus determinat permet escriure el codi repartit en més d'un
 * fitxer.
 */
class GpuProgram {

public:

    GpuProgram();
    ~GpuProgram();

    /// Afegeix un vertex shader des de fitxer.
    void addVertexShaderFromFile( const QString &fileName );
    /// Afegeix un vertex shader passant directament el codi.
    void addVertexShader( const QString &code );
    /// Esborra tots els vertex shaders afegits.
    void clearVertexShaders();
    /// Afegeix un fragment shader des de fitxer.
    void addFragmentShaderFromFile( const QString &fileName );
    /// Afegeix un fragment shader passant directament el codi.
    void addFragmentShader( const QString &code );
    /// Esborra tots els vertex shaders afegits.
    void clearFragmentShaders();
    /// Enllaça els vertex shaders i els fragment shaders per crear el programa.
    void link();

    /// Retorna l'identificador de l'objecte del programa.
    GLhandleARB programObject() const;
    /// Retorna si el programa és vàlid o no. És vàlid si s'ha pogut enllaçar sense errors.
    bool isValid() const;
    /// Inicialitza un uniform (un tipus de variable dels shaders) perquè se li puguin assignar valors més tard.
    bool initUniform( const QString &name );
    /// Retorna l'identificador d'un uniform que hagi estat inicialitzat prèviament.
    GLint uniform( const QString &name ) const;

private:

    /// Afegeix un shader des de fitxer.
    GLhandleARB addShaderFromFile( const QString &fileName, GLenum type );
    /// Afegeix un shader passant directament el codi.
    GLhandleARB addShader( const QString &code, GLenum type );
    /// Si l'infolog no és buit, el printa i retorna cert; altrament no printa res i retorna fals.
    bool printInfoLog( GLhandleARB object ) const;

    /// Constructor còpia privat per evitar la còpia de l'objecte.
    GpuProgram( const GpuProgram &gpuProgram );
    /// Operador d'assiganció privat per evitar la còpia de l'objecte.
    GpuProgram& operator =( const GpuProgram &gpuProgram );

private:

    GLhandleARB m_programObject;
    bool m_valid;
    QSet<GLhandleARB> m_vertexShaders;
    QSet<GLhandleARB> m_fragmentShaders;
    QHash<QString, GLint> m_uniforms;

};


inline GLhandleARB GpuProgram::programObject() const
{
    return m_programObject;
}


inline GLint GpuProgram::uniform( const QString &name ) const
{
    return m_uniforms.value( name );
}


}


#endif
