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
class vtkCornerAnnotation;
class vtkAxisActor2D;
class vtkWindowToImageFilter;
class vtkCoordinate;
class vtkCaptionActor2D;
class vtkScalarBarActor;
class vtkInteractorStyleImage;
class vtkImageBlend;
class vtkImageActor;
class vtkProp;

/// tractament múltiples vistes
class vtkPropCollection;
class vtkRendererCollection;
class vtkCollection;
class vtkActor2DCollection;

// grayscale pipeline
class vtkImageMapToWindowLevelColors;
class vtkImageShiftScale;
class vtkWindowLevelLookupTable;

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
class SliceAnnotationController;

class Q2DViewer : public QViewer{
Q_OBJECT
public:
    /// Axial: XY, Coronal: XZ, Sagittal: YZ
    enum ViewType{ Axial, Coronal, Sagittal };

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
    int *getDivisions();
    void getDivisions( int data[3] );

    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int data[3] );

    /// Desa totes les llesques que es veuen al visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveAll( QString baseName , FileType extension );

    /// Desa la vista actual del visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveCurrent( QString baseName , FileType extension );

    /// Obté el window level actual de la imatge
    double getCurrentColorWindow();
    double getCurrentColorLevel();

    /// retorna la llesca actual
    int getSlice( void ){ return m_currentSlice; }

    /// Obté el window/level original
    void getDefaultWindowLevel( double wl[2] );

    /// Obté el window level actual de la imatge
    void getCurrentWindowLevel( double wl[2] );

    /// Obté la tool que li demanem. \TODO Mètode "temporal" (o no) per poder accedir a les dades d'una tool; per exemple, si tenim la tool de seeds, per certes extensions necessitarem obtenir el valor de la seed, i només la SeedTool té el mètode específic per accedir a aquestes dades
    Tool *getTool( QString toolName );

    /// Retorna el nom de la tool activa
    QString getCurrentToolName();

    /// A través d'aquest mètode afegirem actors associats a llesca i vista que seran visibles segons la vista i llesca en la que es trobin \TODO de moment aquest mètode és un "HELPER METHOD". Segurament aquesta funcionalitat acabarà a passar a ser competència de la futura classe Drawer destinada a pintar annotacions
    void addSliceAnnotation( vtkProp *actor, int slice, int view );

    /// Obtenir la llavor
    void getSeedPosition( double pos[3] );

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

    /// Mètodes de conveniència pels presentation state
    void setModalityRescale( vtkImageShiftScale *rescale ){ m_modalityLUTRescale = rescale; };
    vtkImageActor *getImageActor();

    //
    // DISPLAYED AREA. Mètodes per poder modificar l'àrea visible del volum (zoom enquadrat) i/o canviar aspecte, espaiat de presentació, etc
    //
    /// Canviem l'aspecte entre els eixos x/y
    void setPixelAspectRatio( double ratio );

    /// canviem l'espaiat de pixel en la presentació en les direccions x/y ( no l'espaiat del volum en sí )
    void setPresentationPixelSpacing( double x, double y );

    /// Fem un zoom del requadre definit pels paràmetres topLeft i rightBottom en coordenades de món perquè s'ajusti a la mida de la finestra
    void scaleToFit( double topLeftX, double topLeftY, double bottomRightX, double bottomRightY );

    /// En aquest mode, es presenta la imatge de tal manera que les mides estan a escala real, per tant 1mm de la imatge en pantalla seria 1 mm real d'aquella llesca. \TODO aquest mètode encara no està en funcionament, però el deixem per implementar en un futur pròxim
    void setTrueSizeMode( bool on = true );

    /// Magnifica la imatge en les direccions X/Y pel factor donat. Si el factor és < 0.0 llavors la imatge es "minifica"
    void setMagnificationFactor( double factor );

