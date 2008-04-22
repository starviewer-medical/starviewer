#ifndef UDGQVolumeContourDelimiterExtension_H
#define UDGQVolumeContourDelimiterExtension_H

#include "ui_qvolumecontourdelimiterextensionbase.h" 
#include <vtkSplineWidget.h>
#include <QList> 
#include <QMultiMap>  

using namespace Ui;

// FWD declarations
class QAction;
class vtkImageMask;
class vtkActor;
class QActionGroup;
class vtkPNGReader;
class vtkInteractorStyleImage;
class vtkLinearExtrusionFilter;
class vtkImageData;
class vtkPolyData;
class vtkPropPicker;


namespace udg {
    
    // FWD declarations
    class Volume;
    class ToolsActionFactory;
    class Point;
    
/**
	@author Grup de Gràfics de Girona  ( GGG ) <vismed@ima.udg.es>
*/
class QVolumeContourDelimiterExtension : public QWidget, private QVolumeContourDelimiterExtensionBase
{
Q_OBJECT
public:
    enum { NONE , ZOOMING };
    
    QVolumeContourDelimiterExtension(QWidget *parent = 0);

    ~QVolumeContourDelimiterExtension();
    
    /// Li assigna el volum principal
    void setInput( Volume *input );
    
        ///assigna atributs per defecte als objectes de la classe
    void setAttributes();
    
    ///crea les accions d'aquesta classe associades a les tools
    void createActions();

    ///insereix un punt a la llista de punts marcats
    void insertPoint();

    ///mostra els splines amb els punts que s'han marcat
    void showSplines();
    
    ///assigna els atributs comuns a tots els splines
    void setSplineAtributes( vtkSplineWidget* );

private slots:
    ///guarda l'spline actual en la llesca actual i restaura els splines de la llesca a la qual hem canviat
    void saveCurrentSplineAndGetNeededSplines( int );
    
    ///crea la màscara, fent una extrusió entre una imatge negra i els vtkPolyData i crea un model de vòxels, inserint la màscara binària obtinguda a partir de l'spline en la llesca corresponent
    void createModelOfVoxelsWithObtainedMasks();
    
    ///activa / desactiva el botó d'afegir un nou spline
    void buttonAddSplineEnabled( bool );
    
     ///Afegeix un nou spline a l'escena
    void addNewSpline();
    
    void myEventHandler( unsigned long id );
    
private:
    ///Factory de Tools
    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;
    
    ///Guarda l'estat de l'spline que fa referència a si està tancat o no
    bool m_splineClosed;
    
    ///Estil d'interactuador amb el Q2DViewer
    vtkInteractorStyleImage* m_interactorStyle;
    
    ///Estructura per passar els punts a l'spline
    vtkPoints* m_points;
    
    ///widget que representa l'spline
    vtkSplineWidget* m_splineWidget;
    
    ///objecte vtk que s'encarrega de fer extrusió sobre la imatge
    vtkLinearExtrusionFilter* m_extrude;
    
    ///atribut per guardar el tipus de vista amb el que estem treballant
    Q2DViewer::CameraOrientationType m_view;
    
    ///per guardar la llesca de treball;
    int m_currentSlice;

/// El volum principal
    Volume *m_volume;
    
       ///atribut per guardar la sortida de l'stencil
    vtkImageData *m_stencilOutputDataImage;
    
    ///directori on deixarem les dades temporals. Al finalitzar aquest procés, aquestes dades seran esborrades.
    QString m_tempDirectory;
    
    ///guarda el valor de la llesca anteriorment visitada
    int m_previousSlice;
    
    ///indica si l'usuari ha anotat un spline nou
    bool m_hasAddedNewSpline;
    
    ///llesca on s'edita l'spline actual
    int m_sliceOfCurrentSpline;
    
    ///ens indica si s'ha dibuixat el primer spline
    bool m_firstSplineIsFixed;
    
    ///ens indiquen les llesques mínima i màxima
    int m_firstSlice;
    int m_lastSlice;
    
    ///Multimap de tots els vtkPolyData generats pels splines
    QMultiMap<int, vtkSplineWidget*> m_splinesMap;    
    
    ///llista on guardem els punts que es van anotant
    QList<Point> m_pointList;
    
    ///Acció de zoom
    QAction *m_zoomAction;
    
    ///Acció d'edició d'splines
    QAction *m_splineAction;
    
    ///Acció de windowLevel
    QAction *m_windowLevelAction;
    
    ///Acció d'slicing
    QAction *m_slicingAction;
   };
}

#endif

