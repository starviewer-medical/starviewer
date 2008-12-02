#ifndef UDGQGPUTESTINGVIEWER_H
#define UDGQGPUTESTINGVIEWER_H


#include <QGLWidget>


namespace udg {


class Volume;


class QGpuTestingViewer : public QGLWidget {

    Q_OBJECT

public:

    QGpuTestingViewer( QWidget *parent = 0 );
    ~QGpuTestingViewer();

    void setVolume( Volume *volume );

protected:

    virtual void initializeGL();
    virtual void resizeGL( int width, int height );
    virtual void paintGL();

private:

    void createVolumeTexture();
    void createFramebufferObject();
    void loadShaders();
    void drawCube();



    void enableRenderbuffers();
    void disableRenderbuffers();
    void renderBackface();
    // this method is used to draw the front and backside of the volume
    void drawQuads( float x, float y, float z );
    void vertex( float x, float y, float z );
    void raycastingPass();
    void renderBufferToScreen();





    Volume *m_volume;

    GLuint m_volumeTexture;

    GLhandleARB m_shaderProgram;
    GLint m_texUniform;
    GLint m_volumeTexUniform;
    GLint m_stepSizeUniform;

    GLuint m_framebufferObject;
    GLuint m_backfaceBuffer;

    GLuint m_framebuffer;
    GLuint m_finalImage;
    GLuint m_renderbuffer;

};


}


#endif
