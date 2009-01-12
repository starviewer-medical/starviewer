#include "shader.h"

#include <QByteArray>
#include <QFile>
#include <QTextStream>

#include "logging.h"


namespace udg {


Shader::Shader( const QString &vertexShaderSourceFileName, const QString &fragmentShaderSourceFileName )
 : m_programObject( 0 ), m_valid( true )
{
    GLhandleARB vertexShaderObject = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
    GLhandleARB fragmentShaderObject = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

    QByteArray vertexShaderSourceBytes, fragmentShaderSourceBytes;

    QFile vertexShaderSourceFile( vertexShaderSourceFileName );

    if ( vertexShaderSourceFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        QTextStream vertexShaderSourceTextStream( &vertexShaderSourceFile );
        vertexShaderSourceBytes = vertexShaderSourceTextStream.readAll().toLocal8Bit();
        vertexShaderSourceFile.close();
    }
    else
    {
        DEBUG_LOG( QString( "Error obrint el fitxer del vertex shader: %1" ).arg( vertexShaderSourceFileName ) );
        m_valid = false;
        return;
    }

    QFile fragmentShaderSourceFile( fragmentShaderSourceFileName );

    if ( fragmentShaderSourceFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        QTextStream fragmentShaderSourceTextStream( &fragmentShaderSourceFile );
        fragmentShaderSourceBytes = fragmentShaderSourceTextStream.readAll().toLocal8Bit();
        fragmentShaderSourceFile.close();
    }
    else
    {
        DEBUG_LOG( QString( "Error obrint el fitxer del fragment shader: %1" ).arg( fragmentShaderSourceFileName ) );
        m_valid = false;
        return;
    }

    const char *vertexShaderSource = vertexShaderSourceBytes.constData();
    const char *fragmentShaderSource = fragmentShaderSourceBytes.constData();

    glShaderSourceARB( vertexShaderObject, 1, &vertexShaderSource, 0 );
    glShaderSourceARB( fragmentShaderObject, 1, &fragmentShaderSource, 0 );
    glCompileShaderARB( vertexShaderObject );
    glCompileShaderARB( fragmentShaderObject );

    m_programObject = glCreateProgramObjectARB();
    glAttachObjectARB( m_programObject, vertexShaderObject );
    glAttachObjectARB( m_programObject, fragmentShaderObject );
    glLinkProgramARB( m_programObject );

    const int MAX_ERROR_LENGTH = 512;
    int errorLength;
    char errors[MAX_ERROR_LENGTH];

    glGetInfoLogARB( vertexShaderObject, MAX_ERROR_LENGTH, &errorLength, errors );
    if ( errorLength > 0 )
    {
        DEBUG_LOG( errors );
        m_valid = false;
    }

    glGetInfoLogARB( fragmentShaderObject, MAX_ERROR_LENGTH, &errorLength, errors );
    if ( errorLength > 0 )
    {
        DEBUG_LOG( errors );
        m_valid = false;
    }

    glGetInfoLogARB( m_programObject, MAX_ERROR_LENGTH, &errorLength, errors );
    if ( errorLength > 0 )
    {
        DEBUG_LOG( errors );
        m_valid = false;
    }
}


Shader::~Shader()
{
    glDeleteObjectARB( m_programObject );
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


GLint Shader::uniform( const QString &uniformName ) const
{
    if ( !m_valid || !m_uniforms.contains( uniformName ) ) return -1;

    return m_uniforms.value( uniformName );
}


}
