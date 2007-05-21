/***************************************************************************
 *   Copyright (C) 2005 by Grup de Gràfics de Girona                       *
 *   http://iiia.udg.es/GGG/index.html?langu=uk                            *
 *                                                                         *
 *   Universitat de Girona                                                 *
 ***************************************************************************/
#ifndef UDGQ2DVIEWER_H
#define UDGQ2DVIEWER_H

#include "qviewer.h"
#include <QMap>

// Fordward declarations
// qt
class QEvent;
class QAction;
// vtk
class vtkImageViewer2;
class vtkPropPicker;
class vtkTextActor;
class vtkObject;
class vtkCommand;
class vtkRenderer;
// class vtkRenderWindowInteractor;
class vtkCornerAnnotation;
class vtkAxisActor2D;
class vtkWindowToImageFilter;
class vtkCoordinate;
class vtkCaptionActor2D;
class vtkScalarBarActor;
class vtkInteractorStyleImage;
class vtkImageBlend;

/// tractament múltiples vistes
class vtkPropCollection;
class vtkRendererCollection;
class vtkCollection;
class vtkActor2DCollection;

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
class Q2DViewerToolManager;
class Tool;

class Q2DViewer  : public QViewer{
Q_OBJECT
public:
    /// Axial: XY, Coronal: XZ, Sagittal: YZ
    enum ViewType{ Axial, Coronal, Sagittal , None };

    /// tipus de fusió dels models
    enum OverlayType{ Blend , CheckerBoard , RectilinearWipe };

    /// Aquests flags els farem servir per decidir quines anotacions seran visibles i quines no
    enum AnnotationFlags{ NoAnnotation = 0x0 , WindowInformationAnnotation = 0x1 , PatientOrientationAnnotation = 0x2 , RulersAnnotation = 0x4 , SliceAnnotation = 0x8, PatientInformationAnnotation = 0x10, AcquisitionInformationAnnotation = 0x20, ScalarBarAnnotation = 0x40,
    AllAnnotation = 0x7F };

    Q2DViewer( QWidget *parent = 0 );
    ~Q2DViewer();

    virtual vtkRenderer *getRenderer();
    virtual void setInput( Volume* volume );

    /// Retorna l'interactor style
    vtkInteractorStyleImage *getInteractorStyle();

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

    /// Canviem l'opacitat del volum solapat
    void setOpacityOverlay ( double op );

    // Mètodes específics checkerboard
    /// Obtenim el nombre de divisions
    int * getDivisions();
    void getDivisions( int data[3] );

    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int data[3] );

    /// Desa totes les llesques que es veuen al visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveAll( const char *baseName , FileType extension );

    /// Desa la vista actual del visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveCurrent( const char *baseName , FileType extension );

    /// Obté el window level actual de la imatge
    double getCurrentColorWindow();
    double getCurrentColorLevel();

    /**
        Mètodes pel tractament de múltiples vistes
    */
    /// Assignar/afegir files i columnes
    void setRows( int rows );
    void addRows( int rows );
    void removeRows( int rows );
    void setColumns( int columns );
    void addColumns( int columns );
    void removeColumns( int columns );
    void setGrid( int rows, int columns );
    /// Donada una llesca, li diem en quantes files i columnes volem dividir les seves fases
    void setPhaseRows( int slice, int rows );
    void addPhaseRows( int slice, int rows );
    void removePhaseRows( int slice, int rows );
    void setPhaseColumns( int slice, int columns );
    void addPhaseColumns( int slice, int columns );
    void removePhaseColumns( int slice, int columns );
    void setPhaseGrid( int slice, int rows, int columns );

    /// Indiquem quina informació mostrem
    void updateInformation();

public slots:
    void eventHandler( vtkObject * obj, unsigned long event, void * client_data, void *call_data, vtkCommand * command );

    /// Habilita/deshabilita que es vegi la info de voxel
    void setVoxelInformationCaptionEnabled( bool enable );
    void enableVoxelInformationCaption();
    void disableVoxelInformationCaption();

    /// retorna la llesca actual
    int getSlice( void ){ return m_currentSlice; }

    /// canvia la llesca que veiem de la vista actual
    void setSlice(int value);

    /// canvia la fase en que es veuen les llesques si n'hi ha
    void setPhase( int value );

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

    void reset();

    // Mètodes específics checkerboard
    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int x , int y , int z );

    void contextMenuRelease( vtkObject *object, unsigned long event, void *client_data, void *call_data, vtkCommand * command);

    /// Afegir o treure la visibilitat d'una anotació textual/gràfica
    void enableAnnotation( AnnotationFlags annotation, bool enable = true );
    void removeAnnotation( AnnotationFlags annotation );

    /// Ajusta el window/level
    void setWindowLevel( double window , double level );

    /// Obté el window/level original
    void getDefaultWindowLevel( double wl[2] );

    /// Obté el window level actual de la imatge
    void getCurrentWindowLevel( double wl[2] );

    /// Reseteja el window level al que tingui per defecte el volum
    void resetWindowLevelToDefault();

    /// Actualitza la informació del voxel que hi ha per sota del cursor
    void updateVoxelInformation();

    void setTool( QString toolName );

    /// Obté la tool que li demanem. \TODO Mètode "temporal" (o no) per poder accedir a les dades d'una tool; per exemple, si tenim la tool de seeds, per certes extensions necessitarem obtenir el valor de la seed, i només la SeedTool té el mètode específic per accedir a aquestes dades
    Tool *getTool( QString toolName );

    void setEnableTools( bool enable );
    void enableTools();
    void disableTools();

    /// \TODO Per poder obtenir la llavor que s'ha marcat amb la tool SeedTool. Posar la llavor
    void setSeedPosition( double pos[3] );

    /// Obtenir la llavor
    void getSeedPosition( double pos[3] );

    /// Aplica una rotació de 90 graus en el sentit de les agulles del rellotge
    void rotateClockWise();

    /// Aplica una rotació de 90 graus en el sentit contrari a les agulles del rellotge
    void rotateCounterClockWise();

