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
class vtkPropPicker;
class vtkTextActor;
class vtkObject;
class vtkCommand;
class vtkRenderer;
class vtkCornerAnnotation;
class vtkAxisActor2D;
class vtkWindowToImageFilter;
class vtkCoordinate;
class vtkScalarBarActor;
class vtkImageBlend;
class vtkImageActor;
class vtkImageData;
class vtkInteractorStyleImage;
// grayscale pipeline
class vtkImageMapToWindowLevelColors;
class vtkImageShiftScale;
class vtkWindowLevelLookupTable;
class vtkProjectionImageFilter;

namespace udg {

// Fordward declarations
class Volume;
class Image;
class Drawer;
class Q2DViewerToolManager;
class OldTool;
class ImagePlane;

/**

Classe base per als visualitzadors 2D.
El mode d'operació habitual serà el de visualitar un sol volum.
Normalment per poder visualitzar un volum farem el següent
\code
Q2DViewer* visor = new Q2DViewer();
visor->setInput( volum );
visor->resetView( Q2DViewer::Axial );
visor->render();

En el cas que desitjem solapar dos volums haurem d'indicar el volum solapat mab el mètode setOverlayInput().
Quan solapem volums tenim 3 maneres de solapar aquests volums, amb un patró de checkerboard, aplicant un blending o un rectilinearWipe, en aquest cas hauríem de fer servir el mètode setOverlay() indicant una de les opcions, Blend, CheckerBoard o RectilinearWipe
\TODO acabar la doc sobre solapament

Per defecte el visualitzador mostra la llesca central de la vista seleccionada, si volem canviar al llesca farem servir el mètode setSlice().

Podem escollir quines annotacions textuals i de referència apareixeran en la vista 2D a través dels flags "AnnotationFlags" definits com enums. Aquests flags es poden passar en el constructor o els podem modificar a través dels mètodes \c addAnnotation() o \c removeAnnotation() que faran visible o invisible l'anotació indicada. Per defecte el flag és \c AllAnnotation i per tant es veuen totes les anotacions per defecte.

@author Grup de Gràfics de Girona  ( GGG )
*/
class Q2DViewer : public QViewer{
Q_OBJECT
public:
    /// tipus de fusió dels models
    enum OverlayType{ None, Blend , CheckerBoard , RectilinearWipe };

    /// Aquests flags els farem servir per decidir quines anotacions seran visibles i quines no
    enum AnnotationFlag{ NoAnnotation = 0x0 , WindowInformationAnnotation = 0x1 , PatientOrientationAnnotation = 0x2 , RulersAnnotation = 0x4 , SliceAnnotation = 0x8, PatientInformationAnnotation = 0x10, AcquisitionInformationAnnotation = 0x20, ScalarBarAnnotation = 0x40,
    AllAnnotation = 0x7F };
    Q_DECLARE_FLAGS(AnnotationFlags, AnnotationFlag)

    Q2DViewer( QWidget *parent = 0 );
    ~Q2DViewer();

    virtual vtkRenderer *getRenderer();
    virtual void setInput( Volume* volume );

    void resetView( CameraOrientationType view );
    void resetViewToAxial();
    void resetViewToCoronal();
    void resetViewToSagital();

    /// ens retorna la vista que tenim en aquells moments del volum
    CameraOrientationType getView() const;

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

    /// Desa la vista actual del visor amb el nom de fitxer base \c baseName i en format especificat per \c extension
    void saveCurrent( QString baseName , FileType extension );

    /// Obté el window level actual de la imatge
    double getCurrentColorWindow();
    double getCurrentColorLevel();

    /// retorna la llesca/fase actual
    int getCurrentSlice() const;
    int getCurrentPhase() const;

    void getCurrentWindowLevel( double wl[2] );

    /// Obté la tool antiga que li demanem.
    /// \TODO Mètode "temporal" per poder accedir a les dades d'una tool;
    /// per exemple, si tenim la tool de seeds, per certes extensions necessitarem
    /// obtenir el valor de la seed, i només la SeedTool té el mètode específic per accedir a aquestes dades
    /// S'eliminara quan ens desfem de les tools antigues
    OldTool *getOldTool( QString toolName );

    /// Retorna el nom de la tool antiga activa
    /// TODO eliminar quan el sistema de tools antigues
    /// s'hagi eliminat del core
    QString getCurrentOldToolName();

    /// Obtenir la llavor
    void getSeedPosition( double pos[3] );

    /// Mètodes de conveniència pels presentation state
    void setModalityRescale( vtkImageShiftScale *rescale );

    /**
     * Ens retorna el drawer per poder pintar-hi primitives
     * @return Objecte drawer del viewer
     */
    Drawer *getDrawer() const;

