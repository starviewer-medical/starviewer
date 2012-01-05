#ifndef UDGMAGNIFYINGGLASSTOOL_H
#define UDGMAGNIFYINGGLASSTOOL_H

#include "tool.h"

#include <vtkSmartPointer.h>

class vtkRenderer;
class vtkCamera;
class QPoint;
class QSize;

namespace udg {

class Q2DViewer;

/**
    Tool per mostrar una vista magnificada de la imatge que hi hagi per sota del cursor.
    L'efecte seria com si poséssim una lupa al cim de la imatge i la poguéssim moure per dins del visor.
 */
class MagnifyingGlassTool : public Tool {
Q_OBJECT
public:
    MagnifyingGlassTool(QViewer *viewer, QObject *parent = 0);
    ~MagnifyingGlassTool();

    void handleEvent(unsigned long eventID);

protected:
    /// Viewer 2D sobre el qual treballem
    Q2DViewer *m_2DViewer;

private slots:
    /// Actualitza la càmera i la posició del viewport
    void update();

    /// Actualitza la vista magnificada
    void updateMagnifiedView();
    
    /// Actualitza els paràmetres de la càmera de magnificació
    void updateCamera();

    /// Elimina el renderer magnificat del render window del Q2DViewer
    void removeMagnifiedRenderer();

private:
    /// Habilita o deshabilita les connexions
    void enableConnections(bool enable = true);

    /// Afegeix el renderer magnificat amb els paràmetres adequats
    void addMagnifiedRenderer();

    /// Calcula les mides del viewport magnificat segons el punt a magnificar i la mida del viewer donada
    void updateMagnifiedRendererViewport(const QPoint &center, const QSize &viewerSize);

    /// Ens retorna el factor de zoom que tenim configurat per aplicar
    double getZoomFactor();
private:
    /// Renderer on tindrem la vista magnificada
    vtkRenderer *m_magnifiedRenderer;
    
    /// Indica si s'ha mostrat o no el renderer de magnificació
    bool m_magnifyingRendererIsVisible;
    
    /// Càmera de la vista magnificada
    vtkSmartPointer<vtkCamera> m_magnifiedCamera;
};

}

#endif