protected:
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

    /// La fase (instant de temps) de les llesques
    int m_currentPhase;

    /// Aquest és el segon volum afegit a solapar
    Volume* m_overlayVolume;

    /// Aquest és el blender per veure imatges fusionades
    vtkImageBlend* m_blender;

    /// Opacitat del segon volume
    double m_opacityOverlay;

    /// El nombre de divisions per cada dimensió
    int m_divisions[3];

    /// El picker per anotar punts de la imatge
    vtkPropPicker *m_picker;

    /// Annotacions de texte referents a informació de la sèrie (nom de pacient, protocol,descripció de sèrie, data de l'estudi)
    vtkCornerAnnotation *m_serieInformationAnnotation;

    /// Actualitza la vista en el rendering
    void updateView();

    // Processem l'event de resize de la finestra Qt
    virtual void resizeEvent( QResizeEvent* resize );

private:
    /// flag que ens indica quines anotacions es veuran per la finestra
    unsigned int m_enabledAnnotations;

    /// Refresca les anotacions que s'han de veure i les que no \TODO no fa res
    void updateAnnotations();

    /// inicialitza les annotacions de texte
    void initTextAnnotations();

    /// Tipus de solapament dels volums en cas que en tinguem més d'un
    OverlayType m_overlay;

    /// configuració de la interacció amb l'usuari
    void setupInteraction();

    /// Crea i inicialitza totes les accions d'aquest widget
    void createActions();

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

    /// Mides (x,y) de la imatge que mostrarem com informació adicional
    int m_imageSizeInformation[2];

    /// Valors dels window level per defecte. Pot venir donat pel DICOM o assignat per nosaltres a un valor estàndar de constrast
    double m_defaultWindow, m_defaultLevel;

    /// Llegenda que segueix el cursor amb el valor del voxel
    vtkCaptionActor2D *m_voxelInformationCaption;

    /// Barra que mostra l'escala de colors del model que estem visualitzant \TODO quan tinguem models fusionats tindrem una o dues barres d'escala de colors?
    vtkScalarBarActor *m_scalarBar;

    /// El manager de les tools
    Q2DViewerToolManager *m_toolManager;

    ///Informació de la posició de la seed
    double m_seedPosition[3];

    /// per controlar si la info de voxel està habilitada o no
    bool m_voxelInformationEnabled;

    /// Factor de rotació. En sentit de les agulles del rellotge 0: 0º, 1: 90º, 2: 180º, 3: 270º.
    int m_rotateFactor;

    /**
        Membres pel tractament de múltiples vistes
    */

    /// Afegeix/Eliminia un nou renderer amb imatge, actors, etc
    void addRenderScene();
    void removeRenderScene();

    /// actualitza la distibució de files i columnes (a nivell de llesques)
    void updateGrid();

    /// actualitza la distibució de files i columnes (a nivell de fases)
    void updatePhaseGrid( int slice );

    /// actualitza la distribució de viewports
    void updateViewports();

    /// Actualitza les característiques dels actors dels viewports
    void updateDisplayExtent();

    /// crea i retorna un ruler stàndar
    vtkAxisActor2D* createRuler();

    /// controla el nombre de files i columnes en que es distribueix el visor
    int m_columns, m_rows;

    /// Controla el nombre de llesques en que es subdivideix la finestra
    int m_numberOfSlicesWindows;

    /// per controlar els marges de la zona on es mostraran les llesques (xMin,yMin,xMax,yMax)
    double m_slicesViewportExtent[4];

    /// Col·lecció d'actors per cada llesca
    vtkPropCollection *m_sliceActorCollection;

    /// Col·lecció de renderers per cada llesca
    vtkRendererCollection *m_rendererCollection;

    /// Col·lecció de coordenades dels rulers \TODO canviar el vtkCollection per alguna estructura tipu stl::list<>
    vtkCollection *m_anchoredRulerCoordinatesCollection;

    /// Col·lecció de rulers per cada viewport
    vtkActor2DCollection *m_rulerActorCollection;

    /// Col·lecció d'anotacions per cada viewport
    vtkActor2DCollection *m_sliceAnnotationsCollection;

    /// ampliació tractament dinàmic
    /// Nombre de fases
    int m_numberOfPhases;

    /// Valor màxim al que pot arribar m_currentSlice
    int m_maxSliceValue;

    /// Mapa que guarda la distribució en graella de les fases d'una llesca ( si en té )
    QMap<int, int *> m_phaseGridMap;

private slots:
    /// Actualitza la rotació de la càmera \sa rotateClockWise() i rotateCounterClockWise()
    void updateCameraRotation();

signals:
    /// envia la nova llesca en la que ens trobem
    void sliceChanged(int);

    /// indica el nou window level
    void windowLevelChanged( double window , double level );

    /// Senyal que s'envia quan la llavor s'ha canviat \TODO mirar de treure-ho i posar-ho en la tool SeedTool
    void seedChanged();

};

};  //  end  namespace udg

#endif
