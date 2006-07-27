/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWER_H
#define UDGQ2DVIEWER_H

#include "qviewer.h"

// Fordward declarations
// qt
class QEvent;
class QAction;
// vtk
class vtkImageViewer2;
class vtkPropPicker;
class vtkTextActor;
class vtkEventQtSlotConnect;
class vtkObject;
class vtkCommand;
class vtkRenderer;
class vtkRenderWindowInteractor;
class vtkCornerAnnotation;
class vtkAxisActor2D;
class vtkWindowToImageFilter;
class vtkCoordinate;
class vtkCaptionActor2D;
class vtkScalarBarActor;

namespace udg {

/**

Classe base per als visualitzadors 2D.
El mode d'operació habitual serà el de visualitar un sol volum.
Normalment per poder visualitzar un volum farem el següent
\code
Q2DViewer* visor = new Q2DViewer();
visor->setInput( volum );
visor->setView( Q2DViewer::Axial );
visor->render();

En el cas que desitjem solapar dos volums haurem d'indicar el volum solapat mab el mètode setOverlayInput().
Quan solapem volums tenim 3 maneres de solapar aquests volums, amb un patró de checkerboard, aplicant un blending o un rectilinearWipe, en aquest cas hauríem de fer servir el mètode setOverlay() indicant una de les opcions, Blend, CheckerBoard o RectilinearWipe
\TODO acabar la doc sobre solapament 

Per defecte el visualitzador mostra la llesca central de la vista seleccionada, si volem canviar al llesca farem servir el mètode setSlice().

Podem escollir quines annotacions textuals i de referència apareixeran en la vista 2D a través dels flags "AnnotationFlags" definits com enums. Aquests flags es poden passar en el constructor o els podem modificar a través dels mètodes \c addAnnotation() o \c removeAnnotation() que faran visible o invisible l'anotació indicada. Per defecte el flag és \c AllAnnotation i per tant es veuen totes les anotacions per defecte.

@author Grup de Gràfics de Girona  ( GGG )
*/

// Fordward declarations
class Volume;

class Q2DViewer  : public QViewer{
Q_OBJECT
public:    
    /// Axial: XY, Coronal: XZ, Sagittal: YZ
    enum ViewType{ Axial, Coronal, Sagittal , None };

    /// tipus de fusió dels models
    enum OverlayType{ Blend , CheckerBoard , RectilinearWipe };

    /// Botons del mouse
    enum Buttons{ LeftButton , MiddleButton , RightButton };

    /// Aquests flags els farem servir per decidir quines anotacions seran visibles i quines no
    enum AnnotationFlags{ NoAnnotation = 0x0 , WindowLevelAnnotation = 0x2 , ReferenceAnnotation = 0x4 , RuleAnnotation = 0x8 , AllAnnotation = 0xE };
    
    /// Tools que proporciona... NotSuported és una Tool fictica que indica que la tool en ús a 'aplicació no és aplicable a aquest visor, per tant podríem mostrar un cursor amb signe de prohibició que indiqui que no podem fer res amb aquella tool
    enum Tools{ Zoom , Rotate , Move , Pick , Distance , Cursor , Custom , NotSuported , NoTool , Manipulate };

    enum ManipulateState{ Ready , Picked };
    ManipulateState m_manipulateState;

    Q2DViewer( QWidget *parent = 0 , unsigned int annotations = Q2DViewer::AllAnnotation );
    ~Q2DViewer();

    virtual vtkRenderer *getRenderer();
    virtual vtkRenderWindowInteractor *getInteractor();            
    virtual void setInput( Volume* volume );

    /// Li indiquem quina vista volem del volum: Axial, Coronal o Sagital
    void setView( ViewType view );
    void setViewToAxial(){ setView( Q2DViewer::Axial ); }
    void setViewToCoronal(){ setView( Q2DViewer::Coronal ); }
    void setViewToSagittal(){ setView( Q2DViewer::Sagittal ); }
    
    /// Actualització d'anotacions vàries
    void updateWindowLevelAnnotation();

    /// ens retorna la vista que tenim en aquells moments del volum
    ViewType getView() const { return m_lastView; }

    /// Ens retorna l'ImageViewer
    vtkImageViewer2* getImageViewer(){ return m_viewer; }
    
    /// Afegim el volum solapat
    void setOverlayInput( Volume* volume );    
    
    // Mètodes específics checkerboard
    /// Obtenim el nombre de divisions
    int * getDivisions();
    void getDivisions( int data[3] );

    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int data[3] );
    
    /// Retorna la tool que s'està fent servir en aquell moment
    Tools getCurrentTool(){ return m_currentTool; };

    /// Desa totes les llesques que es veuen al visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveAll( const char *baseName , FileType extension );

    /// Desa la vista actual del visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveCurrent( const char *baseName , FileType extension );

