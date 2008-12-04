// necessari pels shaders
#define GL_GLEXT_PROTOTYPES

#include "qgputestingviewer.h"

#include <QFile>
#include <QMessageBox>
#include <QTextStream>

#include "volume.h"


namespace udg {


QGpuTestingViewer::QGpuTestingViewer( QWidget *parent )
 : QGLWidget( parent ), m_volume( 0 ), m_volumeTexture( 0 ), m_framebufferObject( 0 ), m_framebufferTexture( 0 ), m_shaderProgram( 0 )
{
}


QGpuTestingViewer::~QGpuTestingViewer()
{
    glDeleteTextures( 1, &m_volumeTexture );
    glDeleteFramebuffersEXT( 1, &m_framebufferObject );
    glDeleteTextures( 1, &m_framebufferTexture );
    glDeleteObjectARB( m_shaderProgram );
}


// es crida abans de l'initializeGL
void QGpuTestingViewer::setVolume( Volume *volume )
{
    m_volume = volume;
}


void QGpuTestingViewer::initializeGL()
{
    // Comprovació de les extensions

    const QString requiredExtensions[] = { "GL_ARB_multitexture", "GL_ARB_fragment_shader", "GL_ARB_vertex_shader", "GL_ARB_shader_objects",
                                           "GL_ARB_shading_language_100", "GL_ARB_vertex_buffer_object", "GL_EXT_framebuffer_object" };
    QString nonSupportedExtensions;
    QString extensions( reinterpret_cast<const char*>( glGetString( GL_EXTENSIONS ) ) );

    for ( int i = 0; i < 7; i++ )
    {
        if ( extensions.contains( requiredExtensions[i] ) ) DEBUG_LOG( requiredExtensions[i] + " ok :)" );
        else
        {
            DEBUG_LOG( requiredExtensions[i]  + " ko :(" );
            nonSupportedExtensions += requiredExtensions[i] + "\n";
        }
    }

    if ( !nonSupportedExtensions.isNull() )
        QMessageBox::warning( this, tr("Non-supported extensions"),
                              tr("The GPU testing extension won't work as expected because your system doesn't support the following OpenGL extensions:") + "\n" + nonSupportedExtensions.trimmed() );

    // Color i profunditat inicials
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );

    createVolumeTexture();
    createFramebufferObject();
    loadShaders();

    glEnable( GL_CULL_FACE );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}


void QGpuTestingViewer::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    if ( height == 0 ) height = 1;

    /// \todo el zNear i el zFar haurien de ser ser en funció de la posició de la càmera, per agafar sempre la mida justa del volum
    gluPerspective( 90, static_cast<GLdouble>( width ) / static_cast<GLdouble>( height ), 1, 1000 );
}


void QGpuTestingViewer::paintGL()
{
    // Primer pintem les cares del darrere al framebuffer

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferObject );

    glPushAttrib( GL_VIEWPORT_BIT );                        // guardem el viewport actual
    glViewport( 0, 0, FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE ); // viewport a la mida de la textura del framebuffer

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluLookAt( 2, 2, 2, 0.5, 0.5, 0.5, 0.0, 1.0, 0.0 );

    glCullFace( GL_FRONT );

    drawCube();

    glPopAttrib();                                          // restauremm el viewport d'abans

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    // Després pintem les cares del davant amb els shaders

    glUseProgramObjectARB( m_shaderProgram );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluLookAt( 2, 2, 2, 0.5, 0.5, 0.5, 0.0, 1.0, 0.0 );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_framebufferTexture );
    glUniform1iARB( m_framebufferTextureUniform, 0 );

    glActiveTexture( GL_TEXTURE1 );
    glBindTexture( GL_TEXTURE_3D, m_volumeTexture );
    glUniform1iARB( m_volumeTextureUniform, 1 );

    glCullFace( GL_BACK );

    drawCube();

    glUseProgramObjectARB( 0 );
}


