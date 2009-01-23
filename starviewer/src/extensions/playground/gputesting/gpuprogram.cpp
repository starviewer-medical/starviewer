#include "gpuprogram.h"

#include <QByteArray>
#include <QFile>
#include <QTextStream>

#include "logging.h"


namespace udg {


GpuProgram::GpuProgram()
 : m_valid( false )
{
    m_programObject = glCreateProgramObjectARB();
}


GpuProgram::~GpuProgram()
{
    glDeleteObjectARB( m_programObject );
}


void GpuProgram::addVertexShaderFromFile( const QString &fileName )
{
    GLhandleARB shader = addShaderFromFile( fileName, GL_VERTEX_SHADER_ARB );
    if ( shader > 0 ) m_vertexShaders << shader;
}


void GpuProgram::addVertexShader( const QString &code )
{
    GLhandleARB shader = addShader( code, GL_VERTEX_SHADER_ARB );
    if ( shader > 0 ) m_vertexShaders << shader;
}


void GpuProgram::clearVertexShaders()
{
    foreach ( GLhandleARB vertexShader, m_vertexShaders ) glDetachObjectARB( m_programObject, vertexShader );

    m_vertexShaders.clear();
}


void GpuProgram::addFragmentShaderFromFile( const QString &fileName )
{
    GLhandleARB shader = addShaderFromFile( fileName, GL_FRAGMENT_SHADER_ARB );
    if ( shader > 0 ) m_fragmentShaders << shader;
}


void GpuProgram::addFragmentShader( const QString &code )
{
    GLhandleARB shader = addShader( code, GL_FRAGMENT_SHADER_ARB );
    if ( shader > 0 ) m_fragmentShaders << shader;
}


void GpuProgram::clearFragmentShaders()
{
    foreach ( GLhandleARB fragmentShader, m_fragmentShaders ) glDetachObjectARB( m_programObject, fragmentShader );

    m_fragmentShaders.clear();
}


void GpuProgram::link()
{
    glLinkProgramARB( m_programObject );
    m_valid = !printInfoLog( m_programObject );
}


bool GpuProgram::isValid() const
{
    return m_valid;
}


bool GpuProgram::initUniform( const QString &name )
{
    if ( !m_valid ) return false;
    if ( m_uniforms.contains( name ) ) return false;

    GLint uniform = glGetUniformLocationARB( m_programObject, qPrintable( name ) );

    if ( uniform < 0 ) return false;

    m_uniforms[name] = uniform;

    return true;
}


GLhandleARB GpuProgram::addShaderFromFile( const QString &fileName, GLenum type )
{
    QFile shaderSourceFile( fileName );

    if ( !shaderSourceFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        DEBUG_LOG( QString( "Error obrint el fitxer del shader: %1" ).arg( fileName ) );
        return 0;
    }

    QTextStream shaderSourceTextStream( &shaderSourceFile );
    QString code = shaderSourceTextStream.readAll();
    GLhandleARB shaderObject = addShader( code, type );
    shaderSourceFile.close();

    return shaderObject;
}


GLhandleARB GpuProgram::addShader( const QString &code, GLenum type )
{
    DEBUG_LOG( "addShader()" );

    QByteArray byteArray = code.toLocal8Bit();
    const char *shaderSource = byteArray.constData();

    GLhandleARB shaderObject = glCreateShaderObjectARB( type );
    glShaderSourceARB( shaderObject, 1, &shaderSource, 0 );
    glCompileShaderARB( shaderObject );
    glAttachObjectARB( m_programObject, shaderObject );
    glDeleteObjectARB( shaderObject );

    DEBUG_LOG( "info shader" );
    printInfoLog( shaderObject );
    DEBUG_LOG( "info program" );
    printInfoLog( m_programObject );

    return shaderObject;
}


bool GpuProgram::printInfoLog( GLhandleARB object ) const
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