    /// [apanyo] mentre estiguem manipulant els plans serà true
    bool isManipulateOn(){ return m_manipulating; };
    void setManipulate( bool manip ){ m_manipulating = manip; }

public slots:  
    /// Temporal per proves, veurem quins events es criden
    void eventHandler( vtkObject * obj, unsigned long event, void * client_data, vtkCommand * command );
    
    /// Indiquem si volem veure la informació del volum per pantalla \TODO realment es farà servir aquest mètode?
    void displayInformationText( bool display );

    /// Mètodes varis per mostrar o no certa informació en pantalla
    void displaySliceOn();
    void displaySliceOff();
    void displayProtocolNameOn();
    void displayProtocolNameOff();
    void displayWindowInformationOn();
    void displayWindowInformationOff();
    void displaySerieInformationOn();
    void displaySerieInformationOff();
    void displayRulersOn();
    void displayRulersOff();
    void displayRulersLabelsOn();
    void displayRulersLabelsOff();
    void displayPatientOrientationOn();
    void displayPatientOrientationOff();
    void displayScalarBarOn();
    void displayScalarBarOff();
    
    /// Li indica la tool que es fa servir en aquell moment
    bool setCurrentTool( Tools toolName ){ m_currentTool = toolName; return true; };
    ///Mètodes alternatius per activar Tools
    void setCurrentToolToZoom(){ m_currentTool = Zoom; };
    void setCurrentToolToRotate(){ m_currentTool = Rotate; };
    void setCurrentToolToMove(){ m_currentTool = Move; };
    void setCurrentToolToCustom(){ m_currentTool = Custom; };
    void setCurrentToolToPick(){ m_currentTool = Pick; };
    void setCurrentToolToDistance(){ m_currentTool = Distance; };
    void setCurrentToolToManipulate(){ m_currentTool = Manipulate; };
    
    /// retorna la llesca actual
    int getSlice( void ){ return m_currentSlice; }

    /// canvia la llesca que veiem de la vista actual
    void setSlice(int value);

    /// indica el tipu de solapament dels volums, per defecte checkerboard
    void setOverlay( OverlayType overlay ){ m_overlay = overlay; }
    void setOverlayToBlend(){ setOverlay( Q2DViewer::Blend ); };
    void setOverlayToCheckerBoard(){ setOverlay( Q2DViewer::CheckerBoard ); };
    void setOverlayToRectilinearWipe(){ setOverlay( Q2DViewer::RectilinearWipe ); };
    
    /// Actualitza els valors on apunta el cursor
    void updateCursor( double x, double y , double z , double value )
    {
        m_currentCursorPosition[0] = x;
        m_currentCursorPosition[1] = y;
        m_currentCursorPosition[2] = z;

        m_currentImageValue = value;
    }
    
    virtual void render();
    
    // Mètodes específics checkerboard
    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int x , int y , int z );

    /// Fa reset dels paràmetres inicials \TODO:  no es del tot complet el reset
    void reset()
    {    
        updateView();
    }

    void contextMenuRelease( vtkObject *object, unsigned long event, void *client_data, vtkCommand * command);
    
    /// Afegir o treure la visibilitat d'una anotació textual
    void addAnnotation( AnnotationFlags annotation )
    {
        m_enabledAnnotations = m_enabledAnnotations | annotation;
        updateAnnotations();
    }
    void removeAnnotation( AnnotationFlags annotation )
    {
        // si existeix, llavors la "restem", sinó no cal fer res
        if( m_enabledAnnotations & annotation )
        {
            m_enabledAnnotations -= annotation;
            updateAnnotations();
        }
        
    }

    /// Ajusta el window/level
    void setWindowLevel( double window , double level );

    /// Obté el window/level
    void getWindowLevel( double wl[2] );
    
    /// Reseteja el window level al que tingui per defecte el volum
    void resetWindowLevelToDefault();

    /// Actualitza la informació del voxel que hi ha per sota del cursor
    void updateVoxelInformation();
    
protected:
    /// últim botó que s'ha clicat
    int m_lastButtonPressed;

    /// Connector d'events vtk i slots qt
    vtkEventQtSlotConnect *m_vtkQtConnections;

    /// Aquest mètode es fa servir perquè es mostri un menú contextual
    //virtual void contextMenuEvent( QContextMenuEvent *event );
    
    /// Acció temporal de proves. Simplement fa un reset mostrant la llesca central de nou
    QAction *m_resetAction;

    /// per a les vistes 2D farem servir el vtkImageViewer2
    vtkImageViewer2 *m_viewer;

    /// conserva la vista actual
    ViewType m_lastView;    

    /// La llesca actual que estem visualitzant
    int m_currentSlice;

    /// Aquest és el segon volum afegit a solapar
    Volume* m_overlayVolume;

    /// El nombre de divisions per cada dimensió
    int m_divisions[3];

    /// El picker per anotar punts de la imatge
    vtkPropPicker *m_picker;

    /// actor que agafem de l'escena
    vtkAxisActor2D *m_pickedAxisActor;

    /// Textes informatius de l'image actor , ens estalviarà molta feina
    vtkCornerAnnotation *m_textAnnotation;
    
    /// Actualitza la vista en el rendering
    void updateView();

    // Processem l'event de resize de la finestra Qt
    virtual void resizeEvent( QResizeEvent* resize );