void QGpuTestingViewer::checkGLError( bool alert )
{
    GLenum error = glGetError();

    if ( error != GL_NO_ERROR )
    {
        QString errorString( reinterpret_cast<const char*>( gluErrorString( error ) ) );
        DEBUG_LOG( errorString );
        if ( alert ) QMessageBox::warning( this, tr("OpenGL error"), errorString );
    }
}


void QGpuTestingViewer::createVolumeTexture()
{
    // Primer passem el volum a reals i escalat entre 0 i 1

    vtkImageData *imageData = m_volume->getVtkData();
    double *range = imageData->GetScalarRange();
    float min = range[0], max = range[1];
    float shift = -min, scale = 1.0 / ( max - min );
    int size = imageData->GetNumberOfPoints();
    short *data = reinterpret_cast<short*>( imageData->GetScalarPointer() );
    float *floatData = new float[size];

    for ( int i = 0; i < size; i++ ) floatData[i] = ( data[i] + shift ) * scale;

    // Després creem la textura 3D

    int *dimensions = m_volume->getDimensions();

    glGenTextures( 1, &m_volumeTexture );
    glBindTexture( GL_TEXTURE_3D, m_volumeTexture );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_3D, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_BORDER );
    glTexImage3D( GL_TEXTURE_3D, 0, GL_INTENSITY16F_ARB, dimensions[0], dimensions[1], dimensions[2], 0, GL_RED, GL_FLOAT, floatData );
    checkGLError( true );

    delete[] floatData;
}


void QGpuTestingViewer::createFramebufferObject()
{
    glGenFramebuffersEXT( 1, &m_framebufferObject );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferObject );
    checkGLError();

    glGenTextures( 1, &m_framebufferTexture );
    glBindTexture( GL_TEXTURE_2D, m_framebufferTexture );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE, 0, GL_RGBA, GL_FLOAT, 0 );
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_framebufferTexture, 0 );
    checkGLError();

    GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
    if ( status == GL_FRAMEBUFFER_COMPLETE_EXT ) DEBUG_LOG( "framebuffer ok :D" );
    else DEBUG_LOG( "framebuffer ko :( " + QString::number( status, 16 ) );

    checkGLError();
}


void QGpuTestingViewer::loadShaders()
{
    GLhandleARB vertexShaderObject = glCreateShaderObjectARB( GL_VERTEX_SHADER_ARB );
    GLhandleARB fragmentShaderObject = glCreateShaderObjectARB( GL_FRAGMENT_SHADER_ARB );

    QByteArray vertexShaderSourceBytes, fragmentShaderSourceBytes;

    QFile vertexShaderSourceFile( ":/extensions/GpuTestingExtension/shaders/shader.vert" );

    if ( vertexShaderSourceFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        QTextStream vertexShaderSourceTextStream( &vertexShaderSourceFile );
        vertexShaderSourceBytes = vertexShaderSourceTextStream.readAll().toLocal8Bit();
        vertexShaderSourceFile.close();
    }
    else
    {
        DEBUG_LOG( "Error obrint el fitxer del vertex shader" );
        return;
    }

    QFile fragmentShaderSourceFile( ":/extensions/GpuTestingExtension/shaders/shader.frag" );

    if ( fragmentShaderSourceFile.open( QFile::ReadOnly | QFile::Text ) )
    {
        QTextStream fragmentShaderSourceTextStream( &fragmentShaderSourceFile );
        fragmentShaderSourceBytes = fragmentShaderSourceTextStream.readAll().toLocal8Bit();
        fragmentShaderSourceFile.close();
    }
    else
    {
        DEBUG_LOG( "Error obrint el fitxer del fragment shader" );
        return;
    }

    const char *vertexShaderSource = vertexShaderSourceBytes.constData();
    const char *fragmentShaderSource = fragmentShaderSourceBytes.constData();

    glShaderSourceARB( vertexShaderObject, 1, &vertexShaderSource, 0 );
    glShaderSourceARB( fragmentShaderObject, 1, &fragmentShaderSource, 0 );
    glCompileShaderARB( vertexShaderObject );
    glCompileShaderARB( fragmentShaderObject );

    m_shaderProgram = glCreateProgramObjectARB();
    glAttachObjectARB( m_shaderProgram, vertexShaderObject );
    glAttachObjectARB( m_shaderProgram, fragmentShaderObject );
    glLinkProgramARB( m_shaderProgram );

    const int MAX_ERROR_LENGTH = 512;
    int errorLength;
    char errors[MAX_ERROR_LENGTH];

    glGetInfoLogARB( vertexShaderObject, MAX_ERROR_LENGTH, &errorLength, errors );
    if ( errorLength > 0 ) DEBUG_LOG( errors );
    glGetInfoLogARB( fragmentShaderObject, MAX_ERROR_LENGTH, &errorLength, errors );
    if ( errorLength > 0 ) DEBUG_LOG( errors );
    glGetInfoLogARB( m_shaderProgram, MAX_ERROR_LENGTH, &errorLength, errors );
    if ( errorLength > 0 ) DEBUG_LOG( errors );

    m_framebufferTextureUniform = glGetUniformLocationARB( m_shaderProgram, "uFramebufferTexture" );
    if ( m_framebufferTextureUniform < 0 ) DEBUG_LOG( "Error en obtenir el framebuffer texture uniform" );
    m_volumeTextureUniform = glGetUniformLocationARB( m_shaderProgram, "uVolumeTexture" );
    if ( m_volumeTextureUniform < 0 ) DEBUG_LOG( "Error en obtenir el volume texture uniform" );

    checkGLError();
}


