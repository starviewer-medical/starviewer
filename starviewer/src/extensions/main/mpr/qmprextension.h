#ifndef UDGQMPRVIEWER_H
#define UDGQMPRVIEWER_H

#include "ui_qmprextensionbase.h"

// FWD declarations
class QAction;
class QStringList;
class vtkAxisActor2D;
class vtkImageReslice;
class vtkPlaneSource;
class vtkTransform;

namespace udg {

// FWD declarations
class DrawerPoint;
class ToolManager;
class Q3DViewer;
class Volume;

/**
    Extensió encarregada de fer l'MPR 2D

    TODO Afegir l'interacció de l'usuari. Per rotar i traslladar els plans podem seguir com a model el que fan els mètodes vtkImagePlaneWidget::Spin()
    i vtkImagePlaneWidget::Translate()
  */
class QMPRExtension : public QWidget, private ::Ui::QMPRExtensionBase {
Q_OBJECT
public:
    QMPRExtension(QWidget *parent = 0);
    ~QMPRExtension();

    /// Rota els graus definits per 'angle' sobre l'eix i punt de rotació que defineixen la intersecció de dos plans.
    /// El primer pla es el que volem rotar i l'altre sobre el qual estroba l'eix d'intersecció/rotació
    /// Caldria resoldre què fer quan els plans son coplanars!!!!
    void rotate(double degrees, double rotationAxis[3], vtkPlaneSource *plane);

    /// Rota el pla especificat pel seu centre
    void rotateMiddle(double degrees, double rotationAxis[3], vtkPlaneSource *plane);

public slots:
    /// Li assigna el volum amb el que s'aplica l'MPR.
    /// A cada finestra es veurà el tall corresponent per defecte. De bon principi cada visor visualitzarà la llesca central corresponent a cada vista.
    /// En les vistes axial i sagital es veuran els respectius plans de tall.
    void setInput(Volume *input);

    /// Canvia la distribució horitzontal de les finestres (el que està a la dreta passa a l'esquerra i viceversa)
    void switchHorizontalLayout();

    /// Activa o desactiva la distribució de finestres al mode MIP
    void switchToMIPLayout(bool isMIPChecked);

signals:
    /// Notificació del canvi de direcció de cadascun dels eixos que podem manipular. Aquests senyals haurien de ser enviats quan canviem la direcció a
    /// través dels controls (línies blaves i vermella)
    void coronalXAxisChanged(double x1, double x2, double x3);
    void coronalZAxisChanged(double z1, double z2, double z3);
    void sagitalYAxisChanged(double y1, double y2, double y3);

private:
    /// Ens diu si un eix és paral·lel a un dels aixos de coordenades X Y o Z
    bool isParallel(double axis[3]);

    /// Obtenim els vectors que defineixen els eixos de cada pla (sagital, coronal)
    void getSagitalXVector(double x[3]);
    void getSagitalYVector(double y[3]);
    void getCoronalXVector(double x[3]);
    void getCoronalYVector(double y[3]);
    void getAxialXVector(double x[3]);
    void getAxialYVector(double y[3]);

    /// Inicialitza les tools que tindrà l'extensió
    void initializeTools();
    
    /// Inicialitza grups d'eines concretes
    void initializeZoomTools();
    void initializeROITools();
    void initializeDistanceTools();
    void initializeAngleTools();

    /// Actualitza valors dels plans i del reslice final
    /// TODO: separar en dos mètodes diferenciats segons quin pla????
    void updatePlanes();

    /// Actualitza els valors del pla donat amb el reslice associat
    void updatePlane(vtkPlaneSource *planeSource, vtkImageReslice *reslice, int extentLength[2]);

    /// Actualitza el punt d'intersecció dels 3 plans
    void updateIntersectionPoint();

    /// Inicialitza les orientacions dels plans de tall correctament perquè tinguin un espaiat, dimensions i límits correctes
    void initOrientation();

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Torna a pintar els controls per modificar els plans
    void updateControls();

    /// Crea els actors i els incicialitza
    void createActors();

    /// Crea les accions
    void createActions();

    /// Crea les connexions entre signals i slots
    void createConnections();

    /// Col·loca i ordena les icones i el menú de les eines del botó donat segons l'última eina seleccionada
    /// TODO Refactoritzar i posar aquest mètode en una classe comuna per totes les extensions i no tenir el codi duplicat aquí i a la extensió 2D
    void rearrangeToolsMenu(QToolButton *menuButton);
    
    /// Ens retorna la línia d'intersecció entre dos plans definida per un punt i un vector
    void planeIntersection(vtkPlaneSource *plane1, vtkPlaneSource *plane2, double r[3], double t[3]);

    /// Calcula el punt d'intersecció de 3 plans a l'espai
    void planeIntersection(vtkPlaneSource *plane1, vtkPlaneSource *plane2, vtkPlaneSource *plane3, double intersectionPoint[3]);

    /// Ens dóna l'eix de rotació d'un planeSource
    void getRotationAxis(vtkPlaneSource *plane, double axis[3]);

    /// Inicialitzador d'objectes pel constructor
    void init();

    /// Retorna la tranformació necessària per passar de coordenades de món a coordenades de la vista sagital.
    vtkTransform* getWorldToSagitalTransform() const;

private slots:
    /// Col·loca i ordena les icones i el menú de les eines de ROI segons l'última tool de ROI seleccionada
    void rearrangeROIToolsMenu();

