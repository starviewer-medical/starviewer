/***************************************************************************
 *   Copyright (C) 2006-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.edu/GGG/index.html                                    *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/


#ifndef UDGOPTIMALVIEWPOINTVIEWER_H
#define UDGOPTIMALVIEWPOINTVIEWER_H


#include "qviewer.h"


namespace udg {


/**
 * Aquesta classe fa la visualització de l'extensió Optimal Viewpoint.
 *
 * Només s'encarrega de guardar el QVTKWidget que fa la visualització. La gestió
 * dels volums s'ha d'implementar externament, afegint-los i traient-los del
 * renderer que es pot obtenir d'aquesta classe.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class OptimalViewpointViewer : public QViewer {

    Q_OBJECT

public:

    OptimalViewpointViewer( QWidget * parent = 0 );
    virtual ~OptimalViewpointViewer();

    /// Retorna el renderer.
    virtual vtkRenderer * getRenderer();

    /// Indiquem les dades d'entrada.
    virtual void setInput( Volume * volume );

    /// Posa el color de fons a \a color.
    void setBackgroundColor( QColor color );
    /// Retorna el color de fons actual.
    QColor getBackgroundColor();

    void getCurrentWindowLevel( double wl[2] );
    void resetView( CameraOrientationType view );

public slots:

    /// Força l'execució de la visualització.
    virtual void render();

    /// Activa o desactiva que el manager escolti els events per processar tools.
    virtual void setEnableTools( bool enable );
    virtual void enableTools();
    virtual void disableTools();

    /// Interroga al tool manager per la tool demanada. Segons si aquesta tool
    /// està disponible o no el viewer farà el que calgui. Reimplementat en cada
    /// viewer.
    virtual void setTool( QString tool );

    /// Crida que reinicia a l'estat incial el visor.
    virtual void reset();

    void setWindowLevel( double window, double level );
private:

    vtkRenderer * m_renderer;

};


}


#endif