void QGpuTestingViewer::drawCube()
{
    glBegin( GL_QUADS );
    {
        glNormal3f( -1, 0, 0 );
        glColor3f( 0, 0, 0 ); glVertex3f( 0, 0, 0 );
        glColor3f( 0, 0, 1 ); glVertex3f( 0, 0, 1 );
        glColor3f( 0, 1, 1 ); glVertex3f( 0, 1, 1 );
        glColor3f( 0, 1, 0 ); glVertex3f( 0, 1, 0 );

        glNormal3f( 0, 0, -1 );
        glColor3f( 0, 0, 0 ); glVertex3f( 0, 0, 0 );
        glColor3f( 0, 1, 0 ); glVertex3f( 0, 1, 0 );
        glColor3f( 1, 1, 0 ); glVertex3f( 1, 1, 0 );
        glColor3f( 1, 0, 0 ); glVertex3f( 1, 0, 0 );

        glNormal3f( 0, -1, 0 );
        glColor3f( 0, 0, 0 ); glVertex3f( 0, 0, 0 );
        glColor3f( 1, 0, 0 ); glVertex3f( 1, 0, 0 );
        glColor3f( 1, 0, 1 ); glVertex3f( 1, 0, 1 );
        glColor3f( 0, 0, 1 ); glVertex3f( 0, 0, 1 );

        glNormal3f( 0, 0, 1 );
        glColor3f( 0, 0, 1 ); glVertex3f( 0, 0, 1 );
        glColor3f( 1, 0, 1 ); glVertex3f( 1, 0, 1 );
        glColor3f( 1, 1, 1 ); glVertex3f( 1, 1, 1 );
        glColor3f( 0, 1, 1 ); glVertex3f( 0, 1, 1 );

        glNormal3f( 0, 1, 0 );
        glColor3f( 0, 1, 0 ); glVertex3f( 0, 1, 0 );
        glColor3f( 0, 1, 1 ); glVertex3f( 0, 1, 1 );
        glColor3f( 1, 1, 1 ); glVertex3f( 1, 1, 1 );
        glColor3f( 1, 1, 0 ); glVertex3f( 1, 1, 0 );

        glNormal3f( 1, 0, 0 );
        glColor3f( 1, 0, 0 ); glVertex3f( 1, 0, 0 );
        glColor3f( 1, 1, 0 ); glVertex3f( 1, 1, 0 );
        glColor3f( 1, 1, 1 ); glVertex3f( 1, 1, 1 );
        glColor3f( 1, 0, 1 ); glVertex3f( 1, 0, 1 );
    }
    glEnd();
}


}
