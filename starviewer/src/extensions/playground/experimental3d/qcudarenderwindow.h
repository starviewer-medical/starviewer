#ifndef QCUDARENDERWINDOW_H
#define QCUDARENDERWINDOW_H

#include "glew.h" //Ha d'estar abans que includes d'opengl

#include <QGLWidget>

namespace udg {


class QCudaRenderWindow : public QGLWidget {

    Q_OBJECT

public:

    /// Crea una finestra que renderitza amb el color de fons \a backgroundColor i amb un pixel buffer object de mida \a renderSize * \a renderSize.
    QCudaRenderWindow( QColor backgroundColor, int renderSize );
    virtual ~QCudaRenderWindow();

    /// Retorna el pixel buffer object.
    GLuint pixelBufferObject() const;

protected:

    virtual void initializeGL();
    virtual void resizeGL( int width, int height );
    virtual void paintGL();

private:

    QColor m_backgroundColor;
    int m_renderSize;
    GLuint m_pixelBufferObject;
    GLuint m_texture;

};


} // namespace udg


#endif // QCUDARENDERWINDOW_H
