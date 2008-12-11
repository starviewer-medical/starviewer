#include "qgputestingviewer.h"

#include <QFile>
#include <QKeyEvent>
#include <QMessageBox>
#include <QMouseEvent>
#include <QTextStream>
#include <QWheelEvent>

#include "camera.h"
#include "volume.h"


namespace udg {


const float QGpuTestingViewer::KEYBOARD_CAMERA_INCREMENT = 10.0f;
const float QGpuTestingViewer::MAX_CAMERA_DISTANCE_FACTOR = 1000.0f;


QGpuTestingViewer::QGpuTestingViewer( QWidget *parent )
 : QGLWidget( parent ), m_extensions( false ), m_volume( 0 ), m_camera( 0 ), m_vertexBufferObject( 0 ), m_volumeTexture( 0 ),
   m_framebufferObject( 0 ), m_framebufferTexture( 0 ), m_shaderProgram( 0 )
{
    setFocusPolicy( Qt::WheelFocus );
}


QGpuTestingViewer::~QGpuTestingViewer()
{
    if ( m_extensions )
    {
        delete m_camera;
        glDeleteBuffersARB( 1, &m_vertexBufferObject );
        glDeleteTextures( 1, &m_volumeTexture );
        glDeleteFramebuffersEXT( 1, &m_framebufferObject );
        glDeleteTextures( 1, &m_framebufferTexture );
        glDeleteObjectARB( m_shaderProgram );
    }
}


// es crida abans de l'initializeGL
void QGpuTestingViewer::setVolume( Volume *volume )
{
    m_volume = volume;

    int *dimensions = volume->getDimensions();
    double *spacing = volume->getSpacing();
    m_dimX = dimensions[0] * spacing[0]; m_dimY = dimensions[1] * spacing[1]; m_dimZ = dimensions[2] * spacing[2];
    m_biggestDimension = qMax( qMax( m_dimX, m_dimY ), m_dimZ );
    m_diagonalLength = Vector3( m_dimX, m_dimY, m_dimZ ).length();

    m_keyboardZoomIncrement = 0.1f * m_diagonalLength;
    m_wheelZoomScale = m_diagonalLength / 720.f;
}


void QGpuTestingViewer::keyPressEvent( QKeyEvent *event )
{
    switch ( event->key() )
    {
        case Qt::Key_Left: m_camera->rotateSmoothly( KEYBOARD_CAMERA_INCREMENT, 0.0f, 0.0f ); break;
        case Qt::Key_Right: m_camera->rotateSmoothly( -KEYBOARD_CAMERA_INCREMENT, 0.0f, 0.0f ); break;
        case Qt::Key_Up: m_camera->rotateSmoothly( 0.0f, KEYBOARD_CAMERA_INCREMENT, 0.0f ); break;
        case Qt::Key_Down: m_camera->rotateSmoothly( 0.0f, -KEYBOARD_CAMERA_INCREMENT, 0.0f ); break;
        case Qt::Key_Plus: m_camera->zoom( -m_keyboardZoomIncrement, m_biggestDimension, m_biggestDimension * MAX_CAMERA_DISTANCE_FACTOR ); break;
        case Qt::Key_Minus: m_camera->zoom( m_keyboardZoomIncrement, m_biggestDimension, m_biggestDimension * MAX_CAMERA_DISTANCE_FACTOR ); break;
        case Qt::Key_R: resetCamera(); break;
        default: QWidget::keyPressEvent( event ); return;
    }

    updateGL();
}


void QGpuTestingViewer::mousePressEvent( QMouseEvent *event )
{
    m_lastX = event->x(); m_lastY = event->y();
}


void QGpuTestingViewer::mouseMoveEvent( QMouseEvent *event )
{
    int dx = event->x() - m_lastX;
    int dy = event->y() - m_lastY;

    m_camera->rotateSmoothly( -dx, -dy, 0.0f );

    m_lastX = event->x(); m_lastY = event->y();

    updateGL();
}


void QGpuTestingViewer::wheelEvent( QWheelEvent *event )
{
    m_camera->zoom( -m_wheelZoomScale * event->delta(), m_biggestDimension, m_biggestDimension * MAX_CAMERA_DISTANCE_FACTOR );
    updateGL();
}


void QGpuTestingViewer::initializeGL()
{
    GLenum glew = glewInit();

    if ( glew == GLEW_OK )
    {
        // Comprovació de les extensions

        QString nonSupportedExtensions;

        if ( !GLEW_ARB_multitexture ) nonSupportedExtensions += "GL_ARB_multitexture\n";
        if ( !GLEW_ARB_texture_non_power_of_two ) nonSupportedExtensions += "GL_ARB_texture_non_power_of_two\n";
        if ( !GLEW_ARB_vertex_shader ) nonSupportedExtensions += "GL_ARB_vertex_shader\n";
        if ( !GLEW_ARB_fragment_shader ) nonSupportedExtensions += "GL_ARB_fragment_shader\n";
        if ( !GLEW_ARB_shader_objects ) nonSupportedExtensions += "GL_ARB_shader_objects\n";
        if ( !GLEW_ARB_shading_language_100 ) nonSupportedExtensions += "GL_ARB_shading_language_100\n";
        if ( !GLEW_ARB_vertex_buffer_object ) nonSupportedExtensions += "GL_ARB_vertex_buffer_object\n";
        if ( !GLEW_EXT_framebuffer_object ) nonSupportedExtensions += "GL_EXT_framebuffer_object\n";

        if ( nonSupportedExtensions.isNull() ) m_extensions = true;
        else
        {
            DEBUG_LOG( "Extensions no suportades:\n" + nonSupportedExtensions.trimmed() );
            QMessageBox::warning( this, tr("Non-supported extensions"),
                                  tr("The GPU testing extension won't work as expected because your system doesn't support the following OpenGL extensions:") + "\n" + nonSupportedExtensions.trimmed() );
        }
    }
    else DEBUG_LOG( QString( "Ha fallat el glewInit() amb l'error: %1" ).arg( reinterpret_cast<const char*>( glewGetErrorString( glew ) ) ) );

    // Color i profunditat inicials
    glClearColor( 0.0, 0.0, 0.0, 0.0 );
    glClearDepth( 1.0 );

    if ( m_extensions )
    {
        createCamera();
        createVertexBufferObject();
        createVolumeTexture();
        createFramebufferObject();
        loadShaders();
    }

    glEnable( GL_CULL_FACE );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );
}


