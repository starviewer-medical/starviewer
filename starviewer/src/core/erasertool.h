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
class DrawerPolygon;

/**
    Tool per esborrar primitives i annotacions de pantalla.
    Si es fa un clic, busca la primitiva més propera al punt clicat i si està dins d'un llindar l'esborra.
    També es pot clicar en un punt i arrossegar-lo per dibuixar un requadre, esborrant totes les primitives que
    estiguin dins de l'àrea que delimita el requadre dibuixat.
*/
class EraserTool : public Tool {
Q_OBJECT
public:
    /// Possibles estats de la tool
    enum { StartClick , None };
  
    EraserTool(QViewer *viewer, QObject *parent = 0);
    ~EraserTool();

    void handleEvent(unsigned long eventID);

private: 
    /// Inicia el procés de determinació de la zona d'esborrat
    void startEraserAction();
    
    /// Dibuixa l'àrea d'esborrat
    void drawAreaOfErasure();
    
    /// Cercarà quina és la primitiva més propera, i si està lo suficientment aprop l'esborrarà.
    void erasePrimitive();

private slots:
    /// Slot que torna la tool al seu estat inicial
    void reset();

private:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;
    
    /// Ens permet determinar l'estat de la tool
    int m_state;
    
    /// Primer punt de l'àrea d'esborrat
    double m_startPoint[3];
    
    /// Darrer punt de l'àrea d'esborrat
    double m_endPoint[3];
    
    /// Polígon que ens marca la zona d'esborrat.
    DrawerPolygon *m_polygon;
};

}

#endif