    /**
     * Calcula la posició del cursor en coordenades de món
     * @param xyz[] La posició que obtenim
     * @return Cert si el cursor està sobre una posició vàlida de la imatge, fals altrament
     */
    bool getCurrentCursorPosition( double xyz[3] );

    /**
     * Retorna el valor del voxel allà on es troba el cursor en aquell moment
     * @return valor del voxel
     */
    double getCurrentImageValue();

    //
    // DISPLAYED AREA. Mètodes per poder modificar l'àrea visible del volum (zoom enquadrat) i/o canviar aspecte, espaiat de presentació, etc
    //
    /// Canviem l'aspecte entre els eixos x/y
    void setPixelAspectRatio( double ratio );

    /// canviem l'espaiat de pixel en la presentació en les direccions x/y ( no l'espaiat del volum en sí )
    void setPresentationPixelSpacing( double x, double y );

    /// En aquest mode, es presenta la imatge de tal manera que les mides estan a escala real, per tant 1mm de la imatge en pantalla seria 1 mm real d'aquella llesca. \TODO aquest mètode encara no està en funcionament, però el deixem per implementar en un futur pròxim
    void setTrueSizeMode( bool on = true );

    /// Magnifica la imatge en les direccions X/Y pel factor donat. Si el factor és < 0.0 llavors la imatge es "minifica"
    void setMagnificationFactor( double factor );

    /**
     * Ens retorna la imatge que s'està visualitzant en aquell moment
     * TODO pot ser que aquest mètode sigui purament de conveniència per la tool de reference lines i desaparegui
     * en un futur o sigui substituit per un altre
     * @return Objecte Image corresponent al que es visualitza en aquell moment. NULL si no es veu cap imatge
     */
    Image *getCurrentDisplayedImage() const;

    /**
     * Ens dóna el pla d'imatge actual que estem visualitzant
     * @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
     * per casos en que el pla "real" no és el que volem i necessitem una petita modificació
     * ATENCIÓ: Només es donarà aquest paràmetre (amb valor true) en casos que realment se sàpiga el que s'està fent!
     * @return El pla imatge actual
     */
    ImagePlane *getCurrentImagePlane( bool vtkReconstructionHack = false );

    /**
     * Ens dóna el pla d'imatge de la llesca
     * @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
     * per casos en que el pla "real" no és el que volem i necessitem una petita modificació
     * ATENCIÓ: Només es donarà aquest paràmetre (amb valor true) en casos que realment se sàpiga el que s'està fent!
     * @return El pla imatge de la llesca
     */
    ImagePlane *getImagePlane( int sliceNumber, int phaseNumber, bool vtkReconstructionHack = false );

    /**
     * donat un punt 3D en espai de referència DICOM, ens dóna la projecció d'aquest punt sobre
     * el pla actual, transformat a coordenades de món VTK
     * @param pointToProject[]
     * @param projectedPoint[]
     * @param vtkReconstructionHack HACK variable booleana que ens fa un petit hack
     * per casos en que el pla "real" no és el que volem i necessitem una petita modificació
     */
    void projectDICOMPointToCurrentDisplayedImage( const double pointToProject[3], double projectedPoint[3], bool vtkReconstructionHack = false );

    /// Retorna el thickness
    double getThickness();

    /**
     * Ens retorna el rang actual de llesques, tenint en compte totes les imatges,
     * tant com si hi ha fases com si no
     * @param min valor mínim
     * @param max valor màxim
     */
    void getSliceRange(int &min, int &max);
    int *getSliceRange();

    /**
     * Ens dóna la llesca mínima/màxima de llesques, tenint en compte totes les imatges,
     * tant com si hi ha fases com si no
     * @return valor de la llesca mínima/màxima
     */
    int getMinimumSlice();
    int getMaximumSlice();

    /**
     * Ens indica si s'està aplicant o no thick slab
     * @return
     */
    bool isThickSlabActive() const;

    /**
     * Obtenim el mode de projecció del thickslab.
     * Si el thickslab no està actiu, el valor és indefinit
     * @return
     */
    int getSlabProjectionMode() const;

    /**
     * Obtenim el gruix de l'slab
     * Si el thickslab no està actiu, el valor és indefinit
     * @return
     */
    int getSlabThickness() const;

    /// Donada les coordenades x,y d'on s'ha clicat de la pantalla, retorna el punt més proper i que caigui dins del model.
    double * pointInModel( int screen_x, int screen_y );

    ///Retorna la informació de la llesca actual del visualitzador
    vtkImageData* getCurrentSlabProjection();

    /// Busca la llesca que es troba més a prop del punt i retorna la distancia
    int getNearestSlice( double point[3], double &distance );

public slots:
    virtual void render();
    void reset();
    /// Metodes per activar les Tools antigues.
    /// Per poder-les fer servir cal activar-les explicitament
    /// Per defecte estan desactivades
    /// TODO Quan ens desfem de totes les eines
    /// antigues aquests metodes han de desapareixer
    void setOldTool( QString toolName );
    void setEnableOldTools( bool enable );
    void enableOldTools();
    void disableOldTools();

