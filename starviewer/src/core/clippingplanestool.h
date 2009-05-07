#ifndef UDGCLIPPINGPLANESTOOL_H
#define UDGCLIPPINGPLANESTOOL_H

#include "tool.h"

class vtkEventQtSlotConnect;
class vtkBoxWidget;
class vtkObject;
class vtkCommand;

namespace udg {

class Q3DViewer;

/**
    Tool que permet manipular els plans de tall en un visor 3D
    Mostra un widget que permet rotar, traslladar i manipular els plans de 
    la bounding box del volum del visor 3D
*/
class ClippingPlanesTool : public Tool
{
Q_OBJECT
public:
    ClippingPlanesTool( QViewer *viewer, QObject *parent = 0 );
    ~ClippingPlanesTool();

    /// Gestiona els events del visor
    void handleEvent( long unsigned eventID );

private slots:
    /// Es connectarà als events emesos pel widget per saber quan s'ha interactuat i així aplicar els plans de tall sobre el visor
    void boundingBoxEventHandler(vtkObject *obj, unsigned long event, void *client_data, void *call_data, vtkCommand *command);
#ifdef VTK_QT_5_0_SUPPORT
    /// \TODO Esborrar el metode. Metode de compatibilitat amb VTK 5.0 de vtkQtConnection
    void boundingBoxEventHandler(vtkObject *obj, unsigned long event, void *client_data, vtkCommand *command);
#endif

    /// Cada cop que s'hagi canviat l'input del visor hem de posar a punt 
    /// el widget d'acord amb el nou volum
    void updateInput();

private:
    /// Obté els plans del widget i els aplica com a plans de tall sobre el volum
    void updateViewerClippingPlanes();

protected:
    /// Viewer 3D sobre el qual treballem
    Q3DViewer *m_3DViewer;

    /// Widget vtk per manipular els plans de tall
    vtkBoxWidget *m_boundingBoxClipperWidget;

    /// Connector pels events del widget amb els nostres slots
    vtkEventQtSlotConnect *m_vtkQtConnections;
};

} // end namespace udg

#endif
