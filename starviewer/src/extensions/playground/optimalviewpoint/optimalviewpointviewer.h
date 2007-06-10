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
 * Cal proporcionar-li un renderer extern abans de fer-la servir.
 *
 * \author Grup de Gràfics de Girona (GGG) <vismed@ima.udg.edu>
 */
class OptimalViewpointViewer : public QViewer {


    Q_OBJECT


public:

    OptimalViewpointViewer( QWidget * parent = 0 );
    virtual ~OptimalViewpointViewer();

    /**
     * Assigna a la classe el renderer que haurà de fer la visualització si
     * encara no en té cap. Si la classe ja té un renderer o el mètode rep un
     * null no fa res.
     */
    void setRenderer( vtkRenderer * renderer );

    /// Retorna el renderer.
    virtual vtkRenderer * getRenderer();

    /// Indiquem les dades d'entrada.
    virtual void setInput( Volume * volume );


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


private:

    vtkRenderer * m_renderer;


};



}



#endif
