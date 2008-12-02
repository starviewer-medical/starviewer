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

//     glEnable( GL_CULL_FACE );
/*
    // tot això pot ser necessari pels shaders (falta comprovar-ho individualment)
    glEnable( GL_COLOR_MATERIAL );
    glColorMaterial( GL_FRONT_AND_BACK, GL_AMBIENT_AND_DIFFUSE );
//     glEnable( GL_CULL_FACE );
    glEnable( GL_DEPTH_TEST );
    glEnable( GL_LIGHTING );
    glShadeModel( GL_SMOOTH );
    glHint( GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST );*/

    /*
    loadShaders();
//     setupParallaxScene();
*/
    // Create the to FBO's one for the backside of the volumecube and one for the finalimage rendering
    /*glGenFramebuffersEXT( 1, &m_framebuffer );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebuffer );

    glGenTextures( 1, &m_backfaceBuffer );
    glBindTexture( GL_TEXTURE_2D, m_backfaceBuffer );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, 512, 512, 0, GL_RGBA, GL_FLOAT, 0 );
    glFramebufferTexture2DEXT( GL_FRAMEBUFFER_EXT, GL_COLOR_ATTACHMENT0_EXT, GL_TEXTURE_2D, m_backfaceBuffer, 0 );

    glGenTextures( 1, &m_finalImage );
    glBindTexture( GL_TEXTURE_2D, m_finalImage );
    glTexEnvi( GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_REPLACE );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
    glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
    glTexImage2D( GL_TEXTURE_2D, 0, GL_RGBA16F_ARB, 512, 512, 0, GL_RGBA, GL_FLOAT, 0 );

    glGenRenderbuffersEXT( 1, &m_renderbuffer );
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_renderbuffer );
    glRenderbufferStorageEXT( GL_RENDERBUFFER_EXT, GL_DEPTH_COMPONENT, 512, 512 );
    glFramebufferRenderbufferEXT( GL_FRAMEBUFFER_EXT, GL_DEPTH_ATTACHMENT_EXT, GL_RENDERBUFFER_EXT, m_renderbuffer );
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );

    GLenum status = glCheckFramebufferStatusEXT( GL_FRAMEBUFFER_EXT );
    if ( status == GL_FRAMEBUFFER_COMPLETE_EXT ) std::cout << "bé :D" << std::endl;
    else std::cout << "malament :( " << status << std::endl;*/
}


void QGpuTestingViewer::resizeGL( int width, int height )
{
    glViewport( 0, 0, width, height );

    glMatrixMode( GL_PROJECTION );
    glLoadIdentity();

    if ( height == 0 ) height = 1;

    gluPerspective( 90, static_cast<GLdouble>( width ) / static_cast<GLdouble>( height ), 1, 1000 );
}


void QGpuTestingViewer::paintGL()
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebufferObject );
    glPushAttrib( GL_VIEWPORT_BIT );
    glViewport( 0, 0, FRAMEBUFFER_SIZE, FRAMEBUFFER_SIZE );
    //resizeGL( 512, 512 );

    /////////////
    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    gluLookAt( 2, 2, 2, 0.5, 0.5, 0.5, 0.0, 1.0, 0.0 );

    // dibuixem només les cares del darrere
    glEnable( GL_CULL_FACE );
    glCullFace( GL_FRONT );
    drawCube();
    glDisable( GL_CULL_FACE );
    ////////////

    glPopAttrib();
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );



/////////////////////////////////////////////////////////////////////////////