    /// canvia la llesca que veiem de la vista actual
    void setSlice(int value);

    /// canvia la fase en que es veuen les llesques si n'hi ha
    void setPhase( int value );

    /// indica el tipu de solapament dels volums, per defecte checkerboard
    void setOverlay( OverlayType overlay );
    void setNoOverlay();
    void setOverlayToBlend();
    void setOverlayToCheckerBoard();
    void setOverlayToRectilinearWipe();

    // Mètodes específics checkerboard
    /// Indiquem el nombre de divisions del checkerboard
    void setDivisions( int x , int y , int z );

    /// Afegir o treure la visibilitat d'una anotació textual/gràfica
    void enableAnnotation( AnnotationFlags annotation, bool enable = true );
    void removeAnnotation( AnnotationFlags annotation );

    void setWindowLevel(double window, double level);
    void resetWindowLevelToDefault();

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
    vtkImageMapToWindowLevelColors *getWindowLevelMapper() const;

    ///
    void setModalityRescale( double slope, double intercept );
    void setModalityLUT( vtkWindowLevelLookupTable *lut );
    void setVOILUT( vtkWindowLevelLookupTable *lut );
    void setPresentationLUT( vtkWindowLevelLookupTable *lut );

    /// Aplica el pipeline d'escala de grisos segons la modality, voi i presentation lut's que s'hagin calculat. Això permet que el càlcul s'hagi fet en un presentation state, per exemple
    void applyGrayscalePipeline();

    // TODO aquests mètodes també haurien d'estar en versió QString!
    /**
     * Li indiquem quin mode de projecció volem aplicar sobre l'slab
     * @param projectionMode valor que identifica quina projecció apliquem
     */
    void setSlabProjectionMode( int projectionMode );

    /**
     * Indiquem el gruix de l'slab
     * @param thickness Nombre de llesques que formen l'slab
     */
    void setSlabThickness( int thickness );

    /**
     * Activem a desactivem l'aplicació del thick slab sobre la imatge
     * @param enable
     */
    void enableThickSlab( bool enable = true );

signals:
    /// envia la nova llesca en la que ens trobem
    void sliceChanged(int);

    /// envia la nova fase en la que ens trobem
    void phaseChanged(int);

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

    /**
     * S'emet quan canvia l'slab thickness
     * @param thickness nou valor de thickness
     */
    void slabThicknessChanged(int thickness);

protected:
    /// Processem l'event de resize de la finestra Qt
    virtual void resizeEvent( QResizeEvent* resize );

private:
    /**
     * Refresca la visibilitat de les annotacions en funció dels flags que tenim
     */
    void refreshAnnotations();

    /// Actualitzem les dades de les annotacions, per defecte totes, sinó, només les especificades
    void updateAnnotationsInformation( AnnotationFlags annotation = Q2DViewer::AllAnnotation );

    /// Desglossem les actualitzacions de les diferents informacions que es mostren per pantalla
	void updatePatientAnnotationInformation();
    void updateSliceAnnotationInformation();

    /// Refresca els valors de les annotacions de llesca. Si els valors referents
    /// a les fases són < 2 no es printarà informació de fases
    /// Si hi ha thick slab, mostrarà el rang d'aquest
    void updateSliceAnnotation( int currentSlice, int maxSlice, int currentPhase = 0, int maxPhase = 0 );

    /// configuració de la interacció amb l'usuari
    void setupInteraction();

    /// Crea i inicialitza totes les anotacions que apareixeran per pantalla
    void createAnnotations();

    /// crea els indicadors d'escala
    void createRulers();

    /// Crea la barra de valors
    void createScalarBar();

    /// Actualitza la barra de valors
    void updateScalarBar();

    /// crea les anotacions de l'orientació del pacient
    void createOrientationAnnotations();

    /// Afegeix tots els actors a l'escena
    void addActors();

    /// Helper method, donada una etiqueta d'orientació, ens retorna aquesta etiqueta però amb els valors oposats.
    /// Per exemple, si l'etiqueta que ens donen és RPI (Right-Posterior,Inferior), el valor retornat seria LAS (Left-Anterior-Superior)
    /// Les etiquetes vàlides i els seus oposats són les següents:
    /// R:L (Right-Left), A:P (Anterior-Posterior), S:I (Superior-Inferior), H:F(Head-Feet)
    static QString getOppositeOrientationLabel( const QString &label );

    /// A partir de l'string d'orientació del pacient mapeja les anotacions correctes segons com estem mirant el model. A això li afecta també si la vista és axial, sagital o coronal
    void mapOrientationStringToAnnotation();

