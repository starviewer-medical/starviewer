/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQMPRVIEWER_H
#define UDGQMPRVIEWER_H

#include "ui_qmprextensionbase.h"
using namespace Ui; // \TODO sembla que l'obligació de posar això perquè funcioni és culpa dels FWD decls que hi ha fora de udg, en els que no hi ha FWD decls no cal
#include <QString>

// FWD declarations
class vtkAxisActor2D;
class vtkPlaneSource;
class vtkImageReslice;
class vtkTransform;
class vtkActor2D;
class QAction;

namespace udg {

// FWD declarations
class Volume;
class Q3DViewer;
class ToolsActionFactory;

/**
Extensió encarregada de fer l'MPR 2D

@author Grup de Gràfics de Girona  ( GGG )

\TODO Afegir l'interacció de l'usuari. Per rotar i traslladar els plans podem seguir com a model el que fan els mètodes vtkImagePlaneWidget::Spin() i vtkImagePlaneWidget::Translate()
*/
class QMPRExtension : public QWidget , private QMPRExtensionBase /*Ui::QMPRExtensionBase*/{
Q_OBJECT
public:
    QMPRExtension( QWidget *parent = 0 );

    ~QMPRExtension();
    /**
        Li assigna el volum amb el que s'aplica l'MPR.
        A cada finestra es veurà el tall corresponent per defecte. De bon principi cada visor visualitzarà la llesca central corresponent a cada vista. En les vistes axial i sagital es veuran els respectius plans de tall.
    */
    void setInput( Volume *input );

    /// Rota els graus definits per 'angle' sobre l'eix i punt de rotació que defineixen la intersecció de dos plans. EL primer pla es el que volem rotar i l'altre sobre el qual estroba l'eix d'intersecció/rotació
    // caldria resoldre què fer quan els plans son coplanars!!!!
    void rotate( double degrees , double rotationAxis[3] ,  vtkPlaneSource* plane );

    /// Rota el pla especificat pel seu centre
    void rotateMiddle( double degrees , double rotationAxis[3] ,  vtkPlaneSource* plane );

public slots:
    /// Canvia la distribució horitzontal de les finestres ( el que està a la dreta passa a l'esquerra i viceversa )
    void switchHorizontalLayout();

    /// Activa o desactiva la distribució de finestres al mode MIP
    void switchToMIPLayout( bool isMIPChecked );

signals:
    /// Notificació del canvi de direcció de cadascun dels eixos que podem manipular. Aquests senyals haurien de ser enviats quan canviem la direcció a través dels controls ( línies blaves i vermella)
    void coronalXAxisChanged( double x1 , double x2 , double x3 );
    void coronalZAxisChanged( double z1 , double z2 , double z3 );
    void sagitalYAxisChanged( double y1 , double y2 , double y3 );

private:
    /// Ens diu si un eix és paral·lel a un dels aixos de coordenades X Y o Z
    bool isParallel( double axis[3] );

    /// Calcula l'angle entre dos vectors. Retorna el valor en radians
    double angleInRadians( double vec1[3] , double vec2[3] );

    /// Calcula l'angle entre dos vectors. Retorna el valor en graus
    double angleInDegrees( double vec1[3] , double vec2[3] );

    /// Obtenim els vectors que defineixen els eixos de cada pla ( sagital , coronal )
    void getSagitalXVector( double x[3] );
    void getSagitalYVector( double y[3] );
    void getCoronalXVector( double x[3] );
    void getCoronalYVector( double y[3] );
    void getAxialXVector( double x[3] );
    void getAxialYVector( double y[3] );

    /// Actualitza valors dels plans i del reslice final \TODO: separar en dos mètodes diferenciats segons quin pla????
    void updatePlanes();

    /// Actualitza els valors del pla donat amb el reslice associat
    void updatePlane( vtkPlaneSource *planeSource , vtkImageReslice *reslice );

    /// Actualitza el punt d'intersecció dels 3 plans
    void updateIntersectionPoint();

    /// inicialitza les orientacions dels plans de tall correctament perquè tinguin un espaiat, dimensions i límits correctes
    void initOrientation();

    /// El reslice de cada vista
    vtkImageReslice *m_sagitalReslice, *m_coronalReslice;

    /// La tranformació que apliquem
    vtkTransform *m_transform;

    /// El volum al que se li practica l'MPR
    Volume *m_volume;

