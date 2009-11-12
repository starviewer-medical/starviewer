/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGROITOOL_H
#define UDGROITOOL_H

#include "tool.h"
#include "volume.h"
#include <QPointer>

namespace udg {

class Q2DViewer;
class DrawerPolyline;

/**
*   Tool per dibuixar ROIS amb polilínies
*
*   @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class ROITool : public Tool {
Q_OBJECT
public:
    ROITool( QViewer *viewer, QObject *parent = 0 );
    ~ROITool();

    void handleEvent( long unsigned eventID );

    /// Calcula el vòxel a partir de l'espaiat de la imatge i la coordenada i retorna el valor de gris
    Volume::VoxelType getGrayValue( double *coords );

signals:
    /// The drawing has finished
    void finished();

protected:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Polilínia principal: és la polilínia que ens marca la forma que hem anat editant.
    QPointer<DrawerPolyline> m_mainPolyline;

    /// Polilínia de tancament: es la polilínia que ens simula com quedaria la polilínia principal si es tanques, es a dir, uneix l'últim punt anotat i el primer punt de la polilínia.
    QPointer<DrawerPolyline> m_closingPolyline;

private:
    /// Ens permet anotar el següent punt de la polilínia. Si la primitiva no ha sigut creada, abans d'afegir el nou punt, la crea.
    void annotateNewPoint();

    /// Ens simula com quedaria la polilínia que estem editant si la tanquèssim. Ens serveix per a veure dinàmicament l'evolució de la polilínia.
    void simulateClosingPolyline();

    /// Mètode que tanca la forma de la polilínia que s'ha dibuixat
    void closeForm();
};

}

#endif