void QGpuTestingViewer::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );

    if ( !m_extensions ) return;

    recreateFramebufferTexture();
    adjustProjection();
}


void QGpuTestingViewer::paintGL()
{
    if ( !m_extensions )
    {
        glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );
        return;
    }

    adjustProjection();

    // Primer pintem les cares del darrere al framebuffer

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferObject );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glMultMatrixf( &( m_camera->getViewMatrix()[0][0] ) );

    glCullFace( GL_FRONT );

    drawCube();

    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    // Després pintem les cares del davant amb els shaders

    glUseProgramObjectARB( m_shaderProgram );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    glMultMatrixf( &( m_camera->getViewMatrix()[0][0] ) );

    glActiveTexture( GL_TEXTURE0 );
    glBindTexture( GL_TEXTURE_2D, m_framebufferTexture );
    glUniform1iARB( m_framebufferTextureUniform, 0 );

    glUniform3fARB( m_dimensionsUniform, m_dimX, m_dimY, m_dimZ );

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


void QGpuTestingViewer::createCamera()
{
    m_camera = new Camera();
    resetCamera();
}


void QGpuTestingViewer::resetCamera()
{
    const Vector3 EYE( 0.0, 0.0, 2.0 * m_biggestDimension );

    m_camera->setBehavior( Camera::CAMERA_BEHAVIOR_ORBIT );
    m_camera->setPreferTargetYAxisOrbiting( false );
    m_camera->setOrbitOffsetDistance( EYE.length() );   // ha de ser la distància inicial entre la càmera i l'objectiu

    m_camera->lookAt( EYE, Vector3(), Vector3( 0.0, 1.0, 0.0 ) );   // posició inicial
}


void QGpuTestingViewer::createVertexBufferObject()
{
    GLfloat x = m_dimX / 2.0f, y = m_dimY / 2.0f, z = m_dimZ / 2.0f;

    VertexBufferObjectData data[24] =   // 4 vèrtexs/cara * 6 cares
    {   //  nx     ny     nz   |   r      g      b    |  x   y   z
        { -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,  0.0f,   -x, -y, -z },
        { -1.0f,  0.0f,  0.0f,    0.0f,  0.0f,  1.0f,   -x, -y, +z },
        { -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,  1.0f,   -x, +y, +z },
        { -1.0f,  0.0f,  0.0f,    0.0f,  1.0f,  0.0f,   -x, +y, -z },

        {  0.0f,  0.0f, -1.0f,    0.0f,  0.0f,  0.0f,   -x, -y, -z },
        {  0.0f,  0.0f, -1.0f,    0.0f,  1.0f,  0.0f,   -x, +y, -z },
        {  0.0f,  0.0f, -1.0f,    1.0f,  1.0f,  0.0f,   +x, +y, -z },
        {  0.0f,  0.0f, -1.0f,    1.0f,  0.0f,  0.0f,   +x, -y, -z },

        {  0.0f, -1.0f,  0.0f,    0.0f,  0.0f,  0.0f,   -x, -y, -z },
        {  0.0f, -1.0f,  0.0f,    1.0f,  0.0f,  0.0f,   +x, -y, -z },
        {  0.0f, -1.0f,  0.0f,    1.0f,  0.0f,  1.0f,   +x, -y, +z },
        {  0.0f, -1.0f,  0.0f,    0.0f,  0.0f,  1.0f,   -x, -y, +z },

        {  0.0f,  0.0f,  1.0f,    0.0f,  0.0f,  1.0f,   -x, -y, +z },
        {  0.0f,  0.0f,  1.0f,    1.0f,  0.0f,  1.0f,   +x, -y, +z },
        {  0.0f,  0.0f,  1.0f,    1.0f,  1.0f,  1.0f,   +x, +y, +z },
        {  0.0f,  0.0f,  1.0f,    0.0f,  1.0f,  1.0f,   -x, +y, +z },

        {  0.0f,  1.0f,  0.0f,    0.0f,  1.0f,  0.0f,   -x, +y, -z },
        {  0.0f,  1.0f,  0.0f,    0.0f,  1.0f,  1.0f,   -x, +y, +z },
        {  0.0f,  1.0f,  0.0f,    1.0f,  1.0f,  1.0f,   +x, +y, +z },
        {  0.0f,  1.0f,  0.0f,    1.0f,  1.0f,  0.0f,   +x, +y, -z },

        {  1.0f,  0.0f,  0.0f,    1.0f,  0.0f,  0.0f,   +x, -y, -z },
        {  1.0f,  0.0f,  0.0f,    1.0f,  1.0f,  0.0f,   +x, +y, -z },
        {  1.0f,  0.0f,  0.0f,    1.0f,  1.0f,  1.0f,   +x, +y, +z },
        {  1.0f,  0.0f,  0.0f,    1.0f,  0.0f,  1.0f,   +x, -y, +z }
    };

    glGenBuffersARB( 1, &m_vertexBufferObject );
    glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_vertexBufferObject );
    glBufferDataARB( GL_ARRAY_BUFFER_ARB, 24 * sizeof( VertexBufferObjectData ), data, GL_STATIC_DRAW_ARB );

    checkGLError();
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
    checkGLError();

    createFramebufferTexture();
}


