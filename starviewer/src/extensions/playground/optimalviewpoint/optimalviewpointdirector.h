/***************************************************************************
 *   Copyright (C) 2006 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/

#ifndef UDGOPTIMALVIEWPOINTDIRECTOR_H
#define UDGOPTIMALVIEWPOINTDIRECTOR_H

#include <QObject>
#include <QWidget>

namespace udg {

class OptimalViewpoint;
class OptimalViewpointParameters;
class OptimalViewpointViewer;

class QApplicationMainWindow;

/**
 * Subclasse de Director. En aquest cas fa de pont amb un mètode de
 * visualització anomenat Optimal Viewpoint.
 *
 * Inclou un punter a la classe de paràmetres específics del seu mètode, i un a
 * la classe que encapsula el mètode de visualització (OptimalViewpoint).
 *
 * Totes les classes heredades de Director inclouran un mètode setParameters on
 * li indicarem el punter a la classe de paràmetres corresponent. També
 * implementaran l'slot virtual execute, on es farà la feina més important de
 * l'execució d'un mètode.
 */
class OptimalViewpointDirector : public QObject {

    Q_OBJECT

public:

    OptimalViewpointDirector( QWidget * viewerWidget, QObject * parent = 0 );
    virtual ~OptimalViewpointDirector();

    /// Assigna l'objecte que conté els paràmetres del mètode de visualització.
    void setParameters( OptimalViewpointParameters * parameters );

public slots:

    /**
     * Aquest mètode aplica el mètode de visualització. Comprova primer si té
     * els paràmetres necessaris, sinó no fa res. Slot públic heredat de
     * Director.
     */
    virtual void execute();

private:

    QWidget * m_viewerWidget;

    /// Mètode.
    OptimalViewpoint * m_method;

    /// Paràmetres.
    OptimalViewpointParameters * m_parameters;

    /// Visualitzador principal.
    OptimalViewpointViewer * m_viewer;

}; // end class OptimalViewpointDirector

}; // end namespace udg

#endif // UDGOPTIMALVIEWPOINTDIRECTOR_H
