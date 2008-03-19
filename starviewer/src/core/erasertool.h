/***************************************************************************
 *   Copyright (C) 2005-2006 by Grup de Gràfics de Girona                  *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGERASERTOOL_H
#define UDGERASERTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class DrawerPolyline;

/**
    @author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class EraserTool : public Tool
{
Q_OBJECT
public:
    ///possibles estats de la tool
    enum { START_CLICK , NONE };
  
    EraserTool( QViewer *viewer, QObject *parent = 0 );

    ~EraserTool();

    void handleEvent( unsigned long eventID );

private: 
    ///inicia el procés de determinació de la zona d'esborrat
    void startEraserAction();
    
    ///dibuixa l'àrea d'esborrat
    void drawAreaOfErasure();
    
    ///cercarà quina és la primitiva més propera, i si està lo suficientment aprop l'esborrarà.
    void erasePrimitive();

private slots:
  

private:

/// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;
    
    ///ens permet determinar l'estat de la tool
    int m_state;
    
    ///primer punt de l'àrea d'esborrat
    double m_startPoint[3];
    
    ///darrer punt de l'àrea d'esborrat
    double m_endPoint[3];
    
    ///polilínia que ens marca la zona d'esborrat.
    DrawerPolyline *m_polyline;
    
};

}

#endif
