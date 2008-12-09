#ifndef UDGQGPUTESTINGVIEWER_H
#define UDGQGPUTESTINGVIEWER_H


#include <GL/glew.h>    // s'ha de posar aquí, abans del QGLWidget, que és el que inclou GL/gl.h
#include <QGLWidget>


namespace udg {


class Camera;
class Volume;


/**
 * Visualitzador amb GPU. Fa tota la feina necessària per visualitzar un volum donat amb la GPU.
 */
class QGpuTestingViewer : public QGLWidget {

    Q_OBJECT

public:

    QGpuTestingViewer( QWidget *parent = 0 );
    ~QGpuTestingViewer();

    /// Assigna el volum a visualitzar. Un cop assignat un volum no es pot canviar.
    void setVolume( Volume *volume );

protected:

    static const float KEYBOARD_CAMERA_INCREMENT;

    virtual void keyPressEvent( QKeyEvent *event );
    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseMoveEvent( QMouseEvent *event );

    /// Fa les inicialitzacions necessàries abans de començar a visualitzar amb OpenGL.
    virtual void initializeGL();
    /// Ajusta el viewport i la projecció quan canvia la mida del visualitzador.
    virtual void resizeGL( int width, int height );
    /// Fa la visualització amb OpenGL.
    virtual void paintGL();

private:

    static const GLsizei FRAMEBUFFER_SIZE = 2048;

    /// Comprova si hi ha algun error d'OpenGL.
    void checkGLError( bool alert = false );

    /// Crea la càmera.
    void createCamera();
    /// Reinicia la càmera.
    void resetCamera();
    /// Crea la textura 3D del volum.
    void createVolumeTexture();
    /// Crea el framebuffer object.
    void createFramebufferObject();
    /// Carrega els shaders.
    void loadShaders();

    /// Dibuixa un cub de mida mida 1.
    void drawCube();

    /// Cert si es suporten totes les extensions necessàries.
    bool m_extensions;

    /// Volum a visualitzar.
    Volume *m_volume;

    /// Textura 3D del volum.
    GLuint m_volumeTexture;

    /// Framebuffer object on farem el render de la primera passada per pintar les cares del darrere.
    GLuint m_framebufferObject;
    /// Textura on es guarda el que es pinta al framebuffer object.
    GLuint m_framebufferTexture;

    /// Combinació de shaders que s'aplicarà.
    GLhandleARB m_shaderProgram;
    /// Uniform per la textura del framebuffer.
    GLint m_framebufferTextureUniform;
    /// Uniform per la textura del volum.
    GLint m_volumeTextureUniform;

    Camera *m_camera;

    int m_lastX, m_lastY;

};


}


#endif