public slots:
    void eventHandler( vtkObject * obj, unsigned long event, void * client_data, void *call_data, vtkCommand * command );

    /// Habilita/deshabilita que es vegi la info de voxel
    void setVoxelInformationCaptionEnabled( bool enable );
    void enableVoxelInformationCaption();
    void disableVoxelInformationCaption();

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

    /// Ajusta els ÚNICAMENT els valors de window i level per defecte. Mètode de conveniència pels presentation states
    void setDefaultWindowLevel( double window, double level )
    {
        m_defaultWindow = window;
        m_defaultLevel = level;
    };

    /// Ajusta el window/level
    void setWindowLevel( double window , double level );

    /// Reseteja el window level al que tingui per defecte el volum
    void resetWindowLevelToDefault();

    /// Actualitza la informació del voxel que hi ha per sota del cursor
    void updateVoxelInformation();

    void setTool( QString toolName );

    void setEnableTools( bool enable );
    void enableTools();
    void disableTools();

    /// \TODO Per poder obtenir la llavor que s'ha marcat amb la tool SeedTool. Posar la llavor
    void setSeedPosition( double pos[3] );

    /// Aplica una rotació de 90 graus en el sentit de les agulles del rellotge
    void rotateClockWise();

    /// Aplica una rotació de 90 graus en el sentit contrari a les agulles del rellotge
    void rotateCounterClockWise();

    /// Aplica una rotació absoluta de (90º * factor) mod 360 sobre la imatge. Per tant es fa la rotació com si la imatge estigués amb una rotació inicial de 0º
    void setRotationFactor( int factor );

    /// Aplica un flip horitzontal/vertical sobre la imatge. El flip vertical es farà com una rotació de 180º seguida d'un flip horitzontal
    void horizontalFlip();
    void verticalFlip();

    /// Re-inicia la càmera en la vista actual. Posa els paràmetres de rotació, zoom, desplaçament, flip, etc. als seus valors inicials
    void resetCamera();

    ///Mètode de conveniència pel tractament dels presentation states
    vtkImageMapToWindowLevelColors *getWindowLevelMapper() const { return m_windowLevelLUTMapper; }

    ///
    void setModalityRescale( double slope, double intercept );
    void setModalityLUT( vtkWindowLevelLookupTable *lut );
    void setVOILUT( vtkWindowLevelLookupTable *lut );
    void setPresentationLUT( vtkWindowLevelLookupTable *lut );

    /// Aplica el pipeline d'escala de grisos segons la modality, voi i presentation lut's que s'hagin calculat. Això permet que el càlcul s'hagi fet en un presentation state, per exemple
    void applyGrayscalePipeline();

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

    /// Col·lecció per guardar les informacions de cada llesca per tal que es mostrin el tamany d'acord amb el grid especificat per la llesca
    vtkActor2DCollection *m_informationCollection;

    /// Per controlar l'espaiat en que presentem la imatge
    double m_presentationPixelSpacing[2];

    /// Controlador d'annotacions per llesca
    SliceAnnotationController *m_sliceAnnotationController;

    /// Processem l'event de resize de la finestra Qt
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
    vtkScalarBarActor * createScalarBar();

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

    /// Col·lecció per guardar tots els scalarBar de cada render, per tal que cadascún tingui la mida d'acord amb el render que la conté.
    vtkActor2DCollection *m_scalarBarCollection;

    /// Col·lecció d'anotacions per cada viewport
    vtkActor2DCollection *m_sliceAnnotationsCollection;

    /// ampliació tractament dinàmic
    /// Nombre de fases
    int m_numberOfPhases;

    /// Valor màxim al que pot arribar m_currentSlice
    int m_maxSliceValue;

    /// Mapa que guarda la distribució en graella de les fases d'una llesca ( si en té )
    QMap<int, int *> m_phaseGridMap;

    /// Indica si cal aplicar un flip horitzontal o no sobre la càmera
    bool m_applyFlip;

    /// Aquesta variable controla si la imatge està flipada respecte la seva orientació original. Útil per controlar annotacions.
    bool m_isImageFlipped;

    /// Rangs de dades que ens seran força útils a l'hora de controlar el pipeline de grayscale
    double m_modalityRange[2];

    /// Fa els càlculs del pipeline de l'escala de grisos del volum d'entrada. És a dir calcularà la modality lut, voi lut i presentation lut però no l'aplicarà
    void computeInputGrayscalePipeline();

    /**
     * Aplica la transformació de modalitat sobre la imatge. Ens podem trobar amb que tenim un presentation state associat o no.
     1.- En el cas que no hi hagi un presentation state associat ens podem trobar amb les següents situacions:

        1.1. La imatge original té una modality LUT. La llegim i l'apliquem al principi del pipeline. El seu input són les dades originals del volum.

        1.2. La imatge original conté rescale slope i rescale intercept. No cal fer res, el lector d'itk (GDCM) ja aplica aquesta transformació automàticament quan llegim el volum.

        1.3. No hi ha cap informació referent a modality LUTs. No cal fer res. Aplicar transformació identitat

     2.- En en cas que tinguem un presentation state associat, pot passar el següent

        2.1. El PS té una modality LUT. ídem 1.1

        2.2. El PS conté rescale slope i rescale intercept. Cal aplicar un filtre de rescale (vtImageShiftScale). \TODO Com que la imatge que llegim ja té aplicat el rescale/intercept de la imatge, no sabem si cal contrestar primer la transformació que ens vé de "gratis".

        2.3. ídem 1.3 \TODO però no sabem si en el cas que tinguem un rescalat de "gratis" caldria desfer-lo, és a dir, donar els raw pixels!

    Els canvis de la modality LUT s'apliquen a totes les imatges contingudes en el volum
     */
    void computeModalityLUT();
    ///
    void applyMaskSubstraction();

    /**
     *  Aplica l'ajustament de finestra sobre la imatge. Ens podem trobar amb que tenim un presentation state associat o no.

     1.- En el cas que no hi hagi un presentation state associat ens podem trobar amb les següents situacions:

        1.1. La imatge original té una VOI LUT. La llegim i l'apliquem com a input del window level mapper. \TODO problema: no sabem ben bé què passa si abans teníem una modality LUT. En principi hauríem de fer servir el mapeig sobre l'anterior lut.

        1.2. La imatge original conté valors de window level. Apliquem aquests valors sobre el window level mapper.

        1.3. No hi ha cap informació referent a VOI LUTs. No cal fer res. Aplicar transformació identitat, és a dir el window serà el rang de dades i el level el window/2.

     2.- En en cas que tinguem un presentation state associat, actuem igual que en 1.x. Els valors del presentation state prevalen sobre els de la imatge.

    Es pot tenir més d'una VOI LUT (ja sigui en format de LUT o de window level). Això significa que tenim diverses opcions de presentació. Es pot agafar una per defecte però l'aplicació hauria de mostrar la possiblitat d'escollir entre aquestes.

    Els canvis de la VOI LUT es poden aplicar a sub-conjunts d'imatges referenciades. Això es donarà en el cas d'imatges multi-frame.

    De cares al connectathon només es tracta una sola VOI LUT i imatges mono-frame, però hem de tenir en compte que l'estàndar DICOM contempla les possibilitats abans mencionades.
     */
    void computeVOILUT();

    /// objectes per a les transformacions en el pipeline d'escala de grisos
    vtkImageMapToWindowLevelColors *m_windowLevelLUTMapper;
    vtkImageShiftScale *m_modalityLUTRescale;

    /// Les diferents look up tables que ens podem trobar durant tot el procés.
    vtkWindowLevelLookupTable *m_modalityLut, *m_windowLevelLut, *m_presentationLut;

    /// Aquest mètode s'encarrega de donar-nos en format vtk el tipu de lookup table de la grayscale pipeline que ens ofereix el presentation state actualment assignat. type: 0 -> modality lut, 1-> VOI lut, 2-> presentation lut  \TODO mètode temporal
    vtkWindowLevelLookupTable *parseLookupTable( int type );

private slots:
    /// Actualitza les transformacions de càmera ( de moment rotació i flip )
    void updateCamera();

signals:
    /// envia la nova llesca en la que ens trobem
    void sliceChanged(int);

    /// envia la nova vista en la que ens trobem
    void viewChanged(int);

    /// indica el nou window level
    void windowLevelChanged( double window , double level );

    /// Senyal que s'envia quan la llavor s'ha canviat \TODO mirar de treure-ho i posar-ho en la tool SeedTool
    void seedChanged();

    /// informa del valor del m_rotateFactor. S'emetrà quan la rotació de la càmera s'hagi fet efectiva
    void rotationFactorChanged(int);

    /// informa dels graus que ha girat la càmera quan s'ha actualitzat aquest paràmetre
    void rotationDegreesChanged(double);

};

};  //  end  namespace udg

#endif
