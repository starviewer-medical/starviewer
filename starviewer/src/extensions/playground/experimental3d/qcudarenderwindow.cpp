#include "qcudarenderwindow.h"

#include "logging.h"


namespace udg {


QCudaRenderWindow::QCudaRenderWindow( QColor backgroundColor, int renderSize )
 : QGLWidget(), m_backgroundColor( backgroundColor ), m_renderSize( renderSize )
{
    makeCurrent();

    GLenum glew = glewInit();

    if ( glew != GLEW_OK )
    {
        DEBUG_LOG( "No s'ha pogut inicialitzar GLEW" );
        ERROR_LOG( "No s'ha pogut inicialitzar GLEW" );
    }

    // crear el pbo
    glGenBuffersARB( 1, &m_pixelBufferObject );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_pixelBufferObject );
    glBufferDataARB( GL_PIXEL_UNPACK_BUFFER_ARB, renderSize * renderSize * sizeof(uint), 0, GL_STREAM_DRAW_ARB );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

    // crear la textura
    glGenTextures( 1, &m_texture );
    glBindTexture( GL_TEXTURE_2D, m_texture );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_DECAL );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA8, renderSize, renderSize, 0, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
    glBindTexture( GL_TEXTURE_2D, 0 );
}


QCudaRenderWindow::~QCudaRenderWindow()
{
    glDeleteBuffersARB( 1, &m_pixelBufferObject );
    glDeleteTextures( 1, &m_texture );
}


GLuint QCudaRenderWindow::pixelBufferObject() const
{
    return m_pixelBufferObject;
}


void QCudaRenderWindow::initializeGL()
{
    qglClearColor( m_backgroundColor );
    glDisable( GL_DEPTH_TEST );
    glEnable( GL_TEXTURE_2D );
}


void QCudaRenderWindow::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );
}


void QCudaRenderWindow::paintGL()
{
    glBindTexture( GL_TEXTURE_2D, m_texture );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, m_pixelBufferObject );
    glTexSubImage2D( GL_TEXTURE_2D, 0, 0, 0, m_renderSize, m_renderSize, GL_RGBA, GL_UNSIGNED_BYTE, 0 );
    glBindBufferARB( GL_PIXEL_UNPACK_BUFFER_ARB, 0 );

    glClear( GL_COLOR_BUFFER_BIT );

    qglColor( m_backgroundColor );

    glBegin( GL_QUADS );
    {
        glTexCoord2f( 0.0f, 0.0f );
        glVertex2f( -1.0f, -1.0f );
        glTexCoord2f( 0.0f, 1.0f );
        glVertex2f( -1.0f, 1.0f );
        glTexCoord2f( 1.0f, 1.0f );
        glVertex2f( 1.0f, 1.0f );
        glTexCoord2f( 1.0f, 0.0f );
        glVertex2f( 1.0f, -1.0f );
    }
    glEnd();

    glBindTexture( GL_TEXTURE_2D, 0 );
}


}