    /// Col·loca i ordena les icones i el menu de les eines d'angles segons l'última tool d'angles seleccionada
    void rearrangeAngleToolsMenu();

    /// Col·loca i ordena les icones i el menu de les eines de zoom segons l'última tool de Zoom seleccionada
    void rearrangeZoomToolsMenu();

    /// Col·loca i ordena les icones i el menú de les eines de distància segons l'última eina de distància seleccionada
    void rearrangeDistanceToolsMenu();
    
    /// Marca quins són els viewers que s'han de considerar seleccionats
    void changeSelectedViewer();

    /// Efectua un screenshot sobre el visor que estigui seleccionat
    void screenShot();

    /// Mostrar el diàleg per exportar la sèrie del visor seleccionat.
    void showScreenshotsExporterDialog();

    /// Gestiona els events de cada finestra per controlar els eixos de manipulació
    void handleAxialViewEvents(unsigned long eventID);
    void handleSagitalViewEvents(unsigned long eventID);

    /// Detecten si algun dels plans s'han seleccionat per l'usuari
    bool detectAxialViewAxisActor();
    void detectSagitalViewAxisActor();
    void detectPushSagitalViewAxisActor();

    /// Alliberen el pla que es tenia com a seleccionat per a manipular
    void releaseAxialViewAxisActor();
    void releaseSagitalViewAxisActor();

    void rotateAxialViewAxisActor();
    void rotateSagitalViewAxisActor();

    void pushAxialViewAxisActor();
    void pushSagitalViewAxialAxisActor();
    void pushSagitalViewCoronalAxisActor();

    /// Fa les accions pertinents quan una llesca s'ha actualitzat
    void axialSliceUpdated(int slice);

    /// Actualitza el valor del thickSlab i tot el que hi estigui relacionat amb ell
    void updateThickSlab(double value);
    void updateThickSlab(int value);

    /// Decideix si veiem o no la informació d'anotacions
    void showViewerInformation(bool show);

    /// Actualitza l'etiqueta de la projecció que hi ha al viewer "Axial"
    void updateProjectionLabel();

private:
    /// Distància mínima en la que s'ha de trobar un punt per
    /// considerar-se prou proper per fer una operació de picking
    static const double PickingDistanceThreshold;

    /// El reslice de cada vista
    vtkImageReslice *m_sagitalReslice, *m_coronalReslice;

    /// La tranformació que apliquem
    vtkTransform *m_transform;

    /// El volum al que se li practica l'MPR
    Volume *m_volume;

    /// Els actors que representen els eixos que podrem modificar. Línia vermella, blava (axial), blava (sagital) respectivament i el thickSlab
    /// (línies puntejades blaves en vista axial i sagital).
    vtkAxisActor2D *m_sagitalOverAxialAxisActor, *m_axialOverSagitalIntersectionAxis, *m_coronalOverAxialIntersectionAxis,
                   *m_coronalOverSagitalIntersectionAxis, *m_thickSlabOverAxialActor, *m_thickSlabOverSagitalActor;

    /// Drawer points per pintar els centres dels plans a les vistes.
    DrawerPoint *m_axialViewSagitalCenterDrawerPoint;
    DrawerPoint *m_axialViewCoronalCenterDrawerPoint;
    DrawerPoint *m_sagitalViewAxialCenterDrawerPoint;
    DrawerPoint *m_sagitalViewCoronalCenterDrawerPoint;

    /// Ens serà molt útil ens molts de càlculs i a més serà una dada constant un cop tenim l'input
    double m_axialSpacing[3];

    /// Aquesta variable ens servirà per controlar on col·loquem la llesca del pla axial
    double m_axialZeroSliceCoordinate;

    /// Punt d'intersecció entre els 3 plans
    double m_intersectionPoint[3];

    /// Els plans de tall per cada vista (més el thickSlab)
    vtkPlaneSource *m_sagitalPlaneSource, *m_coronalPlaneSource, *m_axialPlaneSource, *m_thickSlabPlaneSource;

    /// Translació necessària per dibuixar les interseccions dels plans a la vista sagital
    double m_sagitalTranslation[3];

    /// Mida de l'extent de l'X i l'Y del pla sagital.
    int m_sagitalExtentLength[2];
    /// Mida de l'extent de l'X i l'Y del pla coronal.
    int m_coronalExtentLength[2];

    /// Conjunt de tools disponibles en aquesta extensió
    QStringList m_extensionToolsList;

    /// Cosetes per controlar el moviment del plans a partir de l'interacció de l'usuari
    double m_initialPickX, m_initialPickY;
    vtkPlaneSource *m_pickedActorPlaneSource;
    vtkImageReslice *m_pickedActorReslice;

    /// Gruix del thickSlab que servirà per al MIP
    double m_thickSlab;

    /// Acció per poder controlar el layout horizontal
    QAction *m_horizontalLayoutAction;
    /// Acció per activar el mip
    QAction *m_mipAction;

    /// Visor de MIP
    Q3DViewer *m_mipViewer;

    /// Estat en el que es troba la manipulació de plans
    enum { None, Rotating, Pushing };
    int m_state;

    /// ToolManager per configurar l'entorn de tools de l'extensió
    ToolManager *m_toolManager;
};

};  // End namespace udg

#endif