    /// Actualitza les característiques dels actors dels viewports
    void updateDisplayExtent();

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

    /// thick slab
    void computeRangeAndSlice( int newSlabThickness );

    ///  Valida el valor d'slice donat i actualitza les variables membres pertinents, com m_currentSlice o m_firstSlabSlice
    void checkAndUpdateSliceValue( int value );

private slots:
    /// Actualitza les transformacions de càmera ( de moment rotació i flip )
    void updateCamera();

    /// Actualitza els rulers
    void updateRulers();

    /// thick slab
    void setupDefaultPipeline();
    void setupThickSlabPipeline();

protected:
    /// Connector d'events vtk i slots qt
    vtkEventQtSlotConnect *m_vtkQtConnections;

    /// Renderer principal. S'encarrega de pintar la imatge
    vtkRenderer *m_imageRenderer;

    /// Actor d'imatge
    vtkImageActor *m_imageActor;

    /// Interactor Style
    vtkInteractorStyleImage *m_interactorStyle;

    /// conserva la vista actual
    CameraOrientationType m_lastView;

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

    /// Annotacions de texte referents a informació de la sèrie 
	/// (nom de pacient, protocol,descripció de sèrie, data de l'estudi, etc)
	/// i altre informació rellevant ( nº imatge, ww/wl, etc )
    vtkCornerAnnotation *m_cornerAnnotations;

    /// Per controlar l'espaiat en que presentem la imatge
    double m_presentationPixelSpacing[2];

private:
    /// flag que ens indica quines anotacions es veuran per la finestra
    AnnotationFlags m_enabledAnnotations;

    /// Tipus de solapament dels volums en cas que en tinguem més d'un
    OverlayType m_overlay;

    /// Els strings amb els textes de cada part de la imatge
    QString m_lowerLeftText, m_lowerRightText, m_upperLeftText, m_upperRightText;

    /// Aquest string indica les anotacions que ens donen les referències del pacient ( Right,Left,Posterior,Anterior,Inferior,Superior) TODO aquesta variable no s'està fent servir :( decidir si ens pot resultar útil o no
    QString m_patientOrientationText[4];

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

    /// Barra que mostra l'escala de colors del model que estem visualitzant \TODO quan tinguem models fusionats tindrem una o dues barres d'escala de colors?
    vtkScalarBarActor *m_scalarBar;

    /// El manager de les tools
    Q2DViewerToolManager *m_oldToolManager;

    ///Informació de la posició de la seed
    double m_seedPosition[3];

    /// Factor de rotació. En sentit de les agulles del rellotge 0: 0º, 1: 90º, 2: 180º, 3: 270º.
    int m_rotateFactor;

    /// ampliació tractament dinàmic
    /// Nombre de fases
    int m_numberOfPhases;

    /// Valor màxim al que pot arribar m_currentSlice
    int m_maxSliceValue;

    /// Indica si cal aplicar un flip horitzontal o no sobre la càmera
    bool m_applyFlip;

    /// Aquesta variable controla si la imatge està flipada respecte la seva orientació original. Útil per controlar annotacions.
    bool m_isImageFlipped;

    /// Rangs de dades que ens seran força útils a l'hora de controlar el pipeline de grayscale
    double m_modalityRange[2];

    /// Especialista en dibuixar primitives
    Drawer *m_drawer;

    /// objectes per a les transformacions en el pipeline d'escala de grisos
    vtkImageMapToWindowLevelColors *m_windowLevelLUTMapper;
    vtkImageShiftScale *m_modalityLUTRescale;

    /// Les diferents look up tables que ens podem trobar durant tot el procés.
    vtkWindowLevelLookupTable *m_modalityLut, *m_windowLevelLut, *m_presentationLut;

    /// Variable que controla si les tools estant habilitades
    /// Aquesta variable evita que es faci més d'un "connect"
    /// quan es fa l'enableOldTools i ja estan habilitades
    bool m_enabledOldTools;

    // Secció "ThickSlab"
    /// Nombre de llesques que composen el thickSlab
    int m_slabThickness;

    /// Filtre per composar les imatges del thick slab
    vtkProjectionImageFilter *m_thickSlabProjectionFilter;

    /// Variables per controlar el rang de llesques en el que es troba l'slab
    int m_firstSlabSlice, m_lastSlabSlice;

    /// Variable per controlar si el thickSlab s'està aplicant o no.
    /// Perquè sigui true, m_slabThickness > 1, altrament false
    bool m_thickSlabActive;

    /// Indica quin tipus de projecció apliquem sobre l'slab
    int m_slabProjectionMode;

};
Q_DECLARE_OPERATORS_FOR_FLAGS(Q2DViewer::AnnotationFlags)
};  //  end  namespace udg

#endif
