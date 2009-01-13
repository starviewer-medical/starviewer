#include "shader.h"

#include <QFile>
#include <QTextStream>

#include "logging.h"


namespace udg {


Shader::Shader()
 : m_valid( false )
{
    m_programObject = glCreateProgramObjectARB();
}


Shader::~Shader()
{
    glDeleteObjectARB( m_programObject );
}


void Shader::addVertexShader( const QString &fileName )
{
    GLhandleARB shader = addShader( fileName, GL_VERTEX_SHADER_ARB );
    if ( shader > 0 ) m_vertexShaders << shader;
}


void Shader::clearVertexShaders()
{
    foreach ( GLhandleARB vertexShader, m_vertexShaders ) glDetachObjectARB( m_programObject, vertexShader );

    m_vertexShaders.clear();
}


void Shader::addFragmentShader( const QString &fileName )
{
    GLhandleARB shader = addShader( fileName, GL_FRAGMENT_SHADER_ARB );
    if ( shader > 0 ) m_fragmentShaders << shader;
}


void Shader::clearFragmentShaders()
{
    foreach ( GLhandleARB fragmentShader, m_fragmentShaders ) glDetachObjectARB( m_programObject, fragmentShader );

    m_fragmentShaders.clear();
}


void Shader::link()
{
    glLinkProgramARB( m_programObject );
    m_valid = !printInfoLog( m_programObject );
}


GLhandleARB Shader::programObject() const
{
    return m_programObject;
}


bool Shader::isValid() const
{
    return m_valid;
}


bool Shader::initUniform( const QString &uniformName )
{
    if ( !m_valid ) return false;
    if ( m_uniforms.contains( uniformName ) ) return false;

    GLint uniform = glGetUniformLocationARB( m_programObject, qPrintable( uniformName ) );

    if ( uniform < 0 ) return false;

    m_uniforms[uniformName] = uniform;

    return true;
}


GLhandleARB Shader::addShader( const QString &fileName, GLenum type )
{
    QFile shaderSourceFile( fileName );

    if ( !shaderSourceFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( QString( "Error obrint el fitxer del shader: %1" ).arg( fileName ) );
        return 0;
    }

    QTextStream shaderSourceTextStream( &shaderSourceFile );
    const char *shaderSource = qPrintable( shaderSourceTextStream.readAll() );

    GLhandleARB shaderObject = glCreateShaderObjectARB( type );
    glShaderSourceARB( shaderObject, 1, &shaderSource, 0 );
    glCompileShaderARB( shaderObject );
    glAttachObjectARB( m_programObject, shaderObject );
    glDeleteObjectARB( shaderObject );

    shaderSourceFile.close();

    printInfoLog( shaderObject );
    printInfoLog( m_programObject );

    return shaderObject;
}


bool Shader::printInfoLog( GLhandleARB object ) const
{
    int infoLogLength;

    glGetObjectParameterivARB( object, GL_OBJECT_INFO_LOG_LENGTH_ARB, &infoLogLength);  // infoLogLength inclou el \0 final

    if ( infoLogLength > 0 )
    {
        char *infoLog = new char[infoLogLength];
        int length;
        glGetInfoLogARB( object, infoLogLength, &length, infoLog ); // length és la llargada del missatge net
        if ( length > 0 ) DEBUG_LOG( infoLog );
        delete[] infoLog;
        return length > 0;
    }
    else return false;
}


}
