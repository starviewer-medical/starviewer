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

// FWD declarations
class vtkAxisActor2D;
class vtkPlaneSource;
class vtkImageReslice;
class vtkTransform;
class vtkActor2D;

namespace udg {

// FWD declarations
class Volume;
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
    
    /// Inicialitza de nou els paràmetres de l'aplicació
    void reset();
    
    /// Indica la llesca que volem veure de cadascuna de les diferents vistes. Aquest canvi repercuteix sobre els eixos de control de cada visor
    void setAxialSlice( int slice );
    void setCoronalSlice( int slice );
    void setSagitalSlice( int slice );
    /// Indica la llesca que volem veure de cadascuna de les diferents vistes. View = 0 : Axial, 1 : Coronal , 2: Sagital
    void setSlice( int slice , int view = 0 );
    
    /// Rota els graus definits per 'angle' sobre l'eix i punt de rotació que defineixen la intersecció de dos plans. EL primer pla es el que volem rotar i l'altre sobre el qual estroba l'eix d'intersecció/rotació
    // caldria resoldre què fer quan els plans son coplanars!!!!
    void rotate( double degrees , double rotationAxis[3] ,  vtkPlaneSource* plane , vtkImageReslice *reslice );
    
    /// Rota el pla especificat pel seu centre
    void rotateMiddle( double degrees , double rotationAxis[3] ,  vtkPlaneSource* plane , vtkImageReslice *reslice );
       
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
    /// Els actors que representen els eixos que podrem modificar. Línia vermella, blava (axial), blava (sagital) rspectivament.
    vtkAxisActor2D *m_sagitalOverAxialAxisActor, *m_axialOverSagitalIntersectionAxis, *m_coronalOverAxialIntersectionAxis ,  *m_coronalOverSagitalIntersectionAxis;    
    
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
    /// crea les connexions entre signals i slots
    void createConnections();
    
    /// Els plans de tall per cada vista
    vtkPlaneSource *m_sagitalPlaneSource, *m_coronalPlaneSource , *m_axialPlaneSource;
    
    /// ens retorna la línia d'intersecció entre dos plans definida per un punt i un vector     
    void planeIntersection( vtkPlaneSource* plane1 , vtkPlaneSource *plane2 , double r[3] , double t[3] );
    /// Calcula el punt d'intersecció de 3 plans a l'espai 
    void planeIntersection( vtkPlaneSource *plane1 , vtkPlaneSource *plane2 , vtkPlaneSource *plane3 , double intersectionPoint[3] );
    
private slots:
    // temporal
    void rotateXPlus();
    void rotateXMinus();
    void rotateYPlus();
    void rotateYMinus();
    void rotateSagitalYPlus();
    void rotateSagitalYMinus();
    
    void sagitalSlicePlus();
    void sagitalSliceMinus();
    void coronalSlicePlus();
    void coronalSliceMinus();
    /// Fan les accions pertinents quan una llesca s'ha actualitzat
    void axialSliceUpdated( int slice );
    void sagitalSliceUpdated( int slice );
    void coronalSliceUpdated( int slice );
};

/**
* Encapsula les funcionalitats de l'actor que controla els diferents plans de tall.
*//*
class PlaneController2D : public QObject{
Q_OBJECT
public:
    /// Estats en el que es pot trbar. Moving: quan movem el pla ( llesques ), Spinning: rotació sobre un eix
    enum State{ Moving , Spinning , None };
    PlaneController2D( QObject *parent = 0 );
    ~PlaneController2D( );
    /// Li assignem el color a l'actor
    void setColor( double r , double g , double b );
    /// Retorna l'actor 
    vtkAxisActor2D *getPlaneController();
private:
    /// L'actor en qüestió
    vtkAxisActor2D *m_planeControllerActor;
    /// L'estat d'interacció en el que es troba
    State m_state;

};*/

};  //  end  namespace udg 

#endif
