#ifndef UDGMAGNIFYINGGLASSTOOLDATA_H
#define UDGMAGNIFYINGGLASSTOOLDATA_H

#include "tooldata.h"

namespace udg {

class Q2DViewer;

/**
    Classe que guarda les dades compartides per la tool de magnifying glass.
*/
class MagnifyingGlassToolData : public ToolData {
Q_OBJECT
public:
    MagnifyingGlassToolData(QObject *parent = 0);
    ~MagnifyingGlassToolData();

    /// Retorna el q2dviewer on es mostra la lupa
    Q2DViewer* get2DMagnifyingGlassViewer() const;

    /// Assigna/Obtè el factor de zoom
    void setZoomFactor(double factor);
    double getZoomFactor() const;

    /// Assigna/Obtè la variable que controla si s'ha fet show del visor o no encara
    void setViewerShown(bool shown);
    bool viewerHasBeenShown() const;

private:
    /// Inicialitza el visor i els seus paràmetres
    void initializeViewer();

private:
    /// Visor on es mostrarà la imatge magnificada
    Q2DViewer *m_2DMagnifyingGlassViewer;

    /// Factor de zoom a aplicar a la vista magnificada
    double m_zoomFactor;
    
    /// Controla si s'ha fet show del viewer o no
    double m_viewerHasBeenShown;
};

}

#endif