void QGpuTestingViewer::createFramebufferTexture()
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferObject );

    glGenTextures( 1, &m_framebufferTexture );
    glBindTexture( GL_TEXTURE_2D, m_framebufferTexture );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, width(), height(), 0, GL_RGBA, GL_FLOAT, 0 );
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_framebufferTexture, 0 );
    checkGLError();

    GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
    if ( status == GL_FRAMEBUFFER_COMPLETE_EXT ) DEBUG_LOG( "framebuffer ok :D" );
    else DEBUG_LOG( "framebuffer ko :( " + QString::number( status, 16 ) );

    checkGLError();
}


void QGpuTestingViewer::recreateFramebufferTexture()
{
    glDeleteTextures( 1, &m_framebufferTexture );
    createFramebufferTexture();
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
    //QFile fragmentShaderSourceFile( "/scratch/starviewer/src/extensions/playground/gputesting/shaders/shader.frag" );

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

    m_dimensionsUniform = glGetUniformLocationARB( m_shaderProgram, "uDimensions" );
    if ( m_dimensionsUniform < 0 ) DEBUG_LOG( "Error en obtenir el dimensions uniform" );
    m_framebufferTextureUniform = glGetUniformLocationARB( m_shaderProgram, "uFramebufferTexture" );
    if ( m_framebufferTextureUniform < 0 ) DEBUG_LOG( "Error en obtenir el framebuffer texture uniform" );
    m_volumeTextureUniform = glGetUniformLocationARB( m_shaderProgram, "uVolumeTexture" );
    if ( m_volumeTextureUniform < 0 ) DEBUG_LOG( "Error en obtenir el volume texture uniform" );

    checkGLError();
}


void QGpuTestingViewer::adjustProjection()
{
    int width = this->width(), height = this->height();

    if ( height == 0 ) height = 1;

    float cameraDistance = m_camera->getPosition().length();
    float zNear = cameraDistance - m_diagonalLength / 2.0f;
    float zFar = cameraDistance + m_diagonalLength / 2.0f;

    m_camera->perspective( 90.0f, static_cast<float>( width ) / static_cast<float>( height ), zNear, zFar );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();
    glMultMatrixf( &( m_camera->getProjectionMatrix()[0][0] ) );
}


void QGpuTestingViewer::drawCube()
{
    glEnable( GL_NORMAL_ARRAY );
    glEnable( GL_COLOR_ARRAY );
    glEnable( GL_VERTEX_ARRAY );

    glBindBufferARB( GL_ARRAY_BUFFER_ARB, m_vertexBufferObject );

    glNormalPointer( GL_FLOAT, sizeof( VertexBufferObjectData ), 0 );
    glColorPointer( 3, GL_FLOAT, sizeof( VertexBufferObjectData ), reinterpret_cast<const GLvoid*>( 3 * sizeof( GLfloat ) ) );
    glVertexPointer( 3, GL_FLOAT, sizeof( VertexBufferObjectData ), reinterpret_cast<const GLvoid*>( 6 * sizeof( GLfloat ) ) );

    glDrawArrays( GL_QUADS, 0, 24 );    // 24 = 4 vèrtexs/cara * 6 cares

    glDisable( GL_NORMAL_ARRAY );
    glDisable( GL_COLOR_ARRAY );
    glDisable( GL_VERTEX_ARRAY );
}


}
