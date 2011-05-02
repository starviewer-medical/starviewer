#ifndef UDGERASERTOOL_H
#define UDGERASERTOOL_H

#include "tool.h"

namespace udg {

class Q2DViewer;
class DrawerPolygon;
class DrawerPrimitive;

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

    /// Determina quina primitiva es pot esborrar amb el punt, vista i llesca donats.
    /// @param point Coordenada de món a partir de la qual volem determinar si hi ha una primitiva pròxima
    /// @param view Vista actual del model (Axial, Sagital, Coronal)
    /// @param slice Llesca d'on volem obtenir la primitiva
    /// @return La primitiva que estigui propera al punt determinat, dins d'un llindar determinat. 
    /// Si el punt no és prou proper segons el llindar o no hi ha primitives en aquella llesca, la primitiva retornada serà nul·la.
    DrawerPrimitive* getErasablePrimitive(double point[3], int view, int slice);

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
