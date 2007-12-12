/***************************************************************************
 *   Copyright (C) 2005-2007 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGPOLYLINEROITOOL_H
#define UDGPOLYLINEROITOOL_H

#include "tool.h"

#include <QTime>

namespace udg {

class PolylineROIToolData;
class Q2DViewer;
class Volume;
class ImagePlane;
class DrawerPolyline;

/**
Tool per dibuixar ROIS polilinies

	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class PolylineROITool : public Tool
{
Q_OBJECT
public:
    PolylineROITool( QViewer *viewer, QObject *parent = 0 );

    ~PolylineROITool();

    ///funcio manejadora dels events passats.
    void handleEvent( long unsigned eventID );

private slots:

private:
    /// ens permet anotar el seguent punt de la polilinia. Si la primitiva no ha sigut creada, abans d'afegir el nou punt, la crea.
    void annotateNewPoint();

    ///ns simula com quedaria la polilinia que estem editant si la tanquessim. ens serveix per a veure dinamicament l'evolucio de la polilinia.
    void simulateClosingPolyline();

    /// metode per respondre els diferents tipus d'events de teclat
    void answerToKeyEvent();

    /// metode per calcular la mitjana de gris de la regio del polyline
    double computeGrayMean();
    double computeGrayMeanAxial();
    double computeGrayMeanSagittal();
    double computeGrayMeanCoronal();
    
    /// calcula el voxel a partir de l'espaiat de la imatge i la coordenada i retorna el valor de gris
    int getGrayValue( double *coords, double spacing0, double spacing1, double spacing2 );

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

    /// Temps que ha passat des de que s'ha iniciat la tool
    QTime m_time;

    /// Temps en milisegons de l'ultim event tractat
    int m_latestTime;

    ///polilinia principal: es la polilinia que ens marca la forma que hem anat editant.
    DrawerPolyline *m_mainPolyline;

    ///polilinia de tancament: es la polilinia que ens simula com quedaria la polilinia principal si es tanques, es a dir, uneix l'ultim punt anotat i el primer punt de la polilinia.
    DrawerPolyline *m_closingPolyline;
};

}

#endif