private:
    /// [apanyo] per saber si estem manipulant els plans
    bool m_manipulating;
    
    /// flag que ens indica quines anotacions es veuran per la finestra
    unsigned int m_enabledAnnotations;

    /// Refresca les anotacions que s'han de veure i les que no
    void updateAnnotations();
    
    /// inicialitza els strings d'informació que es mostraran a la pantalla
    void initInformationText();

    /// accions associades a interacció amb el ratolí
    void onMouseMove();
    void onLeftButtonUp();
    void onLeftButtonDown();
    void onRightButtonUp();
    void onRightButtonDown();
    
    /// Indica la Tool que s'està fent servir en aquell moment
    Tools m_currentTool;

    /// Tipu de solapament dels volums en cas que en tinguem més d'un
    OverlayType m_overlay;
    
    /// configuració de la interacció amb l'usuari
    void setupInteraction();
    
    /// Crea i inicialitza totes les accions d'aquest widget
    void createActions();

    /// Crea i inicialitza totes les tools d'aquest widget
    void createTools();

    /// Crea i inicialitza totes les anotacions que apareixeran per pantalla
    void createAnnotations();

    /// crea els indicadors d'escala
    void createRulers();
        
    /// Actualitza els rulers
    void updateRulers();

    /// Crea la barra de valors
    void createScalarBar();

    /// Actualitza la barra de valors
    void updateScalarBar();
    
    /// crea els actors necessaris per mostrar la llegenda flotant amb la informació de voxel
    void createVoxelInformationCaption();

    /// crea les anotacions de l'orientació del pacient
    void createOrientationAnnotations();
    
    /// Afegeix tots els actors a l'escena
    void addActors();
    
    /// Els strings amb els textes de cada part de la imatge
    QString m_lowerLeftText, m_lowerRightText, m_upperLeftText, m_upperRightText;

    /// Aquest string indica les anotacions que ens donen les referències del pacient ( Right,Left,Posterior,Anterior,Inferior,Superior)
    QString m_patientOrientationText[4];

    /// A partir de l'string d'orientació del pacient mapeja les anotacions correctes segons com estem mirant el model. A això li afecta també si la vista és axial, sagital o coronal
    void mapOrientationStringToAnnotation();

    /// Marcadors que indicaran les mides relatives del model en les dimensions x,y i z ( ample , alçada i profunditat ). Al ser visor 2D en veurem només dues. Aquestes variaran en funció de la vista en la que ens trobem.
    vtkAxisActor2D *m_sideRuler , *m_bottomRuler;

    /// Coordenades fixes dels rulers que els ajustaran a un dels extrems inferiors/superiors o laterals de la pantalla
    vtkCoordinate *m_anchoredRulerCoordinates;

    /// coordenades dels extrems per cada dimensió del ruler
    double m_rulerExtent[6];
    
    /// Textes adicionals d'anotoació
    vtkTextActor *m_patientOrientationTextActor[4];

    /// Actualització d'anotacions vàries
    void updateSliceAnnotation();
    void updateWindowSizeAnnotation();

    /// Mida de voxels en cada direcció
    int m_size[3];

    /// Valors dels window level per defecte. Pot venir donat pel DICOM o assignat per nosaltres a un valor estàndar de constrast
    double m_defaultWindow, m_defaultLevel;

    /// Llegenda que segueix el cursor amb el valor del voxel
    vtkCaptionActor2D *m_voxelInformationCaption;

    /// Barra que mostra l'escala de colors del model que estem visualitzant \TODO quan tinguem models fusionats tindrem una o dues barres d'escala de colors?
    vtkScalarBarActor *m_scalarBar;
    
signals:
    /// envia la nova llesca en la que ens trobem
    void sliceChanged(int);

    /// indica el nou window level
    void windowLevelChanged( double window , double level );

    /// informem del punt que hem clicat amb el botó esquerre, coordenades de món
    void leftButtonDown( double x , double y );

    /// informem del punt sobre el que movem el mouse, coordenades de món
    void mouseMove( double x, double y );

    /// informem del punt que hem deixat de clicar amb el botó esquerre, coordenades de món
    void leftButtonUp( double x , double y );

    /// informem del punt que hem clicat amb el botó dret, coordenades de món
    void rightButtonDown( double x , double y );

    /// informem del punt que hem deixat de clicar amb el botó dret, coordenades de món
    void rightButtonUp( double x , double y );
    
};

};  //  end  namespace udg 

#endif