    /// Els actors que representen els eixos que podrem modificar. Línia vermella, blava (axial), blava (sagital) respectivament i el thickSlab ( línies puntejades blaves en vista axial i sagital ).
    vtkAxisActor2D *m_sagitalOverAxialAxisActor, *m_axialOverSagitalIntersectionAxis, *m_coronalOverAxialIntersectionAxis , *m_coronalOverSagitalIntersectionAxis, *m_thickSlabOverAxialActor , *m_thickSlabOverSagitalActor;

    /// Ens serà molt útil ens molts de càlculs i a més serà una dada constant un cop tenim l'input
    double m_axialSpacing[3];

    /// Punt d'intersecció entre els 3 plans
    double m_intersectionPoint[3];

    /// Llegir/Escriure la configuració de l'aplicació
    void readSettings();
    void writeSettings();

    /// Torna a pintar els controls per modificar els plans
    void updateControls();

    /// Crea els actors i els incicialitza
    void createActors();

    /// Crea les accions
    void createActions();

    /// crea les connexions entre signals i slots
    void createConnections();

    /// Els plans de tall per cada vista ( més el thickSlab )
    vtkPlaneSource *m_sagitalPlaneSource, *m_coronalPlaneSource , *m_axialPlaneSource , *m_thickSlabPlaneSource;

    /// ens retorna la línia d'intersecció entre dos plans definida per un punt i un vector
    void planeIntersection( vtkPlaneSource* plane1 , vtkPlaneSource *plane2 , double r[3] , double t[3] );

    /// Calcula el punt d'intersecció de 3 plans a l'espai
    void planeIntersection( vtkPlaneSource *plane1 , vtkPlaneSource *plane2 , vtkPlaneSource *plane3 , double intersectionPoint[3] );

    /// Ens dóna l'eix de rotació d'un planeSource
    void getRotationAxis( vtkPlaneSource *plane , double axis[3] );

    /// Inicialitzador d'objectes pel constructor
    void init();

    /// El directori on es desaran les imatges per defecte
    QString m_defaultSaveDir;

    /// Filtre de fitxers que es poden desar
    QString m_fileSaveFilter;

    /// Cosetes per controlar el moviment del plans a partir de l'interacció de l'usuari
    double m_initialPickX , m_initialPickY;
    vtkPlaneSource *m_pickedActorPlaneSource;
    vtkImageReslice *m_pickedActorReslice;

    /// Gruix del thickSlab que servirà per al MIP
    double m_thickSlab;

    /// Acció per poder controlar el layout horizontal
    QAction *m_horizontalLayoutAction;
    /// Acció per activar el mip
    QAction *m_mipAction;
    /// Accions associades a tools
    QAction *m_slicingAction;
    QAction *m_windowLevelAction;
    QAction *m_zoomAction;
    QAction *m_moveAction;
    QAction *m_screenShotAction;
    QAction *m_voxelInformationAction;
    QAction *m_rotate3DAction;
    ToolsActionFactory *m_actionFactory;

    /// Grup de botons en format exclusiu
    QActionGroup *m_toolsActionGroup;

    /// Visor de MIP
    Q3DViewer *m_mipViewer;

    /// Estat en el que es troba la manipulació de plans
    enum { NONE , ROTATING , PUSHING };
    int m_state;

private slots:
    /// gestiona els events de cada finestra per controlar els eixos de manipulació
    void handleAxialViewEvents( unsigned long eventID );
    void handleSagitalViewEvents( unsigned long eventID );

    /// s'encarreguen de moure les llesques dels plans
    void detectAxialViewAxisActor();
    void releaseAxialViewAxisActor();

    void detectSagitalViewAxisActor();
    void releaseSagitalViewAxisActor();

    void rotateAxialViewAxisActor(); // void moveAxialViewAxisActor( double x , double y );
    void rotateSagitalViewAxisActor(); // void rotateAxisActor( double x , double y );

    /// s'encarreguen de moure les llesques dels plans
    void detectPushAxialViewAxisActor();
    void releasePushAxialViewAxisActor();

    void detectPushSagitalViewAxisActor();
    void releasePushSagitalViewAxisActor();

    void pushAxialViewAxisActor(); // void pushAxisActor( double x , double y );
    void pushSagitalViewAxisActor(); // void pushAxialActor( double x , double y );

    /// Fa les accions pertinents quan una llesca s'ha actualitzat
    void axialSliceUpdated( int slice );

    /// Fa el procés de guardar les imatges capturades
    void saveImages();

    /// Actualitza el valor del thickSlab i tot el que hi estigui relacionat amb ell
    void updateThickSlab( double value );
    void updateThickSlab( int value );
};

};  //  end  namespace udg

#endif
