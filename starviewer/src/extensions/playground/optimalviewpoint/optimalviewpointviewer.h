/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGOPTIMALVIEWPOINTVIEWER_H
#define UDGOPTIMALVIEWPOINTVIEWER_H

#include <QWidget>

class QVTKInteractor;
class QVTKWidget;
class vtkRenderer;

namespace udg {

class Volume;

/**
 * Aquesta classe fa la visualització dels Miralls Màgics.
 *
 * Cal proporcionar-li un renderer extern abans de fer-la servir.
 */
class OptimalViewpointViewer : public QWidget {

    Q_OBJECT

public:

    OptimalViewpointViewer( QWidget * parent = 0, vtkRenderer * renderer = 0 );
    virtual ~OptimalViewpointViewer();

    /**
     * Assigna a la classe el renderer que haurà de fer la visualització si
     * encara no en té cap. Si la classe ja té un renderer o el mètode rep un
     * null no fa res.
     */
    void setRenderer( vtkRenderer * renderer );

    /// Retorna l'interactor de la finestra.
    QVTKInteractor * getInteractor() const;

    /**
     * Força l'actualització de la visualització. Si la classe no té un renderer
     * no fa res.
     */
    void update();

private:

    QVTKWidget * m_vtkWidget;
    vtkRenderer * m_renderer;

}; // end class OptimalViewpointViewer

}; // end namespace udg

#endif // UDGOPTIMALVIEWPOINTVIEWER_H
