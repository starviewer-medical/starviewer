#ifndef UDGSHADER_H
#define UDGSHADER_H


#include <GL/glew.h>
#include <GL/gl.h>

#include <QHash>


namespace udg {


/**
 * Representa un shader d'OpenGL.
 */
class Shader {

public:

    Shader( const QString &vertexShaderSourceFileName, const QString &fragmentShaderSourceFileName );
    ~Shader();

    GLhandleARB programObject() const;
    bool isValid() const;
    bool initUniform( const QString &uniformName );
    GLint uniform( const QString &uniformName ) const;

private:

    GLhandleARB m_programObject;
    bool m_valid;
    QHash<QString, GLint> m_uniforms;

};


}


#endif
