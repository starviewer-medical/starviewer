#ifndef UDGGPUPROGRAM_H
#define UDGGPUPROGRAM_H


#include <GL/glew.h>
#include <GL/gl.h>

#include <QHash>
#include <QSet>


namespace udg {


/**
 * Representa un shader d'OpenGL.
 *
 * \warning S'ha de fer servir des d'un punter sempre, perquè sinó es destruiria la referència al program object.
 */
class GpuProgram {

public:

    GpuProgram();
    ~GpuProgram();

    void addVertexShader( const QString &fileName );
    void clearVertexShaders();
    void addFragmentShader( const QString &fileName );
    void clearFragmentShaders();
    void link();

    GLhandleARB programObject() const;
    bool isValid() const;
    bool initUniform( const QString &uniformName );
    GLint uniform( const QString &uniformName ) const;

private:

    GLhandleARB addShader( const QString &fileName, GLenum type );
    /// Si l'infolog no és buit, el printa i retorna cert; altrament no printa res i retorna fals.
    bool printInfoLog( GLhandleARB object ) const;

    GLhandleARB m_programObject;
    bool m_valid;
    QSet<GLhandleARB> m_vertexShaders;
    QSet<GLhandleARB> m_fragmentShaders;
    QHash<QString, GLint> m_uniforms;

};


inline GLint GpuProgram::uniform( const QString &uniformName ) const
{
    return m_uniforms.value( uniformName );
}


}


#endif
