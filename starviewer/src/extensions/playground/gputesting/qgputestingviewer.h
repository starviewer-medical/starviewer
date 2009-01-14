#ifndef UDGQGPUTESTINGVIEWER_H
#define UDGQGPUTESTINGVIEWER_H


#include <GL/glew.h>    // s'ha de posar aquí, abans del QGLWidget, que és el que inclou GL/gl.h
#include <QGLWidget>

#include "transferfunction.h"
#include "vector3.h"


namespace udg {


class Camera;
class GpuProgram;
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

    /// Retorna el color de fons.
    const QColor& backgroundColor() const;
    /// Assigna el color de fons.
    void setBackgroundColor( const QColor &backgroundColor );
    /// Assigna la distància que avança un raig a cada pas.
    void setRayStep( float rayStep );
    /// Assigna la funció de transferència.
    void setTransferFunction( const TransferFunction &transferFunction );

    /// Retorna els paràmetres de la càmera.
    void getCamera( Vector3 &position, Vector3 &focus, Vector3 &up ) const;
    /// Assigna els paràmetres de la càmera.
    void setCamera( const Vector3 &position, const Vector3 &focus, const Vector3 &up );

public slots:

    /// Assigna l'obertura de la càmera.
    void setFieldOfView( int fieldOfView );

protected:

    virtual void keyPressEvent( QKeyEvent *event );
    virtual void mousePressEvent( QMouseEvent *event );
    virtual void mouseMoveEvent( QMouseEvent *event );
    virtual void wheelEvent( QWheelEvent *event );

    /// Fa les inicialitzacions necessàries abans de començar a visualitzar amb OpenGL.
    virtual void initializeGL();
    /// Ajusta el viewport i la projecció quan canvia la mida del visualitzador.
    virtual void resizeGL( int width, int height );
    /// Fa la visualització amb OpenGL.
    virtual void paintGL();

private:

    /// Estructura de les dades al vertex buffer object
    struct VertexBufferObjectData
    {
        GLfloat nx, ny, nz; // normal
        GLfloat r, g, b;    // color
        GLfloat x, y, z;    // vertex
    };

    static const float KEYBOARD_CAMERA_INCREMENT;
    static const float MAX_CAMERA_DISTANCE_FACTOR;
    static const int TRANSFER_FUNCTION_TEXTURE_SIZE = 4096;

    /// Comprova si hi ha algun error d'OpenGL.
    void checkGLError( bool alert = false );

    /// Crea la càmera.
    void createCamera();
    /// Reinicia la càmera.
    void resetCamera();
    /// Crea el vertex buffer object amb els vèrtexs del volum.
    void createVertexBufferObject();
    /// Crea la textura 3D del volum.
    void createVolumeTexture();
    /// Crea el framebuffer object.
    void createFramebufferObject();
    /// Crea la textura del framebuffer.
    void createFramebufferTexture();
    /// Destrueix la textura anterior del framebuffer i en crea una de nova.
    void recreateFramebufferTexture();
    /// Carrega els shaders.
    void loadShaders();
    /// Crea la textura de la funció de transferència.
    void createTransferFunctionTexture();
    /// Actualitza la textura de la funció de transferència amb la funció actual.
    void updateTransferFunctionTexture();

    /// Ajusta la projecció segons la mida de la finestra i la posició de la càmera.
    void adjustProjection();
    /// Primera passada per dibuixar: pinta les cares del darrere al framebuffer.
    void firstPass();
    /// Segona passada per dibuixar: pinta les cares del davant mitjançant els shaders.
    void secondPass();
    /// Dibuixa el contingut del vertex buffer object.
    void drawVertexBufferObject();

private:

    /// Cert si es suporten totes les extensions necessàries.
    bool m_extensions;

    /// Volum a visualitzar.
    Volume *m_volume;
    /// Dimensions del volum multiplicades per l'espaiat.
    float m_dimX, m_dimY, m_dimZ;
    /// Mida de la dimensió multiplicada per l'espaiat més gran del volum.
    float m_biggestDimension;
    /// Longitud de la diagonal del volum segons les dimensions multiplicades per l'espaiat.
    float m_diagonalLength;

    /// La càmera.
    Camera *m_camera;
    /// Obertura de la càmera.
    float m_fieldOfView;
    /// Increments del zoom amb el teclat.
    float m_keyboardZoomIncrement;
    /// Escala que s'aplica a la rodeta per fer zoom.
    float m_wheelZoomScale;
    /// Última posició del ratolí.
    int m_lastX, m_lastY;

    /// Vertex buffer object per guardar els vèrtexs i atributs del cub.
    GLuint m_vertexBufferObject;

    /// Textura 3D del volum.
    GLuint m_volumeTexture;

    /// Framebuffer object on farem el render de la primera passada per pintar les cares del darrere.
    GLuint m_framebufferObject;
    /// Textura on es guarda el que es pinta al framebuffer object.
    GLuint m_framebufferTexture;

    /// Shader que s'aplicarà.
    GpuProgram *m_gpuProgram;

    /// Color de fons.
    QColor m_backgroundColor;
    /// Distància que avança un raig a cada pas.
    float m_rayStep;
    /// Funció de transferència.
    TransferFunction m_transferFunction;
    /// Textura de la funció de transferència.
    GLuint m_transferFunctionTexture;

};


}


#endif