///////////////////////////////////////////////////////////////////////////
/*
    glEnable( GL_TEXTURE_2D );
    glBindTexture( GL_TEXTURE_2D, m_framebufferTexture );
    //glTexParameteri( GL_TEXTURE_2D, GL_TEXTURE_COMPARE_MODE_ARB, GL_NONE );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glViewport( 0, 0, width(), height() );

    glMatrixMode( GL_PROJECTION );
    glPushMatrix();
    glLoadIdentity();

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();

    glBegin( GL_QUADS );
    {
        glTexCoord2f( 0.0f, 1.0f ); glVertex2f( -1.0f, 1.0f );
        glTexCoord2f( 0.0f, 0.0f ); glVertex2f( -1.0f, -1.0f );
        glTexCoord2f( 1.0f, 0.0f ); glVertex2f( 1.0f, -1.0f );
        glTexCoord2f( 1.0f, 1.0f ); glVertex2f( 1.0f, 1.0f );
    }
    glEnd();

    glMatrixMode( GL_PROJECTION );
    glPopMatrix();
    glMatrixMode( GL_MODELVIEW );

    glDisable( GL_TEXTURE_2D );
*/
//////////////////////////////////////////////////////////////////////////




    glUseProgramObjectARB( m_shaderProgram );

    glClear( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glMatrixMode( GL_MODELVIEW );
    glLoadIdentity();
    gluLookAt( 2, 2, 2, 0.5, 0.5, 0.5, 0.0, 1.0, 0.0 );

    glActiveTexture( GL_TEXTURE0 ); // 1?
    glBindTexture( GL_TEXTURE_2D, m_framebufferTexture );
    glUniform1iARB( m_texUniform, 0 );  // 1?

    glActiveTexture( GL_TEXTURE1 ); // 2?
    glBindTexture( GL_TEXTURE_3D, m_volumeTexture );
    glUniform1iARB( m_volumeTexture, 1 );  // 2?

    glEnable( GL_CULL_FACE );
    glCullFace( GL_BACK );

    drawCube();

    glDisable( GL_CULL_FACE );

    glUseProgramObjectARB( 0 );







//     enableRenderbuffers();
/*
    glLoadIdentity();
    glTranslatef( 0, 0, -2.25 );
    glRotatef( rotate, 0, 1, 1 );
    glTranslatef( -0.5, -0.5, -0.5 );   // center the texturecube

    renderBackface();
    //raycastingPass();

//     disableRenderbuffers();

    renderBufferToScreen();*/
/*
    glBegin( GL_TRIANGLE_STRIP );
    {
        glColor3f( 1.0f, 1.0f, 1.0f );
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( -100, 990, 24 );
        glVertex3f( -197, -100, -49 );
        glVertex3f( 100, -290, -99 );
        glVertex3f( 184, 139, 229 );
    }
    glEnd();*/

/*
    // shader on
    glUseProgramObjectARB( m_shaderProgram );

    glBegin( GL_TRIANGLE_STRIP );
    {
        glNormal3f( 0.0f, 0.0f, 1.0f );
        glVertex3f( -100, 990, 24 );
        glVertex3f( -197, -100, -49 );
        glVertex3f( 100, -290, -99 );
        glVertex3f( 184, 139, 229 );
    }
    glEnd();

    // shader off
    glUseProgramObjectARB( 0 );*/
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
    if ( !m_shaderProgram )
    {
//         QDir shaderDir( "/scratch/starviewer/src/extensions/playground/gputesting/shaders" );
//         QString shaderDirPath = shaderDir.path() + "/";

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
            std::cerr << "Error obrint el fitxer del vertex shader" << std::endl;
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
            std::cerr << "Error obrint el fitxer del fragment shader" << std::endl;
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

        const unsigned short ERROR_LENGTH = 512;
        char errors[ERROR_LENGTH];
        glGetInfoLogARB( vertexShaderObject, ERROR_LENGTH, 0, errors );
        std::cerr << errors << std::endl;
        glGetInfoLogARB( fragmentShaderObject, ERROR_LENGTH, 0, errors );
        std::cerr << errors << std::endl;
        glGetInfoLogARB( m_shaderProgram, ERROR_LENGTH, 0, errors );
        std::cerr << errors << std::endl;

        m_texUniform = glGetUniformLocationARB( m_shaderProgram, "tex" );
        m_volumeTexUniform = glGetUniformLocationARB( m_shaderProgram, "volume_tex" );
        m_stepSizeUniform = glGetUniformLocationARB( m_shaderProgram, "stepsize" );
    }
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


void QGpuTestingViewer::enableRenderbuffers()
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, m_framebuffer );
    glBindRenderbufferEXT( GL_RENDERBUFFER_EXT, m_renderbuffer );
}


void QGpuTestingViewer::disableRenderbuffers()
{
    glBindFramebufferEXT( GL_FRAMEBUFFER_EXT, 0 );
}


// this method is used to draw the front and backside of the volume
void QGpuTestingViewer::drawQuads( float x, float y, float z )
{
    glBegin( GL_QUADS );
    /* Back side */
    glNormal3f(0.0, 0.0, -1.0);
    vertex(0.0, 0.0, 0.0);
    vertex(0.0, y, 0.0);
    vertex(x, y, 0.0);
    vertex(x, 0.0, 0.0);

    /* Front side */
    glNormal3f(0.0, 0.0, 1.0);
    vertex(0.0, 0.0, z);
    vertex(x, 0.0, z);
    vertex(x, y, z);
    vertex(0.0, y, z);

    /* Top side */
    glNormal3f(0.0, 1.0, 0.0);
    vertex(0.0, y, 0.0);
    vertex(0.0, y, z);
    vertex(x, y, z);
    vertex(x, y, 0.0);

    /* Bottom side */
    glNormal3f(0.0, -1.0, 0.0);
    vertex(0.0, 0.0, 0.0);
    vertex(x, 0.0, 0.0);
    vertex(x, 0.0, z);
    vertex(0.0, 0.0, z);

    /* Left side */
    glNormal3f(-1.0, 0.0, 0.0);
    vertex(0.0, 0.0, 0.0);
    vertex(0.0, 0.0, z);
    vertex(0.0, y, z);
    vertex(0.0, y, 0.0);

    /* Right side */
    glNormal3f(1.0, 0.0, 0.0);
    vertex(x, 0.0, 0.0);
    vertex(x, y, 0.0);
    vertex(x, y, z);
    vertex(x, 0.0, z);
    glEnd();
}


void QGpuTestingViewer::vertex( float x, float y, float z )
{
    glColor3f(x,y,z);
    glMultiTexCoord3fARB(GL_TEXTURE1_ARB, x, y, z);
    glVertex3f(x,y,z);
}


}
